#pragma once
#include <wfc/gateway/client_options.hpp>
#include <wfc/pubsub/pubsub_gateway_options.hpp>

namespace wfc{ namespace gateway{ 

struct tcp_option: client_options< ::wfc::pubsub::pubsub_gateway_options > {};

}}
