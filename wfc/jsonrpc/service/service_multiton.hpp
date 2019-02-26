//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/service/service.hpp>
#include <wfc/jsonrpc/service/service_options_json.hpp>
#include <wfc/jsonrpc/statistics_options_json.hpp>
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{ namespace jsonrpc{

/**
 * @brief service_multiton
 * @tparam Name
 * @tparam MethodList
 * @tparam Impl
 **/
template< typename Name, typename MethodList, template<typename> class Impl = interface_implementation >
class service_multiton: public ::wfc::multiton<
  Name,
  ::wfc::instance< service< MethodList, Impl > >,
  ::wfc::jsonrpc::service_options_json< typename service< MethodList, Impl >::options_type >,
  ::wfc::component_features::DisabledPriority,
  statistics_options_json
>
{
public:
  virtual std::string interface_name() const override
  {
    return "ijsonrpc";
  }
  
  std::list<std::string> get_method_list() const
  {
    return service< MethodList, Impl >::get_method_list();
  }
  
  virtual std::string help(const std::string& args) const override
  {
    return service< MethodList, Impl >::schema_help(this->name(), args);
  }

private:
  
};

}}
