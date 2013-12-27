#pragma once

#include <wfc/inet/conn/ad_connection_start.hpp>
#include <wfc/inet/conn/ad_stream_reader.hpp>
#include <wfc/inet/conn/ad_stream_writer.hpp>
#include <wfc/inet/conn/connection_context.hpp>
#include <wfc/inet/stream/tags.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/logger.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace stream{


template<typename Base = empty_context>
struct stream_context: connection_context<Base>
{
  size_t outgoing_warning;
  size_t outgoing_limit;
  bool shutdown;
  stream_context()
    : connection_context<Base>()
    , outgoing_warning(0)
    , outgoing_limit(0)
    , shutdown(false)
  {}
};
  
struct ad_outgoing
{
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    t.get_aspect().template get<_writer_>()(t, std::move(d));
  }
};

struct ad_shutdown
{
  template<typename T>
  void operator()(T& t)
  {
    // std::cout << "ad_shutdown" << std::endl;
    t.get_io_service().dispatch([&t]()
    {
      // std::cout << "ad_shutdown dispatch" << std::endl;
      size_t size  = t.get_aspect().template get<_writer_>().outgoing_size();
      if ( size == 0)
      {
        t.close();
      }
      else
      {
        // std::cout << "t.context().shutdown == true" << std::endl;
        t.context().shutdown = true;
      }
    });
  }
};

struct ad_on_write
{
  template<typename T>
  void operator()(T& t)
  {
    size_t limit = t.context().outgoing_limit;
    size_t warn  = t.context().outgoing_warning;
    bool shutdown = t.context().shutdown;
    size_t size  = t.get_aspect().template get<_writer_>().outgoing_size();
    if ( limit!=0 && size>=limit)
    {
      COMMON_LOG_WARNING(" Outgoing data size=" << size << "(outgoing_limit==" << limit << "). Сonnection will be closed!" )
      t.close();
    }
    else if ( warn!=0 && size>=warn )
    {
      COMMON_LOG_WARNING(" Outgoing data size=" << size << "(outgoing_warning==" << warn << ")" )
    }
    
    //std::cout << "size=" << size << " " << shutdown << std::endl;
    if ( shutdown && size==0 )
    {
      //std::cout << "shutdown!!!" << std::endl;
      t.close();
    }
  }
};
/*
 * _incoming_ required
 */
struct aspect: fas::aspect
< 
  context<empty_context>,
  basic_context_t<stream_context>, 
  fas::advice<_start_,    ad_connection_start<_reader_> >,
  //fas::advice<_reader_,   ad_stream_reader<_incoming_> >,
  fas::advice<_reader_,   ad_stream_reader<> >,
  fas::advice<_writer_,   ad_stream_writer<> >,
  fas::alias<_on_read_,   _incoming_ >,
  fas::alias<_input_,     _reader_ >,
  fas::advice<_outgoing_, ad_outgoing >,
  fas::alias<_write_,     _writer_ >,  
  fas::type< _socket_type_, boost::asio::ip::tcp::socket >,

  fas::advice<_on_write_, ad_on_write>, 
  fas::advice<_shutdown_, ad_shutdown>,
  fas::stub<_write_error_>, 
  fas::stub<_read_error_>, 
  fas::stub<_alive_error_>
>
{};

}}}
