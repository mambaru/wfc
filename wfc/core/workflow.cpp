#include "global.hpp"
#include "workflow.hpp"

namespace wfc{
  
std::shared_ptr<workflow> workflow::create_and_start(workflow_options opt)
{
  std::shared_ptr<workflow> wrk;
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if ( opt.enabled )
      wrk = std::shared_ptr<workflow>( new workflow( g->io_service, opt ) );
    else
      return g->workflow;
  }
  else
  {
    wrk = std::shared_ptr<workflow>( new workflow( opt ) );
  }
  wrk->start();
  return wrk;
}

std::shared_ptr<workflow> workflow::recreate_and_start( std::shared_ptr<workflow> wrk,  workflow_options opt )
{
  if ( wrk == nullptr )
    return workflow::create_and_start(opt);
  
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if ( opt.enabled )
    {
      if ( wrk == g->workflow ) wrk = workflow::create_and_start(opt);
      else wrk->reconfigure(opt);
    }
    else
      return g->workflow;
  }
  else
  {
    wrk->reconfigure(opt);
  }
  return wrk;
}

std::shared_ptr<workflow> workflow::create(io_service_type& io, iow::queue_options opt)
{
  workflow_options wopt;
  static_cast<iow::queue_options&>(wopt) = opt;
  wopt.threads = 0;
  return std::shared_ptr<workflow>( new workflow( io, wopt ) );
}

workflow::workflow(workflow_options opt )
  : super(opt)
{}

workflow::workflow(io_service_type& io, workflow_options opt)
  : super(io, opt)
{}

  
}
