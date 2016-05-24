#include <wfc/core/workflow.hpp>
#include "global.hpp"
#include <iow/workflow/workflow.hpp>

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

std::shared_ptr<workflow> workflow::create(::wfc::asio::io_service& io, iow::queue_options opt)
{
  workflow_options wopt;
  static_cast<iow::queue_options&>(wopt) = opt;
  wopt.threads = 0;
  return std::shared_ptr<workflow>( new workflow( io, wopt ) );
}

workflow::~workflow()
{
  
}

workflow::workflow(workflow_options opt )
{
  _impl = std::make_shared<impl>(opt, opt.threads);
  _impl->rate_limit( opt.rate_limit );
  _delay_ms = opt.post_delay_ms;
}

workflow::workflow(io_service& io, workflow_options opt)
{
  _impl = std::make_shared<impl>(io, opt, opt.threads, opt.use_io_service);
  _impl->rate_limit( opt.rate_limit );
  _delay_ms = opt.post_delay_ms;
}

void workflow::start()
{
  _impl->start();
}

void workflow::reconfigure(workflow_options opt)
{
  _impl->rate_limit( opt.rate_limit );
  _impl->reconfigure(opt, opt.threads, opt.use_io_service);
  
}

void workflow::stop()
{
  _impl->stop();
}

std::shared_ptr< workflow::impl > workflow::get() const
{
  return _impl;
}

bool workflow::post(post_handler handler)
{
  if ( _delay_ms == 0)
    return _impl->post( std::move(handler) );
  else
    return this->post( std::chrono::milliseconds(_delay_ms), std::move(handler) );
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

workflow::timer_id_t workflow::create_async_timer(duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(duration_t sd, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create( sd, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(duration_t sd, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create( sd, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(time_point_t tp, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(time_point_t tp, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(std::string tp, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(std::string tp, duration_t d, async_timer_handler handler, bool expires_after)
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
