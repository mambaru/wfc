#pragma once 

#include <wfc/jsonrpc/aspect.hpp>

#include <wfc/inet/stream/tcp/aspect.hpp>
#include <wfc/inet/rn/aspect.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/inet/conn/connection_context.hpp>

namespace wfc{ namespace jsonrpc{
  
template<typename BaseContext = inet::empty_context>
struct context_stream: inet::stream::context<BaseContext>
{
  bool close_after_response;
  context_stream()
    : inet::stream::context<BaseContext>()
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
      inet::stream::tcp::aspect,
      inet::rn::aspect,
      aspect,
      fas::alias< inet::_incoming_, inet::rn::_input_        >,
      fas::alias< inet::rn::_output_,       inet::stream::_outgoing_ >,
      fas::alias< inet::rn::_incoming_,     jsonrpc::_input_   >,
      fas::advice< _write_ , ad_stream_write>,
      fas::alias< _output_, inet::rn::_outgoing_ >
  >
{};

}}
