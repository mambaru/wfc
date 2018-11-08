//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/statistics_options_json.hpp>
#include <wfc/module/domain_options.hpp>
#include <wfc/module/component_features.hpp>
#include <wfc/json.hpp>


namespace wfc{
  
template<bool, typename N, typename V, typename M, M V::* m, typename W = json::value<M> >
struct optional_member;

template<typename N, typename V, typename M, M V::* m, typename W >
struct optional_member<true, N, V, M, m, W>
{
  typedef json::member<N, V, M, m, W> type;
};

template<typename N, typename V, typename M, M V::* m, typename W >
struct optional_member<false, N, V, M, m, W>
{
  typedef ::fas::empty_list type;
};

template<int Features, typename StatOptJson /*= nostat_json*/ >
struct domain_options_json_t
{
  typedef StatOptJson base_stat_json;
  typedef typename base_stat_json::target base_stat_type;
  typedef domain_options_t<base_stat_type> domain_options_type;
  typedef statistics_options_json_t<base_stat_json> stat_json;
  typedef typename stat_json::target statistics_options;
  
  JSON_NAME(name)
  JSON_NAME(enabled)
  JSON_NAME(suspend)
  JSON_NAME(startup_priority)
  JSON_NAME(shutdown_priority)
  JSON_NAME(cpu)
  JSON_NAME(tracking)
  JSON_NAME(workflow)
  JSON_NAME(statistics)

  enum 
  {
    has_name       =  ( Features & static_cast<int>(component_features::Singleton)        ) == 0,
    has_enabled    =  ( Features & static_cast<int>(component_features::DisabledEnabled)  ) == 0,
    has_suspend    =  ( Features & static_cast<int>(component_features::DisabledSuspend)  ) == 0,
    has_priority   =  ( Features & static_cast<int>(component_features::DisabledPriority) ) == 0,
    has_workflow   =  ( Features & static_cast<int>(component_features::DisabledWorkflow) ) == 0,
    has_cpu        =  ( Features & static_cast<int>(component_features::EnableCPU)        ) != 0,
    has_tracking   =  ( Features & static_cast<int>(component_features::EnableTracking)   ) != 0,
    has_statistics =  domain_options_type::statistics_enabled
  };
  
  typedef json::object<
    domain_options_type,
    json::member_list<
      typename optional_member<
        has_name,
        n_name,
        domain_options_type, 
        std::string, 
        &domain_options_type::name
      >::type,
      typename optional_member<
        has_enabled,
        n_enabled,
        domain_options_type,
        bool,
        &domain_options_type::enabled
      >::type,
      typename optional_member<
        has_suspend,
        n_suspend,
        domain_options_type,
        bool,
        &domain_options_type::suspend
      >::type,
      typename optional_member<
        has_tracking,
        n_tracking,
        domain_options_type,
        bool,
        &domain_options_type::tracking
      >::type,
      typename optional_member<
        has_priority,
        n_startup_priority,
        domain_options_type,
        int,
        &domain_options_type::startup_priority
      >::type,
      typename optional_member<
        has_priority,
        n_shutdown_priority,
        domain_options_type,
        int,
        &domain_options_type::shutdown_priority
      >::type,
      typename optional_member<
        has_workflow,
        n_workflow,
        domain_options_type,
        std::string,
        &domain_options_type::workflow
      >::type,
      typename optional_member<
        has_cpu,
        n_cpu,
        domain_options_type,
        std::set<int>,
        &domain_options_type::cpu,
        json::array< std::set< json::value<int> >  >
      >::type,
      typename optional_member<
        has_statistics,
        n_statistics,
        domain_options_type,
        statistics_options,
        &domain_options_type::statistics,
        stat_json
      >::type
    >,
    json::strict_mode
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

  
}
