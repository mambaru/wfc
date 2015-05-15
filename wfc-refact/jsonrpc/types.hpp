#pragma once

#include <wfc/asio.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{

typedef ::wfc::asio::io_service    io_service;
typedef ::wfc::io::io_id_t   io_id_t;
typedef ::wfc::io::data_type data_type;
typedef ::wfc::io::data_ptr  data_ptr;


typedef ::wfc::io::outgoing_handler_t outgoing_handler_t;
typedef ::wfc::io::add_shutdown_handler_t add_shutdown_handler_t;


}} 

