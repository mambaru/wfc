#pragma once 


#include <wfc/jsonrpc/engine_base.hpp>
#include <iow/jsonrpc/engine.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler, template<typename> class Opt>
class basic_engine
  : public engine_base<
              Interface, 
              ::iow::jsonrpc::engine<JsonrpcHandler>,
              Opt< typename ::iow::jsonrpc::engine<JsonrpcHandler>::options_type > 
           >
{};

}}