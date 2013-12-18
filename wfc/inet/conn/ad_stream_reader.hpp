#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>


namespace wfc{ namespace inet{

template<typename Tg>
struct ad_stream_reader
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;

  static constexpr size_t BUFFER_SIZE=8096*2;

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
        if (!alive) {
          std::cout << "READER NOT ALIVE!!!" << std::endl;
          return;
        }
        if (!ec)
        {
          // std::cout << "REEADED " << bytes_transferred << ": [[" << std::string(_data.begin(), _data.begin()+bytes_transferred) << "]]" << std::endl;
          t.get_aspect().template get<Tg>()(t, data_ptr(new data_type(_data.begin(), _data.begin()+bytes_transferred)));
          this->do_read(t);
        }
        else
        {
          // std::cout << "closed??" << std::endl;
          t.release();
        }
      }
    );
  }
  
private:
  std::array<char, 8192*2> _data;
};

}}
