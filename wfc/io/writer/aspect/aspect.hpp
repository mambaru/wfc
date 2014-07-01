#pragma once

#include <wfc/io/writer/aspect/ad_log_abort.hpp>
#include <wfc/io/writer/aspect/ad_log_error.hpp>
#include <wfc/io/writer/aspect/ad_trace.hpp>
#include <wfc/io/writer/aspect/ad_write_handler.hpp>
#include <wfc/io/writer/aspect/ad_write_ready.hpp>
#include <wfc/io/writer/aspect/ad_write_error.hpp>
#include <wfc/io/writer/aspect/ad_async_write_some.hpp>
#include <wfc/io/writer/aspect/ad_make_buffer.hpp>
#include <wfc/io/writer/aspect/ad_preapare_buffer.hpp>
#include <wfc/io/writer/aspect/ad_write_more.hpp>
#include <wfc/io/writer/aspect/ad_write.hpp>

#include <wfc/io/writer/aspect/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace wfc{ namespace io{ namespace writer{
  
typedef std::list<  ::wfc::io::basic::data_ptr> data_list;

struct aspect: fas::aspect<
  //fas::value< _error_code_, boost::system::error_code>,
  fas::alias<_incoming_, _write_>, 
  fas::advice< _write_some_,    ad_async_write_some >, 
  fas::advice< _write_handler_, ad_write_handler >,
  fas::advice< _write_ready_, ad_write_ready>,
  fas::advice< _write_error_, ad_write_error>, 
  fas::advice< _write_, ad_write>, 
  fas::advice< _make_buffer_, ad_make_buffer>, 
  fas::value< _outgoing_buffer_list_, data_list>, 
  fas::value< _outgoing_buffer_size_, size_t>, 
  
  fas::advice<_preapare_buffer_, ad_preapare_buffer>,
  fas::advice<_write_more_, ad_write_more>, 
  fas::stub<_write_aborted_>, 
  fas::stub<_write_error_>
>{};

template<typename Descriptor,  typename TgOutgoing = _incoming_ >
struct stream: fas::aspect<
  fas::type<  ::wfc::io::_descriptor_type_, Descriptor>,
  aspect
>
{};
  
struct error_log: fas::aspect<
  fas::advice<_log_error_, ad_log_error>,
  fas::advice<_log_abort_, ad_log_abort>,
  fas::advice< _trace_, ad_trace>,
  fas::group< _on_write_, _trace_>,
  fas::group<_on_error_, _log_error_>,
  fas::group<_on_abort_, _log_abort_>
>{};



  

}}}
