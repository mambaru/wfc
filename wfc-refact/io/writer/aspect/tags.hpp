#pragma once

//#include <wfc/io/writer/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace wfc{ namespace io{ namespace writer{
  
struct _error_code_;
struct _write_;
struct _async_write_;
struct _incoming_;
struct _output_;
struct _create_;
struct _handler_;
struct _ready_;
struct _process_;
struct _write_some_;
struct _make_buffer_;
struct _free_buffer_;
struct _outgoing_buffer_size_;
struct _outgoing_buffer_list_;
struct _write_handler_;
struct _write_ready_;
struct _write_aborted_;
struct _write_error_;
struct _on_write_;
struct _write_more_;
struct _preapare_buffer_;

struct _on_abort_;
struct _on_error_;
struct _trace_;
struct _log_abort_;
struct _log_error_;

}}}
