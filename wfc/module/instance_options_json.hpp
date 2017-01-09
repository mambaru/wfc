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

struct nostat_json
{
  typedef ::wjson::object<
    nostat,
    ::wjson::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;

};

template<typename DomainStatOpt = nostat_json >
struct base_statistics_options_json;

template<>
struct base_statistics_options_json<nostat_json>
{
  static constexpr bool enabled = false;
  typedef typename nostat_json::target domain_stat;

  typedef ::wjson::object<
    base_statistics_options<domain_stat>,
    ::wjson::member_list<
      ::wjson::base<nostat_json>
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename DomainStatOptJson>
struct base_statistics_options_json
{
  static constexpr bool enabled = true;
  typedef typename DomainStatOptJson::target domain_stat;
  JSON_NAME(disabled)
  JSON_NAME(target)

  typedef ::wjson::object<
    base_statistics_options<domain_stat>,
    ::wjson::member_list<
      ::wjson::member<n_disabled, domain_stat, bool, &domain_stat::disabled>,
      ::wjson::member<n_target, domain_stat, std::string, &domain_stat::target>,
      ::wjson::base<DomainStatOptJson>
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;

};


template<int Features, typename DomainStatOptJson = nostat_json >
struct base_instance_options_json
{
  typedef base_statistics_options_json<DomainStatOptJson> stat_json;
  typedef typename stat_json::target domain_stat;
  typedef base_instance_options<domain_stat> options_type;
  
  JSON_NAME(name)
  JSON_NAME(enabled)
  JSON_NAME(suspend)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)
  JSON_NAME(workflow)
  JSON_NAME(statistics)

  enum 
  {
    has_name       =  ( Features & component_features::Singleton        )   == 0 ,
    has_enabled    =  ( Features & component_features::DisabledEnabled  )   == 0 ,
    has_suspend    =  ( Features & component_features::DisabledSuspend  )   == 0 ,
    has_priority   =  ( Features & component_features::DisabledPriority )   == 0 ,
    has_workflow   =  ( Features & component_features::DisabledWorkflow )  == 0,
    has_statistics =  stat_json::enabled
    /*,
    has_statistics =  ( Features & component_features::DisabledStatistics ) == 0*/
  };
  
  typedef ::wjson::object<
    options_type,
    ::wjson::member_list<
      typename optional_member<
        has_name,
        n_name,
        options_type, 
        std::string, 
        &options_type::name
      >::type,
      typename optional_member<
        has_enabled,
        n_enabled,
        options_type,
        bool,
        &options_type::enabled
      >::type,
      typename optional_member<
        has_suspend,
        n_suspend,
        options_type,
        bool,
        &options_type::suspend
      >::type,
      typename optional_member<
        has_priority,
        n_startup_priority,
        options_type,
        int,
        &options_type::startup_priority
      >::type,
      typename optional_member<
        has_priority,
        n_shutdown_priority,
        options_type,
        int,
        &options_type::shutdown_priority
      >::type,
      typename optional_member<
        has_workflow,
        n_workflow,
        options_type,
        std::string,
        &options_type::workflow
      >::type
#warning TODO statistics
      /*,
      typename optional_member<
        has_statistics,
        n_statistics,
        options_type,
        domain_stat,
        &options_type::statistics,
        stat_json
      >::type*/
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
