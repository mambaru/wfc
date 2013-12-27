#pragma once 
#include <string>
#include <vector>
#include <iostream>

namespace wfc{ namespace inet{

struct server_config
{
  bool enabled;
  bool enable_stat;
  std::string host;
  std::string port;

  server_config()
    : enabled(true)
    , enable_stat(false)
  {
  }
  
/*  
  template<typename T>
  void operator()(T& t)
  {
    
    
  }

  template<typename T>
  void configure(T& t, typename T::server_context_type& cntx)
  {
    cntx.enable_stat = this->enable_stat;
    cntx.host = this->host;
    cntx.port = this->port;
  }
*/

  /*
  server_context_type cntx = this->server_context();
  cntx.enable_stat = conf.enable_stat;
  cntx.host = conf.host;
  cntx.port = conf.port;
  this->server_context(cntx);
  */
};

struct server_configurator
{
  typedef server_config config_type;

  template<typename T>
  void operator()(T& t, const server_config& conf)
  {
    auto cntx = t.server_context();
    //this->configure(t, cntx, conf);
    cntx.enable_stat = conf.enable_stat;
    cntx.host = conf.host;
    cntx.port = conf.port;

    t.server_context(cntx);
  }

  /*
  template<typename T>
  void configure(T& t, typename T::server_context_type& cntx, const server_config& conf)
  {
    cntx.enable_stat = conf.enable_stat;
    cntx.host = conf.host;
    cntx.port = conf.port;
  }
  */
};

}}
