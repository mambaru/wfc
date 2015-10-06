#pragma once 


#include <wfc/jsonrpc/domain_base.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <iow/jsonrpc/engine.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class basic_domain
  : public domain_base<
              Interface, 
              ::iow::jsonrpc::engine<JsonrpcHandler>,
              ::wfc::jsonrpc::options< 
                typename ::iow::jsonrpc::engine<JsonrpcHandler>::options_type
              > 
           >
{};

}}