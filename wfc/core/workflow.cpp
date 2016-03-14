#include <wfc/core/workflow.hpp>
#include <iow/workflow/workflow.hpp>

namespace wfc{
  

  
workflow::~workflow()
{
  
}

workflow::workflow(io_service& io)
{
  ::iow::queue_options opt;
  _impl = std::make_shared<impl>(io, opt, 0);
}

void workflow::start()
{
  _impl->start();
}

void workflow::configure(workflow_options opt)
{
  this->reconfigure(opt);
}

void workflow::reconfigure(workflow_options conf)
{
  ::iow::queue_options opt;
  opt.wrnsize = conf.wrnsize;
  opt.maxsize = conf.maxsize;
  _impl->reconfigure(opt, conf.threads);
  
}

void workflow::stop()
{
  _impl->stop();
}

bool workflow::post(post_handler handler)
{
  return _impl->post( std::move(handler) );
}

bool workflow::post(time_point tp, post_handler handler)
{
  return _impl->post_at( tp, std::move(handler) );
}

bool workflow::post(duration d,   post_handler handler)
{
  return _impl->delayed_post(d, std::move(handler) );
}
  

workflow::timer_id workflow::create_timer(duration d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id workflow::create_timer(duration d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id workflow::create_timer(time_point tp, duration d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id workflow::create_timer(time_point tp, duration d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id workflow::create_timer(std::string tp, duration d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id workflow::create_timer(std::string tp, duration d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

std::shared_ptr<bool> workflow::detach_timer(timer_id id)
{
  return _impl->timer()->detach(id);
}

bool workflow::release_timer( timer_id id )
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
