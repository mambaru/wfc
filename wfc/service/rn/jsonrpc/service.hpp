#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/service/rn/jsonrpc/service_config.hpp>
#include <wfc/io_service.hpp>
#include <wfc/core/global.hpp>
#include <list>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>


namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class server;
}}}}}}


namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{


struct ifactory
{
  typedef  ::wfc::jsonrpc::service jsonrpc_type;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;

  virtual ~ifactory() {}
  virtual jsonrpc_ptr create_for_tcp( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt) = 0;
  virtual jsonrpc_ptr create_for_udp( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt) = 0;
};

template<typename MethodList>
class factory
  : public ifactory
{
public:
  typedef MethodList methods_type;
  typedef typename methods_type::target_type target_type;
  typedef typename methods_type::provider_type provider_type;

  factory(target_type target, provider_type provider = provider_type())
    : _target(target)
    , _provider(provider)
    {}
    
  virtual jsonrpc_ptr create_for_tcp( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt)
  {
    if ( _jsonrpc == nullptr )
      _jsonrpc = std::make_shared< jsonrpc_type >( io_service, opt,  ::wfc::jsonrpc::handler<methods_type>(_target, _provider) );
    return _jsonrpc;
  }

  virtual jsonrpc_ptr create_for_udp( ::wfc::io_service& io_service, const  ::wfc::jsonrpc::options& opt)
  {
    if ( _jsonrpc == nullptr )
      _jsonrpc = std::make_shared< jsonrpc_type >( io_service, opt,  ::wfc::jsonrpc::handler<methods_type>(_target, _provider) );
    return _jsonrpc;
  }

private:
  target_type _target;
  provider_type _provider;
  jsonrpc_ptr _jsonrpc;
};


template<typename ML>
inline std::shared_ptr<ifactory>
make_factory(typename ML::target_type target, typename ML::provider_type provider = typename ML::provider_type() )
{
  return std::make_shared< factory< ML> >( target, provider );
}

class service
{
  typedef  ::wfc::jsonrpc::service jsonrpc_type;
  typedef  ::wfc::io::ip::tcp::rn::jsonrpc::server server_tcp_type;
  typedef  ::wfc::io::ip::udp::rn::jsonrpc::server server_udp_type;
  typedef std::shared_ptr<server_tcp_type> server_tcp_ptr;
  typedef std::shared_ptr<server_udp_type> server_udp_ptr;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;
  
public:
  
  service(std::weak_ptr<  ::wfc::global > g, const service_config& conf);
  
  service( ::wfc::io_service& io, const service_config& conf, std::shared_ptr<ifactory> fact);
  
  void reconfigure(const service_config& conf);
  
  void initialize(std::shared_ptr<ifactory> fact);
  
  void start();
  
  void stop();
  
  static service_config create_config(std::string type);
  // static jsonrpc_options create_jsonrpc_options();

private:
  
  void create( ::wfc::io_service& io, const service_config& conf, std::shared_ptr<ifactory> fact);
  
private:
  ::wfc::io_service& _io_service;
  service_config _conf;
  
  jsonrpc_ptr _jsonrpc_for_tcp;
  std::list< server_tcp_ptr > _tcp_servers;

  jsonrpc_ptr _jsonrpc_for_udp;
  std::list< server_udp_ptr > _udp_servers;

};

}}}}
