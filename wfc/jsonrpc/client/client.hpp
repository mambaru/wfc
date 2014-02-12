#pragma once

#include <wfc/inet/client/client.hpp>
#include <wfc/inet/client/client_tcp_config.hpp>
#include <wfc/inet/client/client_tcp_configurator.hpp>

namespace wfc{ namespace jsonrpc{
  
struct client_tcp_config
  : inet::client_tcp_config
{
  bool close_after_response;
  client_tcp_config()
    : close_after_response(false)
  {}
};


struct client_tcp_configurator: 
  inet::client_tcp_configurator
{
  typedef client_tcp_config config_type;

  template<typename T>
  void operator()(T& t, const client_tcp_config& conf)
  {
    inet::client_tcp_configurator::operator()(t, conf);
    auto cntx = t.client_context();
    cntx.close_after_response =  conf.close_after_response;
    t.client_context(cntx);
  }
};


namespace connection{


struct aspect: fas::aspect< fas::type_list_n<
  aspect_error, 
  fas::advice<_send_json_, ad_send_json>,
  fas::advice<_invoke_, ad_invoke>,
  fas::alias<_input_, _invoke_>,
  fas::group<inet::_startup_, _invoke_>, 
  fas::stub<_method_stat_>
>::type >
{};

template<typename BaseContext = inet::empty_context>
struct context_stream: inet::stream::context<BaseContext>
{
  bool close_after_response;
  context_stream()
    : inet::stream::context<BaseContext>()
    , close_after_response(false)
  {};
};


struct aspect_stream_rn
  : fas::aspect<
      fas::advice< inet::_configurator_, client_tcp_configurator>, 
      inet::basic_context_t< context_stream >, 
      inet::stream::tcp::aspect,
      inet::rn::aspect,
      aspect,
      fas::alias< inet::_incoming_, inet::rn::_input_        >,
      fas::alias< inet::rn::_output_,       inet::stream::_outgoing_ >,
      fas::alias< inet::rn::_incoming_,     /*jsonrpc::*/_input_   >,
      fas::advice< _write_ , ad_stream_write>,
      fas::alias< _output_, inet::rn::_outgoing_ >
  >
{};


// namespace rn{
  
template<typename ...Args>
struct tcp  
  : inet::connection_aspect<
      typename fas::merge_aspect< 
        fas::aspect<Args...>, 
        aspect_stream_rn
      >::type 
    >
{
};

}

namespace client {

/*
struct aspect_client_tcp: fas::merge_aspect<
  fas::aspect<
    //fas::advice<inet::_configuration_, wfc::inet::client_tcp_configurator>
  >,
  
>::type
{};*/


template<typename ...Args>
struct tcp: inet::client_aspect< 
  fas::aspect<Args..., inet::aspect_client_tcp>
>{};


template<typename ...Args>
struct client_tcp_helper
{
  // TODO: Убрать!!! указать явно!!
  typedef inet::client< Args... > client_base;
};
  
template<typename A = connection::tcp<>, typename B = tcp<>, typename ...Args>
class client
  : public inet::client<A, B, Args...>
{
public:
  typedef inet::client<A, B, Args...> super;
  
  
  client( ::wfc::io_service& io_service, const client_tcp_config& conf )
    : super(io_service, conf)
  {
  }

  client( std::weak_ptr< wfc::global > g, const client_tcp_config& conf )
    : super(g, conf)
  {
  }
};

}
  
}}
