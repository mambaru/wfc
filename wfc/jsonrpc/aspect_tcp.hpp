#pragma once 

#include <wfc/jsonrpc/aspect.hpp>

#include <wfc/inet/conn/stream/tcp/aspect.hpp>
#include <wfc/inet/conn/rn/aspect.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/inet/conn/connection_context.hpp>

namespace wfc{ namespace jsonrpc{
  
template<typename BaseContext = inet::conn::empty_context>
struct context_stream: inet::conn::stream::context<BaseContext>
{
  bool close_after_response;
  context_stream()
    : inet::conn::stream::context<BaseContext>()
    , close_after_response(false)
  {};
};


struct ad_stream_write
{
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    t.get_aspect().template get<jsonrpc::_output_>()(t, std::move(d) );
    if ( t.context().close_after_response )
      t.shutdown();
  }
};

template<typename ...Args>
struct aspect_stream
  : fas::aspect<
      typename fas::type_list_n<Args...>::type,
      inet::basic_context_t< context_stream >, 
      inet::conn::stream::tcp::aspect,
      inet::conn::rn::aspect,
      aspect,
      fas::alias< inet::conn::_incoming_, inet::conn::rn::_input_        >,
      fas::alias< inet::conn::rn::_output_,       inet::conn::stream::_outgoing_ >,
      fas::alias< inet::conn::rn::_incoming_,     jsonrpc::_input_   >,
      fas::advice< _write_ , ad_stream_write>,
      fas::alias< _output_, inet::conn::rn::_outgoing_ >
  >
{};

}}
