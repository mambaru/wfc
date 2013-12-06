#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <comet/callback/callback_owner.hpp>
#include <comet/asio/basic/tags.hpp>
#include <fas/xtime.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace basic{

struct ad_writer
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::list<data_ptr> data_list_type;
  typedef ::boost::asio::const_buffer buffer_type;

  

  static constexpr size_t BUFFER_SIZE=8096*2;

  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    
    if ( d==nullptr || d->empty() )
      return;

    
    
    if ( !_data_list.empty() )
    {
      _data_list.push_back(std::move(d));
    }
    else
    {
      const char *ptr = &((*d)[0]);

      auto buf = ::boost::asio::buffer(d->data(), d->size());
      //size_t s = t.socket().send( buf );
      int sock = t.socket().native_handle();
      fas::nanospan ns = fas::nanotime();
      size_t s = t.socket().send( ::boost::asio::buffer(d->data(), d->size()) );
      //size_t s = ::send( sock, d->data(), d->size(), 0 );
      fas::nanospan nf = fas::nanotime();
      std::cout << "method timeout " << nf-ns << std::endl;
      std::cout << "method rate " << fas::rate(nf-ns) << std::endl;
      
      if ( s != d->size() )
      {
        d->erase(d->begin(), d->begin()+s);
        _data_list.push_back(std::move(d));
        do_write(t);
      }
      else
        ;
    }


  }

  template<typename T>
  void do_write(T& t)
  {
    if ( _data_list.empty() )
      return;

    callback_owner::weak_type wk = t.owner().alive();
    t.socket().async_write_some(
      ::boost::asio::buffer( _data_list.front()->data(),  _data_list.front()->size() ),
      t.strand().wrap(
      [this, &t, wk](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        auto alive = wk.lock();
        if (!alive)
        {
          std::cout << "NOT ALIVE!!!" << std::endl;
          return;
        }
        
        
        if ( bytes_transferred == this->_data_list.front()->size() )
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
        this->do_write(t);
      }
      ) // t.strand().wrap(
    );
  }
  
private:

  data_list_type _data_list;
};


}}}}
