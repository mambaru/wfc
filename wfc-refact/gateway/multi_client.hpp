#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/gateway/rn/jsonrpc/gateway_config.hpp>
#include <wfc/asio.hpp>
#include <wfc/core/global.hpp>
#include <wfc/pubsub/pubsub_gateway.hpp>
#include <vector>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

#include <wfc/gateway/tcp_options.hpp>

/*
namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class client;
}}}}}}
*/


namespace wfc{ namespace gateway{
  


template<typename Client>
class multi_client
{
public:

  typedef Client client_type;
  typedef typename client_type::rpc_type rpc_type;
  typedef typename client_type::options_type client_options_type;
  typedef multi_client_options<client_options_type> options_type;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef typename client_type::rpc_ptr rpc_ptr;
  typedef std::vector< client_ptr > client_list;
  typedef ::wfc::asio::io_service io_service_type;
  
  multi_client( std::shared_ptr< ::wfc::global > g, const options_type& conf)  
    : _global(g)
    , _io_service( g->io_service )
    , _conf( conf )
  {
  }

  multi_client( io_service_type& io, const options_type& conf, rpc_ptr rpc)  
    : _io_service(io)
    , _conf( conf )
    , _rpc(rpc)
  {
    this->recreate_();
  }

  void reconfigure(const options_type& conf)
  {
    _conf = conf;
  }
  
  void initialize(rpc_ptr rpc)
  {
    _rpc = rpc;
    this->recreate_();
  }
  
  void start()
  {
    for (auto& i: _client_list)
    {
      i->start();
    }
  }

  
  void stop(std::function<void()> finalize)
  {
    for (auto& i: _client_list)
    {
      i->stop(finalize);
    }
  }

  void shutdown()
  {
    for (auto& i: _client_list)
    {
      i->shutdown();
    }
  }

private:
  
  void recreate_()
  {
    if ( _conf.enabled && _conf.count > 0 )
    {
      if ( _client_list.size() > _conf.count )
      {
        for (size_t i = _conf.count; i < _client_list.size(); i++ )
        {
          _client_list[i]->stop(nullptr);
        }
        _client_list.resize(_conf.count);
      }
      else
      {
        while ( _client_list.size() < _conf.count )
        {
          _client_list.push_back(
            std::make_shared<client_type>( _io_service, _conf, _rpc )
          );
        }
      }
    }
    else
    {
      for (auto& cli: _client_list)
      {
        cli->stop(nullptr);
      }
      _client_list.clear();
    }
  }
  
private:
  std::shared_ptr< ::wfc::global> _global;

  io_service_type& _io_service;
  options_type _conf;
  rpc_ptr _rpc;
  client_list _client_list;
};

}}
