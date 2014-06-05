#pragma once
#include <iostream>

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/io/strand.hpp>
#include <wfc/io_service.hpp>
#include <wfc/json/json.hpp>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>
#include <wfc/jsonrpc/options.hpp>




#include <wfc/jsonrpc/types.hpp>
#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/service/aspect/aspect.hpp>

namespace wfc{ namespace jsonrpc{
  

/// /////////////////////////////////////////


struct service_basic_aspect: fas::aspect<
  fas::advice<_input_, ad_input>,
  fas::advice<_incoming_parser_, ad_incoming_parser>,
  fas::advice<_verify_,ad_verify>,
  fas::advice<_handler_switch_,ad_handler_switch>,
  fas::advice<_callback_json_,  ad_callback_json>,
  fas::advice<_callback_error_, ad_callback_error>,
  fas::advice<_error_handler_,  ad_error_handler>,
  fas::advice<_method_handler_, ad_method_handler>,
  fas::advice<_result_handler_, ad_result_handler>
>{};

struct service_aspect: fas::aspect<
  service_basic_aspect::advice_list,
  fas::type< ::wfc::io::_options_type_, ::wfc::jsonrpc::options>,
  fas::type<_worker_type_, wfc::io::strand >
>{};

  
}} // wfc


