#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/types.hpp>


//#include <wfc/io/reader/read_some.hpp>
//#include <wfc/io/basic/aspect.hpp>
//#include <wfc/io/common/fork.hpp>
//#include <wfc/memory.hpp>
//#include <fas/aop.hpp>
//#include <wfc/logger.hpp>
//#include <wfc/callback/callback_status.hpp>


#include <wfc/io/reader/aspect/ad_async_read_some.hpp>
#include <wfc/io/reader/aspect/ad_read_loop.hpp>
#include <wfc/io/reader/aspect/ad_log_abort.hpp>
#include <wfc/io/reader/aspect/ad_log_error.hpp>
#include <wfc/io/reader/aspect/ad_make_buffer.hpp>
#include <wfc/io/reader/aspect/ad_read_error.hpp>
#include <wfc/io/reader/aspect/ad_read_ready.hpp>
#include <wfc/io/reader/aspect/ad_read_more.hpp>
#include <wfc/io/reader/aspect/ad_start.hpp>
#include <wfc/io/reader/aspect/ad_init.hpp>
#include <wfc/io/reader/aspect/ad_trace.hpp>
#include <wfc/io/reader/aspect/ad_read_handler.hpp>
#include <wfc/io/reader/aspect/ad_user_handler.hpp>
#include <wfc/io/reader/aspect/ad_free_buffer.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

#include <fas/aop.hpp>

#include <list>
#include <atomic>
#include <chrono>

namespace wfc{ namespace io{ namespace reader{ 

/*
 * read_some ->
 *  ^  read_ready
 *  |------ handler ---> incoming ---> outgoing ---> user_handler --> output
 */

struct aspect: fas::aspect<
  fas::group< ::wfc::io::_on_start_, _start_>,
  fas::group< ::wfc::io::_pre_start_, _init_>,
  
  fas::advice< _init_, ad_init >,
  fas::advice< _start_, ad_start >, 
  fas::advice< _read_more_,    ad_read_more >,
  fas::advice< _read_some_,    ad_async_read_some >,
  fas::advice< _read_ready_,   ad_read_ready >,
  fas::advice< _read_error_,   ad_read_error >,
  fas::advice< _read_handler_, ad_read_handler >,
  fas::advice< _read_loop_,    ad_read_loop>,
  fas::advice< _user_handler_, ad_user_handler>,
  fas::advice< _make_buffer_,  ad_make_buffer>,
  fas::advice< _free_buffer_,  ad_free_buffer>,
  fas::stub<_output_>, // результат чтения и обработки 
  fas::alias<_incoming_, _outgoing_>, // заглушка для цепочки обработки
  fas::alias<_outgoing_, _user_handler_>
>{};

/// Еррор лог аспект

struct error_log: fas::aspect<
  fas::advice<_log_error_, ad_log_error>,
  fas::advice<_log_abort_, ad_log_abort>,
  fas::advice< _trace_, ad_trace>,
  fas::group< _on_read_, _trace_>,
  fas::group<_on_error_, _log_error_>,
  fas::group<_on_abort_, _log_abort_>
>{};



}}}
