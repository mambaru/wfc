#pragma once

#include <string>
#include <iow/owner/owner_handler.hpp>
#include <iow/owner/callback_handler.hpp>

namespace wfc{

template<typename Opt, typename StatOpt>
struct instance_handler_
{
  typedef domain_config_t<Opt, StatOpt> domain_config;
  typedef typename domain_config::domain_options domain_options;
  
  virtual void domain_generate(domain_config& conf, const std::string& type) = 0;
  virtual void create_domain(const std::string& objname, std::shared_ptr<wfcglobal> g ) = 0;
  virtual void configure_domain(const domain_config& opt) = 0;
  virtual void initialize_domain() = 0;
  virtual void reconfigure_domain_basic(const domain_options& opt) = 0;
  virtual void reconfigure_domain(const domain_config& conf) = 0;
  virtual void restart_domain() = 0;
  virtual void start_domain() = 0;
  virtual void stop_domain() = 0;
};

template<typename H, typename H2>
struct wrap_result_
{
  using type = ::iow::owner_handler<
    typename std::remove_reference<H>::type, 
    typename std::remove_reference<H2>::type
  >;
};

template<typename H>
struct callback_result_
{
  using type = ::iow::callback_handler<typename std::remove_reference<H>::type>;
};

}

