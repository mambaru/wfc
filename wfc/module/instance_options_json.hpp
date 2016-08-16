#pragma once

//#include <wfc/core/workflow_options_json.hpp>

#include <wfc/module/instance_options.hpp>
#include <wfc/module/component_features.hpp>

#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wfc{
  
template<bool, typename N, typename V, typename M, M V::* m, typename W = ::wjson::value<M> >
struct optional_member;

template<typename N, typename V, typename M, M V::* m, typename W >
struct optional_member<true, N, V, M, m, W>
{
  typedef ::wjson::member<N, V, M, m, W> type;
};

template<typename N, typename V, typename M, M V::* m, typename W >
struct optional_member<false, N, V, M, m, W>
{
  typedef ::fas::empty_list type;
};

template<int Features = 0 >
struct base_instance_options_json
{
  JSON_NAME(name)
  JSON_NAME(enabled)
  JSON_NAME(suspend)
  JSON_NAME(workflow)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)

  enum 
  {
    has_name      =  ( Features & component_features::Singleton        )   == 0 ,
    has_enabled   =  ( Features & component_features::DisabledEnabled  )   == 0 ,
    has_priority  =  ( Features & component_features::DisabledPriority )   == 0 ,
    has_suspend   =  ( Features & component_features::DisabledSuspend  )   == 0 ,
    has_workflow  =  ( Features & component_features::DisabledWorkflow )   == 0
  };
  
  typedef ::wjson::object<
    base_instance_options,
    ::wjson::member_list<
      typename optional_member<has_name,     n_name,              base_instance_options, std::string, &base_instance_options::name>::type,
      typename optional_member<has_enabled,  n_enabled,           base_instance_options, bool,        &base_instance_options::enabled>::type,
      typename optional_member<has_suspend,  n_suspend,           base_instance_options, bool,        &base_instance_options::suspend>::type,
      typename optional_member<has_priority, n_startup_priority,  base_instance_options, int,         &base_instance_options::startup_priority>::type,
      typename optional_member<has_priority, n_shutdown_priority, base_instance_options, int,         &base_instance_options::shutdown_priority>::type,
      typename optional_member<has_workflow, n_workflow,          base_instance_options, std::string, &base_instance_options::workflow>::type
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename DomainJson, int Features = 0 >
struct domain_instance_options_json
{
  typedef base_instance_options_json<Features> instance_json;
  typedef DomainJson domain_json;
  typedef typename domain_json::target domain_options;
  typedef domain_instance_options<domain_options> options_type;

  typedef ::wjson::object<
    options_type,
    ::wjson::member_list<
      ::wjson::base<instance_json>,
      ::wjson::base<DomainJson>
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};


}
