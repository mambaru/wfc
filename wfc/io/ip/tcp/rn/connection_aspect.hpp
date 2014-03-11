#pragma once

#include <wfc/io/ip/tcp/rn/connection_options.hpp>
//#include <wfc/io/strategy/posix/connection/rn/stream.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

#include <wfc/io/posix/rn/reader_options.hpp>
#include <wfc/io/reader/aspect.hpp>
#include <wfc/io/rn/reader/aspect.hpp>

#include <wfc/io/posix/rn/writer_options.hpp>
#include <wfc/io/writer/writer.hpp>
#include <wfc/io/rn/writer/aspect.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{
/*
// Убрать нафик (поставить handler)
struct ad_handler
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    auto handler = t.options().handler;
    if ( handler == nullptr )
    {
      t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
    }
    else
    {
      handler( std::move(d), [&t](typename T::data_ptr d)
      {
        t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
        t.descriptor().close();
      });
    }
  }
};*/

struct _on_write_;
struct _on_rn_write_;
struct _shutdown_flag_;

struct ad_on_write
{
  template<typename T, typename Itr>
  void operator()(T& t, Itr beg, Itr end)
  {
    std::cout <<  "ad_on_write [" <<  std::string(beg, end) << "]" << std::endl;
    if (t.get_aspect().template get<_shutdown_flag_>())
    {
      std::cout << "_outgoing_buffer_size_ ==" << t.get_aspect().template get< wfc::io::writer::_outgoing_buffer_size_>() << std::endl;
      //auto& lst = t.get_aspect().template get< wfc::io::writer::_outgoing_buffer_list_>();
      if ( t.get_aspect().template get< wfc::io::writer::_outgoing_buffer_size_>() == 0 )
      {
        std::cout << "list empty" << std::endl;
        t.descriptor().close();
      }
    }
  }
};

struct ad_on_rn_write
{
  template<typename T, typename Itr>
  void operator()(T& t, Itr beg, Itr end)
  {
    std::cout <<  "ad_on_rn_write [" <<  std::string(beg, end) << "]" << std::endl;
    if (!t.options().keep_alive)
    {
      //t.descriptor().shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
      t.get_aspect().template get<_shutdown_flag_>() = true;
    }
  }
};


  
struct connection_aspect: 
  fas::aspect<
    fas::value<_shutdown_flag_, bool>, 
    fas::advice<_on_write_, ad_on_write>, 
    fas::advice<_on_rn_write_, ad_on_rn_write>, 
    fas::group<wfc::io::writer::_on_write_, _on_write_>, 
    fas::group<wfc::io::rn::writer::_on_write_, _on_rn_write_>, 
    
    fas::stub< wfc::io::_stop_>, // tmp
    fas::advice< wfc::io::_options_type_, connection_options>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::ip::tcp::socket>,
    
    wfc::io::rn::writer::aspect2<wfc::io::writer::_incoming_>,
    wfc::io::writer::stream< boost::asio::posix::stream_descriptor/*, wfc::io::rn::writer::_incoming_*/>, 

    wfc::io::reader::aspect< wfc::io::async_read_some2, wfc::io::rn::reader::_incoming_, wfc::io::rn::writer::_incoming_ >,
    wfc::io::rn::reader::aspect2<wfc::io::reader::_incoming_>,
    wfc::io::reader::error_log


    /*
    fas::stub< wfc::io::_stop_>,
    fas::advice< wfc::io::_options_type_, connection_options>,
    fas::advice< wfc::io::_incoming_, ad_handler>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::ip::tcp::socket>,
    wfc::io::strategy::posix::connection::rn::stream
    */
  >
{
};
  
}}}}}