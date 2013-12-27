#pragma once

namespace wfc{  namespace jsonrpc{

enum class stat_category
{
  invoke_request, 
  invoke_notify,
  call_request, 
  call_notify, 
};

struct _write_;

struct _invoke_;
struct _input_;
struct _output_;

//struct _outgoing_;
struct _method_;
struct _send_json_;
struct _method_stat_;

/*
// ERROs
struct _not_jsonrpc_;
struct _invalid_json_;

/// jsonrpc 2.0 errors
struct _parse_error_;
struct _invalid_request_;
struct _method_not_found_;
struct _invalid_params_;
struct _internal_error_;
struct _server_error_;
*/
}}
