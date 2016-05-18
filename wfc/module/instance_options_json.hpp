#pragma once

#include <wfc/module/instance_options.hpp>
#include <wfc/module/component_features.hpp>

#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace wfc{
  
template<bool, typename N, typename V, typename M, M V::* m, typename W = ::iow::json::value<M> >
struct optional_member;

template<typename N, typename V, typename M, M V::* m, typename W >
struct optional_member<true, N, V, M, m, W>
{
  typedef ::iow::json::member<N, V, M, m, W> type;
};

template<typename N, typename V, typename M, M V::* m, typename W >
struct optional_member<false, N, V, M, m, W>
{
  typedef ::fas::empty_list type;
};

template<typename DomainJson, int Features = 0 >
struct instance_options_json
{
  JSON_NAME(name)
  JSON_NAME(enabled)
  JSON_NAME(suspend)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)
  
  enum 
  {
    has_name =     ( Features & int(component_features::Singleton)      ) == 0 ,
    has_enabled =  ( Features & int(component_features::Fixed)          ) == 0,
    has_priority = ( Features & int(component_features::Extraordinary)  ) == 0 ,
    has_suspend =  ( Features & int(component_features::SuspendSupport) ) != 0 
  };
  
  typedef DomainJson domain_json;
  typedef typename domain_json::target domain_options;
  typedef instance_options<domain_options> options_type;

  typedef ::iow::json::object<
    options_type,
    typename fas::type_list_n<
      typename optional_member<has_name,     n_name,              options_type, std::string, &options_type::name>::type,
      typename optional_member<has_enabled,  n_enabled,           options_type, bool, &options_type::enabled>::type,
      typename optional_member<has_suspend,  n_suspend,           options_type, bool, &options_type::suspend>::type,
      typename optional_member<has_priority, n_startup_priority,  options_type, int,  &options_type::startup_priority>::type,
      typename optional_member<has_priority, n_shutdown_priority, options_type, int,  &options_type::shutdown_priority>::type,
      ::iow::json::base<DomainJson>
    >::type
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;

};

}
