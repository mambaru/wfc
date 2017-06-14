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

template<typename T>
struct empty_stat_json_t
{
  typedef ::wjson::object<
    T,
    ::wjson::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

struct nostat_json: empty_stat_json_t<nostat>
{
  /*
  typedef ::wjson::object<
    nostat,
    ::wjson::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
  */
};

struct defstat_json: empty_stat_json_t<defstat>
{
  /*
  typedef ::wjson::object<
    defstat,
    ::wjson::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
  */
};


template<typename StatOptJson = nostat_json >
struct statistics_options_json;

template<>
struct statistics_options_json<nostat_json>
{
  typedef ::wjson::object<
    statistics_options<nostat>,
    ::wjson::member_list<
      ::wjson::base<nostat_json>
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename StatOptJson>
struct statistics_options_json
{
  typedef typename StatOptJson::target base_options;
  typedef statistics_options<base_options> statistics_type;
  JSON_NAME(disabled)
  JSON_NAME(target)

  typedef ::wjson::object<
    statistics_type,
    ::wjson::member_list<
      ::wjson::member<n_disabled, statistics_type, bool, &statistics_type::disabled>,
      ::wjson::member<n_target, statistics_type, std::string, &statistics_type::target>,
      ::wjson::base<StatOptJson>
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;

};


template<int Features, typename StatOptJson /*= nostat_json*/ >
struct basic_instance_options_json
{
  /*
  typedef base_statistics_options_json<DomainStatOptJson> stat_json;
  typedef typename stat_json::target domain_stat;
  */
  typedef StatOptJson base_stat_json;
  typedef typename base_stat_json::target base_stat_type;
  typedef basic_instance_options<base_stat_type> options_type;
  typedef statistics_options_json<base_stat_json> stat_json;
  typedef typename stat_json::target statistics_options;
  
  JSON_NAME(name)
  JSON_NAME(enabled)
  JSON_NAME(suspend)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)
  JSON_NAME(cpu)
  JSON_NAME(workflow)
  JSON_NAME(statistics)

  enum 
  {
    has_name       =  ( Features & component_features::Singleton        ) == 0,
    has_enabled    =  ( Features & component_features::DisabledEnabled  ) == 0,
    has_suspend    =  ( Features & component_features::DisabledSuspend  ) == 0,
    has_priority   =  ( Features & component_features::DisabledPriority ) == 0,
    has_workflow   =  ( Features & component_features::DisabledWorkflow ) == 0,
    has_cpu        =  ( Features & component_features::EnableCPU        ) != 0,
    has_statistics =  options_type::statistics_enabled
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
      >::type,
      typename optional_member<
        has_cpu,
        n_cpu,
        options_type,
        std::set<int>,
        &options_type::cpu,
        ::wjson::array< std::set< ::wjson::value<int> >  >
      >::type,
      typename optional_member<
        has_statistics,
        n_statistics,
        options_type,
        statistics_options,
        &options_type::statistics,
        stat_json
      >::type
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename DomainJson, int Features /*= 0*/, typename StatOptJson /*= nostat_json*/ >
struct instance_options_json
{
  typedef StatOptJson stat_json;
  typedef basic_instance_options_json<Features, stat_json> basic_json;
  typedef DomainJson domain_json;
  typedef typename domain_json::target domain_options;
  typedef typename stat_json::target stat_options;
  typedef instance_options<domain_options, stat_options> options_type;

  typedef ::wjson::object<
    options_type,
    ::wjson::member_list<
      ::wjson::base< basic_json >,
      ::wjson::base< domain_json >
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};


}
