#pragma once

#include <wfc/io/ip/tcp/rn/connection_options.hpp>
//#include <wfc/io/strategy/posix/connection/rn/stream.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

#include <wfc/io/posix/rn/reader_options.hpp>
#include <wfc/io/reader/aspect/aspect.hpp>
#include <wfc/io/rn/reader/aspect/aspect.hpp>

#include <wfc/io/posix/rn/writer_options.hpp>
#include <wfc/io/writer/writer.hpp>
#include <wfc/io/rn/writer/aspect.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

struct _on_write_;
struct _on_rn_write_;
struct _shutdown_flag_;

struct _on_read_error_;
struct _on_write_error_;

struct ad_on_read_error
{
  template<typename T>
  void operator()(T& t, boost::system::error_code& ec )
  {
    if (ec != boost::asio::error::operation_aborted )
    {
      auto outgoing_buffer_size = t.get_aspect().template get< wfc::io::writer::_outgoing_buffer_size_>();
      if ( !t.get_aspect().template get<_shutdown_flag_>())
      {
        if ( outgoing_buffer_size!=0 )
        {
          COMMON_LOG_WARNING("connection closed with outgoing_buffer_size" << outgoing_buffer_size)
        }
        t.self_stop(t, nullptr);
      }
      else
      {
        DAEMON_LOG_ERROR("io shutdown not impl. outgoing_buffer_size" << outgoing_buffer_size)
        t.self_stop(t, nullptr); // TODO: shutdown
      }
    }
    else
    {
    }
  }
};



struct ad_on_write
{
  template<typename T, typename Itr>
  void operator()(T& t, Itr , Itr )
  {
    if (t.get_aspect().template get<_shutdown_flag_>())
    {
      if ( t.get_aspect().template get< wfc::io::writer::_outgoing_buffer_size_>() == 0 )
      {
        t.descriptor().close();
      }
    }
  }
};

struct ad_on_rn_write
{
  template<typename T, typename Itr>
  void operator()(T& t, Itr , Itr )
  {
    if (!t.options().keep_alive)
    {
      t.get_aspect().template get<_shutdown_flag_>() = true;
    }
  }
};


  
struct connection_aspect: 
  fas::aspect<
    fas::value<_shutdown_flag_, std::atomic<bool> >, 
    fas::advice<_on_write_, ad_on_write>, 
    fas::advice<_on_rn_write_, ad_on_rn_write>, 
    fas::advice<_on_read_error_, ad_on_read_error>,
    fas::group<wfc::io::reader::_on_abort_, _on_read_error_>,
    fas::group<wfc::io::reader::_on_error_, _on_read_error_>, 
    fas::group<wfc::io::writer::_on_write_, _on_write_>, 
    fas::group<wfc::io::rn::writer::_on_write_, _on_rn_write_>, 
    //fas::stub< wfc::io::_stop_>, // tmp

    fas::advice< wfc::io::_options_type_, connection_options>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::ip::tcp::socket>,
    
    wfc::io::rn::writer::aspect2<wfc::io::writer::_incoming_>,
    wfc::io::writer::stream< boost::asio::posix::stream_descriptor>, 

    //wfc::io::reader::aspect< wfc::io::reader::async_read_some, wfc::io::rn::reader::_incoming_, wfc::io::rn::writer::_incoming_ >,
    fas::alias< ::wfc::io::reader::_output_, ::wfc::io::rn::writer::_incoming_>,

    fas::alias< ::wfc::io::reader::_incoming_, ::wfc::io::rn::reader::_input_>,
    fas::alias< ::wfc::io::rn::reader::_output_, ::wfc::io::reader::_outgoing_>,
    ::wfc::io::reader::aspect,
    ::wfc::io::rn::reader::aspect,
    ////wfc::io::rn::reader::aspect2<wfc::io::reader::_incoming_>,
    wfc::io::reader::error_log,
    wfc::io::writer::error_log
  >
{
};
  
}}}}}
