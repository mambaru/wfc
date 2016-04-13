#pragma once

#include <wfc/core/workflow_options.hpp>
#include <iow/workflow/workflow.hpp>
#include <wfc/asio.hpp>
#include <chrono>

namespace iow{ class workflow;}
namespace wfc{

class workflow
{
  typedef ::iow::workflow impl;
public:
  typedef ::wfc::asio::io_service io_service;
  typedef std::function< void() > post_handler;
  
  typedef iow::workflow::timer_type::timer_id_t timer_id_t;
  typedef iow::workflow::timer_type::time_point_t  time_point_t;
  typedef iow::workflow::timer_type::duration_t    duration_t;

  typedef iow::workflow::timer_type::handler1 timer_handler;
  typedef iow::workflow::timer_type::handler_callback  callback_timer_handler;
  typedef iow::workflow::timer_type::handler2 async_timer_handler;
  
  virtual ~workflow();

  friend struct wfcglobal;
private:
  explicit workflow(workflow_options opt = workflow_options() );
  workflow(io_service& io, workflow_options opt = workflow_options() );

public:
  static std::shared_ptr<workflow> create(workflow_options opt);
  // простая очередь без потоков
  static std::shared_ptr<workflow> create(::wfc::asio::io_service& io, iow::queue_options opt);
  
  void start();
  void reconfigure(workflow_options opt);
  void stop();

  bool post(post_handler handler);
  bool post(time_point_t, post_handler handler);
  bool post(duration_t,   post_handler handler);


  timer_id_t create_timer(duration_t, timer_handler, bool = true);
  timer_id_t create_timer(duration_t, async_timer_handler, bool = true);

  timer_id_t create_timer(duration_t, duration_t, timer_handler, bool = true);
  timer_id_t create_timer(duration_t, duration_t, async_timer_handler, bool = true);

  timer_id_t create_timer(time_point_t, duration_t, timer_handler, bool = true);
  timer_id_t create_timer(time_point_t, duration_t, async_timer_handler, bool = true);

  timer_id_t create_timer(std::string, duration_t, timer_handler, bool = true);
  timer_id_t create_timer(std::string, duration_t, async_timer_handler, bool = true);
  
  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return _impl->timer()->create<Req, Res>( d, i, mem_fun, std::move(handler) );
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( duration_t sd, duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return _impl->timer()->create<Req, Res>( sd, d, i, mem_fun, std::move(handler) );
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( std::string st, duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return _impl->timer()->create<Req, Res>(st, d, i, mem_fun, std::move(handler) );
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( time_point_t tp, duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return _impl->timer()->create<Req, Res>( tp, d, i, mem_fun, std::move(handler) );
  }

  std::shared_ptr<bool> detach_timer(timer_id_t );
  
  bool release_timer( timer_id_t id );
  
  size_t timer_size() const;
  size_t queue_size() const;
  
private:
  
  std::shared_ptr<impl> _impl;
};

}
