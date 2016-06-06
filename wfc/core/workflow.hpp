#pragma once

#include <iow/workflow/workflow.hpp>
#include <wfc/core/workflow_options.hpp>
#include <wfc/core/workflow_options_json.hpp>

namespace wfc{

using ::iow::workflow;

/*
std::shared_ptr<workflow> recreate_and_start_workflow( std::shared_ptr<workflow> wrk,  workflow_options opt );
std::shared_ptr<workflow> create_and_start_workflow(workflow_options opt);
*/

/*
class workflow
  : public ::iow::workflow
{
  typedef ::iow::workflow super;
public:
  typedef super::io_service_type io_service_type;
  friend struct wfcglobal;
protected:
  explicit workflow(workflow_options opt = workflow_options() );
  workflow(io_service_type& io, workflow_options opt = workflow_options() );
public:
  static std::shared_ptr<workflow> create_and_start(workflow_options opt);
  // простая очередь без потоков
  static std::shared_ptr<workflow> create(io_service_type& io, queue_options opt);
  static std::shared_ptr<workflow> recreate_and_start( std::shared_ptr<workflow> wrk,  workflow_options opt );
};
*/

}
