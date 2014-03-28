#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/gateway/rn/jsonrpc/gateway_config.hpp>
#include <wfc/io_service.hpp>
#include <wfc/core/global.hpp>
#include <wfc/pubsub/pubsub_gateway.hpp>
#include <list>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>


namespace wfc{ namespace gateway{ 
}}


namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class client;
}}}}}}


namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{


struct ifactory
{
  typedef  ::wfc::jsonrpc::service jsonrpc_type;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;

  virtual ~ifactory() {}
  virtual jsonrpc_ptr create_for_tcp( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt) = 0;
  virtual jsonrpc_ptr create_for_pubsub( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt) = 0;
};

template<typename MethodList>
class factory
  : public ifactory
{
public:
  typedef MethodList methods_type;
  typedef typename methods_type::target_type target_type;
  typedef typename methods_type::provider_type provider_type;

  factory(target_type target, provider_type provider)
    : _target(target)
    , _provider(provider)
    {}
    
  virtual jsonrpc_ptr create_for_tcp( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt)
  {
    return std::make_shared< jsonrpc_type >( io_service, opt,  ::wfc::jsonrpc::handler<methods_type>(_target, _provider) );
  }

  virtual jsonrpc_ptr create_for_pubsub( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt)
  {
    return std::make_shared< jsonrpc_type >( io_service, opt,  ::wfc::jsonrpc::handler<methods_type>(_target, _provider) );
  }

private:
  target_type   _target;
  provider_type _provider;
};


template<typename ML>
inline std::shared_ptr<ifactory>
make_factory(typename ML::target_type target, typename ML::provider_type provider)
{
  return std::make_shared< factory< ML> >( target, provider);
}

class gateway
{
  typedef ::wfc::jsonrpc::service jsonrpc_type;
  typedef ::wfc::io::ip::tcp::rn::jsonrpc::client client_tcp_type;
  typedef ::wfc::pubsub::pubsub_gateway pubsub_gateway;
  typedef std::shared_ptr<pubsub_gateway> pubsub_gateway_ptr;
  typedef std::shared_ptr<client_tcp_type> client_tcp_ptr;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;
  
public:
  
  gateway( std::weak_ptr<::wfc::global> g, const gateway_config& conf);
  
  gateway( ::wfc::io_service& io, const gateway_config& conf, std::shared_ptr<ifactory> fact);
  
  void reconfigure(const gateway_config& conf);
  
  void initialize(std::shared_ptr<ifactory> fact);
  
  void start();
  
  void stop();

  static gateway_config create_config(std::string);
  
private:
  
  void create( ::wfc::io_service& io, const gateway_config& conf, std::shared_ptr<ifactory> fact);
  
private:
  std::weak_ptr<::wfc::global> _global;
  ::wfc::io_service& _io_service;
  gateway_config _conf;
  
  jsonrpc_ptr _jsonrpc_for_tcp;
  jsonrpc_ptr _jsonrpc_for_pubsub;
  std::list< client_tcp_ptr > _tcp_clients;
  std::list< pubsub_gateway_ptr > _pubsubs;
};

}}}}
