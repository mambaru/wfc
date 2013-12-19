#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <fas/xtime.hpp>

namespace wfc{ namespace inet{

template<size_t BufferSize = 8096*2 >
struct ad_stream_writer
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::list<data_ptr> data_list_type;
  typedef ::boost::asio::const_buffer buffer_type;

  static constexpr size_t BUFFER_SIZE=BufferSize;

  ad_stream_writer()
    : _dispatch_count(0)
    , _outgoing_size(0)
  {}
  
  template<typename T>
  struct unique_capture
  {
    unique_capture(std::unique_ptr<T> c): capt(std::move(c)) {};
    unique_capture(const unique_capture& uc): capt(std::move( const_cast<unique_capture&>(uc).capt)) {};
    std::unique_ptr<T> get() { return std::move(capt);};
    std::unique_ptr<T> capt;
  };
  
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    //TODO: dispatch counter для shutdown чтобы не лочить
    std::cout<< "dispatch{" << std::endl;
    ++_dispatch_count;
    unique_capture<data_type> capt( std::move(d) );
    t.socket().get_io_service().dispatch([this, &t, capt]() {
      unique_capture<data_type> cc(capt);
      this->dispatch(t, std::move(cc.get()));
    });
    std::cout<< "}dispatch" << std::endl;
  }
  
  size_t size() const
  {
    return _outgoing_size;
  }
  
private:
  
  template<typename T>
  void dispatch(T& t, data_ptr d)
  {
    --_dispatch_count;

    std::cout<< "dispatch" << std::endl;
    //static int count = 0;
    if ( d==nullptr || d->empty() )
      return;

    //std::cout << "ad_writer " << count++ << ": " << std::string(d->begin(), d->end()) <<  std::endl; 
    if ( !_data_list.empty() )
    {
      _outgoing_size+=d->size();
      _data_list.push_back(std::move(d));
    }
    else
    {
      boost::system::error_code ec;
      size_t s = t.socket().send( ::boost::asio::buffer(d->data(), d->size()), 0, ec);
      
      if (ec)
      {
        s = 0;
        t.get_aspect().template get<_write_error_>()(t,  ec);
      }

      if ( s != d->size() )
      {
        d->erase(d->begin(), d->begin()+s);
        _outgoing_size+=d->size();
        _data_list.push_back(std::move(d));
        do_write(t);
      }
      
      if (!ec)
        t.get_aspect().template get<_on_write_>()(t);
    }
  }

  template<typename T>
  void do_write(T& t)
  {
    if ( _data_list.empty() )
      return;

    // TODO T::owner_type
    typename T::owner_type::weak_type wk = t.owner().alive();
    t.socket().async_write_some(
      ::boost::asio::buffer( _data_list.front()->data(),  _data_list.front()->size() ),
      t.strand().wrap(
      [this, &t, wk](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        auto alive = wk.lock();
        if (!alive)
        {
          t.get_aspect().template get<_alive_error_>()(t);
          return;
        }
        else if (ec)
        {
          t.get_aspect().template get<_write_error_>()(t, ec);
        }
        else if ( bytes_transferred == this->_data_list.front()->size() )
        {
          this->_data_list.pop_front();
          _outgoing_size-=bytes_transferred;
        }
        else
        {
          size_t tail_size = this->_data_list.front()->size() - bytes_transferred;
          
          if ( /*true ||*/ /*tail_size < bytes_transferred ||*/ tail_size < BUFFER_SIZE)
          {
            // std::cout << "==================== truncate =================" <<  std::endl;
            this->_data_list.front()->erase(
              this->_data_list.front()->begin(),
              this->_data_list.front()->begin()+bytes_transferred
            );
            _outgoing_size-=bytes_transferred;
          }
          else
          {
            // std::cout << "---------------------- separate ------------" <<  std::endl;
            // std::cout << "bytes_transferred " << bytes_transferred <<  std::endl;
            size_t buff_size = bytes_transferred;
            // if ( buff_size < BUFFER_SIZE )
              buff_size = BUFFER_SIZE;

            //buff_size = 42;

            data_ptr cur = std::move(this->_data_list.front());
            //std::cout << "cur->size() " << cur->size() <<  std::endl;
            //size_t cut_size = cur->size() - bytes_transferred;
            this->_data_list.pop_front();
            auto itr = this->_data_list.begin();
            auto beg = cur->begin() + bytes_transferred;
            auto end = cur->end();
            for(;beg != end; )
            {
              if ( std::distance(beg, end) <= buff_size )
              {
                //std::cout<< "final!" <<  std::endl;
                this->_data_list.insert( itr, std::make_unique<data_type>(beg, end));
                break;
              }
              
              this->_data_list.insert( itr, std::make_unique<data_type>(beg, beg + buff_size));
              beg+=buff_size;
            }
          }
        }
        
        if (!ec)
          t.get_aspect().template get<_on_write_>()(t);

        this->do_write(t);
      }
      ) // t.strand().wrap(
    );
  }
  
  size_t available() const
  {
    return 0;
    //TODO: переименовать и добавть счетчик
  }
  
private:
  std::atomic<int> _dispatch_count;
  std::atomic<int> _outgoing_size;
  // Блокировка не требуется - всегда вызываеться из потока io_service
  data_list_type _data_list;
};


}}
