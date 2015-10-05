#pragma once

#include <wfc/module/instance_options.hpp>

#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace wfc{

template<typename DomainJson>
struct singleton_instance_options_json
{
  JSON_NAME(enabled)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)

  typedef DomainJson domain_json;
  typedef typename domain_json::target domain_options;
  typedef instance_options<domain_options> options_type;

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

template<typename DomainJson>
struct multiton_instance_options_json
{
  JSON_NAME(name)

  typedef DomainJson domain_json;
  typedef typename domain_json::target domain_options;
  typedef instance_options<domain_options> options_type;

  typedef ::iow::json::object<
    options_type,
    typename fas::type_list_n<
      ::iow::json::member<n_name, options_type, std::string, &options_type::name>,
      typename singleton_instance_options_json<DomainJson>::member_list
    >::type
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename DomainJson, bool Singleton>
struct instance_options_json;

template<typename DomainJson>
struct instance_options_json<DomainJson, true>
  : singleton_instance_options_json<DomainJson>
{};


template<typename DomainJson>
struct instance_options_json<DomainJson, false>
  : multiton_instance_options_json<DomainJson>
{};

}
