#pragma once 

#include <wfc/json.hpp>
#include <iow/workflow/queue_options_json.hpp>
#include <wfc/core/workflow_options.hpp>

namespace wfc {
  
struct workflow_options_json
{
  JSON_NAME(threads)
  JSON_NAME(enabled)
 
  typedef json::object<
    workflow_options,
    json::member_list<
      json::base< ::iow::queue_options_json >,
      json::member< n_enabled, workflow_options, bool, &workflow_options::enabled >,
      json::member< n_threads, workflow_options, int,  &workflow_options::threads >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}