#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{ namespace jsonrpc{

template< typename Name, typename MethodList, template<typename> class Impl = interface_implementation >
class service_multiton: public ::wfc::multiton<
  Name,
  wfc::instance< service< MethodList, Impl > >,
  wfc::jsonrpc::options_json< typename service< MethodList, Impl >::options_type >
>
{
};

}}