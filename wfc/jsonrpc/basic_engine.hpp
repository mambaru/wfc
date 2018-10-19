//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/jsonrpc/engine_base.hpp>
#include <wjrpc/engine.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler, template<typename> class Opt>
class basic_engine
  : public engine_base<
              Interface, 
              ::wjrpc::engine<JsonrpcHandler>,
              Opt< typename ::wjrpc::engine<JsonrpcHandler>::options_type > 
           >
{};

}}
