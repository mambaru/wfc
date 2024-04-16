//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/core_status.hpp>
#include <wfc/json.hpp>

namespace wfc{

struct core_status_json
{
  JSON_NAME(OK)
  JSON_NAME(WARINING)
  JSON_NAME(ERROR)
  JSON_NAME(ABORT)
  JSON_NAME(HANG)

  typedef wjson::enumerator<
    core_status,
    wjson::member_list<
      wjson::enum_value<n_OK, core_status, core_status::OK>,
      wjson::enum_value<n_WARINING, core_status, core_status::WARINING>,
      wjson::enum_value<n_ERROR, core_status, core_status::ERROR>,
      wjson::enum_value<n_ABORT, core_status, core_status::ABORT>,
      wjson::enum_value<n_HANG, core_status, core_status::HANG>
    >
  > meta;
  typedef typename meta::target      target;
  typedef typename meta::member_list member_list;
  typedef typename meta::serializer  serializer;
};

struct core_stage_json
{
  JSON_NAME(UNDEFINED)
  JSON_NAME(CONFIGURING)
  JSON_NAME(INITIALIZING)
  JSON_NAME(STARTING)
  JSON_NAME(IS_RUN)
  JSON_NAME(STOPPING)

  typedef wjson::enumerator<
    core_stage,
    wjson::member_list<
      wjson::enum_value<n_UNDEFINED, core_stage, core_stage::UNDEFINED>,
      wjson::enum_value<n_CONFIGURING, core_stage, core_stage::CONFIGURING>,
      wjson::enum_value<n_INITIALIZING, core_stage, core_stage::INITIALIZING>,
      wjson::enum_value<n_STARTING, core_stage, core_stage::STARTING>,
      wjson::enum_value<n_IS_RUN, core_stage, core_stage::IS_RUN>,
      wjson::enum_value<n_STOPPING, core_stage, core_stage::STOPPING>
    >
  > meta;
  typedef typename meta::target      target;
  typedef typename meta::member_list member_list;
  typedef typename meta::serializer  serializer;
};

}

