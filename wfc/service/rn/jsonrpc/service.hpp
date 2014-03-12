#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/core/global.hpp>
#include <list>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class server;
}}}}}}


namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

typedef wfc::io::ip::tcp::rn::server_options tcp_options;

struct configuration
{
  wfc::jsonrpc::options jsonrpc;
  std::vector<tcp_options> tcp;
};

struct ifactory
{
  typedef wfc::jsonrpc::service jsonrpc_type;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;

  virtual ~ifactory() {}
  virtual jsonrpc_ptr create_for_tcp() = 0;
};

template<typename MethodList>
class factory
  : public ifactory
{
public:
  typedef MethodList methods_type;
  typedef typename methods_type::target_type target_type;

  factory(target_type target)
    : _target(target)
    {}
    
  virtual jsonrpc_ptr create_for_tcp()
  {
    return std::make_shared< jsonrpc_type >( wfc::jsonrpc::handler<methods_type>(_target) );
  }

private:
  target_type _target;
};


template<typename ML>
inline std::shared_ptr<ifactory>
make_factory(typename ML::target_type target)
{
  return std::make_shared< factory< ML> >( target );
}

class service
{
  typedef wfc::jsonrpc::service jsonrpc_type;
  typedef wfc::io::ip::tcp::rn::jsonrpc::server server_tcp_type;
  typedef std::shared_ptr<server_tcp_type> server_tcp_ptr;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;
public:
  service(std::weak_ptr< wfc::global > g, const configuration& conf, std::shared_ptr<ifactory> fact);
  
  void reconfigure(const configuration& conf);
  
  void initialize( );
  
  void start();
  
  void stop();

private:
  jsonrpc_ptr _jsonrpc_for_tcp;
  std::list< server_tcp_ptr > _tcp_servers;
};

}}}}
