#pragma once

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct _invoke_;
struct _input_;
struct _output_;
struct _outgoing_;
struct _method_;

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

}}}}
