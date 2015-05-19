#pragma once

#include <wfc/module/instance_options.hpp>

#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace wfc{

/*
template<typename DomainOptions>
struct instance_options: DomainOptions
{
  typedef DomainOptions domain_options;
  bool enabled = true;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
};
*/

template<typename DomainOptions, typename DomainJson>
struct basic_instance_options_json
{
  JSON_NAME(enabled)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)

  typedef DomainOptions domain_options;
  typedef DomainJson domain_json;
  typedef instance_options<DomainOptions> options_type;

  typedef ::iow::json::object<
    options_type,
    typename fas::type_list_n<
      ::iow::json::member<n_enabled, options_type, bool, &options_type::enabled>,
      ::iow::json::member<n_startup_priority, options_type, int, &options_type::startup_priority>,
      ::iow::json::member<n_shutdown_priority, options_type, int, &options_type::shutdown_priority>,
      ::iow::json::base<DomainJson>
    >::type
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename DomainOptions, typename DomainJson>
struct instance_options_json
{
  JSON_NAME(name)
  
  typedef DomainOptions domain_options;
  typedef DomainJson domain_json;
  typedef instance_options<DomainOptions> options_type;

  typedef ::iow::json::object<
    options_type,
    typename fas::type_list_n<
      ::iow::json::member<n_name, options_type, std::string, &options_type::name>,
      typename basic_instance_options_json<DomainOptions, DomainJson>::member_list
      
      /*::iow::json::member<n_enabled, options_type, bool, &options_type::enabled>,
      ::iow::json::member<n_startup_priority, options_type, int, &options_type::startup_priority>,
      ::iow::json::member<n_shutdown_priority, options_type, int, &options_type::shutdown_priority>,
      
      ::iow::json::base<DomainJson>
      */
    >::type
  > type;
  
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
  
};



}
