#pragma once

#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace io{

template<typename Tg>
struct async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "async_read_some" << std::endl;
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
        { 
          std::cout << "async_read_some ready " << bytes_transferred << std::endl;
          (*dd)->resize(bytes_transferred);
          t.get_aspect().template get<Tg>()(t, std::move(*dd), ec /*, bytes_transferred*/);
        }
      )
    );
    
    std::cout << "async_read_some done" << std::endl;
  }
};

template<typename Tg>
struct async_read_some2
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "async_read_some" << std::endl;
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
        { 
          std::cout << "async_read_some ready " << bytes_transferred << std::endl;
          if ( ec )
          {
            t.get_aspect().template get<_status_>() = false;
            t.get_aspect().template get<_error_code_>() = ec;
          }
          (*dd)->resize(bytes_transferred);
          t.get_aspect().template get<Tg>()(t, std::move(*dd));
        }
      )
    );
    
    std::cout << "async_read_some done" << std::endl;
  }
};

template<typename Tg>
struct read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "sync read_some {" << std::endl;
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.descriptor().read_some( ::boost::asio::buffer( *d ), ec);

    if ( ec )
    {
      t.get_aspect().template get<_status_>() = false;
      t.get_aspect().template get<_error_code_>() = ec;
    }
      
    d->resize(bytes_transferred);
    t.get_aspect().template get< Tg >()(t, std::move(d));
    std::cout << "} sync read_some " << bytes_transferred << ": " << ec.message() << ": " << t.descriptor().native_handle()<< std::endl;
  }
};


}}
