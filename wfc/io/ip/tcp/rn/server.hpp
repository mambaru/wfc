#pragma once

#include "server/config.hpp"
#include "server/config_json.hpp"
#include <wfc/io_service.hpp>
#include <wfc/core/global.hpp>

#include <memory>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{
  
namespace impl{ class server; }
  
class server
{
public:
  typedef impl::config config;
  typedef impl::config_json config_json;
  
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;

  typedef std::function<void(data_ptr)> handler_callback;
  typedef std::function<void(data_ptr, handler_callback)> handler;

  server( wfc::io_service& io);
  server( std::weak_ptr< wfc::global > g, const config& conf );
  
  void reconfigure(const config& conf);
  void configure(const config& conf);
  void initialize( handler h );
  void start();
  void stop();
  
private:
  std::shared_ptr<impl::server> _server;
};
  
}}}}}