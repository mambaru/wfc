#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/gateway.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{ namespace jsonrpc{

template< typename Name, typename MethodList, template<typename> class Impl = interface_implementation >
class gateway_multiton: public ::wfc::multiton<
  Name,
  wfc::instance< gateway< MethodList, Impl > >,
  wfc::jsonrpc::options_json< typename gateway< MethodList, Impl >::options_type >
>
{
};

}}