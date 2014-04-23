#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

template<typename Tg>
struct async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
        { 
          (*dd)->resize(bytes_transferred);
          TRACE_LOG_MESSAGE( "READ [[" << std::string((*dd)->begin(), (*dd)->end() ) << "]]" )

          t.get_aspect().template get<Tg>()(t, std::move(*dd), ec /*, bytes_transferred*/);
        }
      )
    );
  }
};

template<typename Tg>
struct async_read_some2
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
        { 
          
          TRACE_LOG_MESSAGE( "READ [[" << std::string((*dd)->begin(), (*dd)->end() ) << "]]" )
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
    
  }
};

template<typename Tg>
struct read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
   
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
    
  }
};


}}
