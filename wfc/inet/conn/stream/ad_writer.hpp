#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <fas/xtime.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/memory.hpp>
#include <wfc/inet/conn/tags.hpp>


namespace wfc{ namespace inet{ namespace conn{ namespace stream{
  
template<size_t BufferSize = 8096*2 >
struct ad_writer
{
  //typedef std::vector<char> data_type;
  //typedef std::unique_ptr<data_type> data_ptr;
  typedef std::list<data_ptr> data_list_type;
  typedef ::boost::asio::const_buffer buffer_type;

  static constexpr size_t BUFFER_SIZE=BufferSize;

  ad_writer()
    : _outgoing_size(0)
  {}
  
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    _outgoing_size += d->size();
    auto capt = unique_wrap( std::move(d) );
    t.socket().get_io_service().dispatch([this, &t, capt]() {
      this->dispatch(t, unique_unwrap(capt) );
    });
  }
  
  size_t outgoing_size() const
  {
    return _outgoing_size;
  }
  
private:
  
  template<typename T>
  void dispatch(T& t, data_ptr d)
  {
    if ( d==nullptr || d->empty() )
    {
      return;
    }
    
    if ( !_data_list.empty() )
    {
      _data_list.push_back(std::move(d));
    }
    else
    {
      size_t bytes_transferred = t.get_aspect().template get<_send_>()( t, *d );

      _outgoing_size -= bytes_transferred;

      if ( bytes_transferred != d->size() )
      {
        auto beg = d->begin() + bytes_transferred;
        auto end = d->end();
        for(;beg != end; )
        {
          if ( std::distance(beg, end) > static_cast<ptrdiff_t>(BufferSize*2) ) // Последний блок может быть [BufferSize..BufferSize*2]
          {
            this->_data_list.push_back( std::make_unique<data_type>(beg, beg + BufferSize));
            std::advance(beg, BufferSize);
          }
          else
          {
            this->_data_list.push_back( std::make_unique<data_type>(beg, end));
            beg = end;
          }
        }

        do_write(t);
      }
      
      if ( /*!ec*/ bytes_transferred > 0 )
      {
        t.get_aspect().template get<_on_write_>()(t);
      }
    }
  }

  template<typename T>
  void do_write(T& t)
  {
    if ( _data_list.empty() )
      return;

    typename T::owner_type::weak_type wk = t.owner().alive();
    
    t.get_aspect().template get<_async_send_>()(t,
      *(_data_list.front()),
      //t.strand().wrap(
      [this, &t, wk](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        auto alive = wk.lock();
        if (!alive)
        {
          t.get_aspect().template get<_alive_error_>()(t);
          return;
        }
        
        this->_outgoing_size-=bytes_transferred;
        
        if (ec)
        {
          t.get_aspect().template get<_write_error_>()(t, ec);
        }
        else if ( bytes_transferred == this->_data_list.front()->size() )
        {
          this->_data_list.pop_front();
        }
        else
        {
          this->_data_list.front()->erase(
            this->_data_list.front()->begin(),
            this->_data_list.front()->begin()+bytes_transferred
          );
        }
        
        if (!ec)
          t.get_aspect().template get<_on_write_>()(t);

        this->do_write(t);
      }
      //) // t.strand().wrap(
    );
  }
    
private:
  std::atomic<int> _outgoing_size;
  // Блокировка не требуется - всегда вызываеться из потока io_service
  data_list_type _data_list;
};

}}}}
