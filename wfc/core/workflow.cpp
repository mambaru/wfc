#include <wfc/core/workflow.hpp>
#include "global.hpp"
#include <iow/workflow/workflow.hpp>

namespace wfc{
  

  
std::shared_ptr<workflow> workflow::create(workflow_options opt)
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if ( opt.enabled )
      return std::shared_ptr<workflow>( new workflow( g->io_service, opt ) );
    else
      return g->workflow;
  }
  else
  {
    return std::shared_ptr<workflow>( new workflow( opt ) );
  }
}
  
workflow::~workflow()
{
  
}

workflow::workflow(workflow_options opt )
{
  _impl = std::make_shared<impl>(opt, opt.threads);
}

workflow::workflow(io_service& io, workflow_options opt)
{
  _impl = std::make_shared<impl>(io, opt, opt.threads);
}

void workflow::start()
{
  _impl->start();
}

void workflow::reconfigure(workflow_options opt)
{
  _impl->reconfigure(opt, opt.threads);
}

void workflow::stop()
{
  _impl->stop();
}

bool workflow::post(post_handler handler)
{
  return _impl->post( std::move(handler) );
}

bool workflow::post(time_point_t tp, post_handler handler)
{
  return _impl->post_at( tp, std::move(handler) );
}

bool workflow::post(duration_t d,   post_handler handler)
{
  return _impl->delayed_post(d, std::move(handler) );
}

workflow::timer_id_t workflow::create_timer(duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(time_point_t tp, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(time_point_t tp, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(std::string tp, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(std::string tp, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

std::shared_ptr<bool> workflow::detach_timer(timer_id_t id)
{
  return _impl->timer()->detach(id);
}

bool workflow::release_timer( timer_id_t id )
{
  return _impl->timer()->release(id);
}
  
size_t workflow::timer_size() const
{
  return _impl->timer()->size();
}

size_t workflow::queue_size() const
{
  return _impl->size();
}
  
}
