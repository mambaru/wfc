#pragma once

#include <wfc/core/workflow_options.hpp>
#include <wfc/asio.hpp>
#include <chrono>

namespace iow{ class workflow;}
namespace wfc{

class workflow
{
  typedef ::iow::workflow impl;
public:
  typedef ::wfc::asio::io_service io_service;
  typedef std::function< void () > post_handler;
  
  typedef std::chrono::time_point<std::chrono::system_clock>  time_point;
  typedef time_point::duration                                  duration;
  typedef std::function< bool() >  timer_handler;
  typedef std::function< void(bool) >  callback_timer_handler;
  typedef std::function< void(callback_timer_handler) >  async_timer_handler;
  typedef int timer_id;
  
  virtual ~workflow();
  workflow(io_service& io);
  void start();
  void configure(workflow_options opt);
  void reconfigure(workflow_options opt);
  void stop();

  bool post(post_handler handler);
  bool post(time_point, post_handler handler);
  bool post(duration,   post_handler handler);
  

  timer_id create_timer(duration, timer_handler, bool);
  timer_id create_timer(duration, async_timer_handler, bool);

  timer_id create_timer(time_point, duration, timer_handler, bool);
  timer_id create_timer(time_point, duration, async_timer_handler, bool);

  timer_id create_timer(std::string, duration, timer_handler, bool);
  timer_id create_timer(std::string, duration, async_timer_handler, bool);

  std::shared_ptr<bool> detach_timer(timer_id);
  bool release_timer( timer_id id );
  
  size_t timer_size() const;
  size_t queue_size() const;
  
private:
  
  std::shared_ptr<impl> _impl;
};

}
