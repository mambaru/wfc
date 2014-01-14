#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>

#include <wfc/inet/conn/tags.hpp>
#include <wfc/memory.hpp>


namespace wfc{ namespace inet{


template<size_t BufferSize = 8096*2 >
struct ad_stream_reader
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;

  static constexpr size_t BUFFER_SIZE = BufferSize;

  template<typename T>
  void operator()(T& t)
  {
    this->do_read(t);
  }

  template<typename T>
  void do_read(T& t)
  {
    typename T::owner_type::weak_type wk = t.owner().alive();
    t.socket().async_read_some(
      ::boost::asio::buffer(_data),
      [this, &t, wk](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        auto alive = wk.lock();
        if (!alive) 
        {
          t.get_aspect().template get<_alive_error_>()(t);
        }
        else if (!ec)
        {
          std::cout << "[[" << std::string(_data.begin(), _data.begin() + bytes_transferred) << "]" << std::endl;
          t.get_aspect().template get<_on_read_>()
            (t, std::make_unique<data_type>(_data.begin(), _data.begin() + bytes_transferred) );
            
          this->do_read(t);
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
          t.get_aspect().template get<_read_error_>()(t, ec);
          t.close(); /*перенести в _read_error_*/
        }
      }
    );
  }
  
private:
  std::array<char, 8192*2> _data;
};

}}
