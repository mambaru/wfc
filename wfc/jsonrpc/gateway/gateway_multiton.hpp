//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/statistics_options_json.hpp>
#include <wfc/jsonrpc/gateway/gateway.hpp>
#include <wfc/jsonrpc/gateway/gateway_options_json.hpp>
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{ namespace jsonrpc{

namespace helper
{
  template< typename Name, typename MethodList, template<typename> class Impl >
  struct make_gateway_multiton
  {
    typedef wfc::multiton<
      Name,
      wfc::instance< gateway< MethodList, Impl > >,
      wfc::jsonrpc::gateway_options_json< typename gateway< MethodList, Impl >::options_type >,
      wfc::component_features::DisabledPriority,
      statistics_options_json
    > type;
  };
}


template< typename Name, typename MethodList, template<typename> class Impl = interface_implementation >
class gateway_multiton: public helper::make_gateway_multiton<Name, MethodList, Impl>::type
{
  typedef typename helper::make_gateway_multiton<Name, MethodList, Impl>::type super;
public:
  std::list<std::string> get_method_list() const
  {
    return gateway< MethodList, Impl >::get_method_list();
  }

  virtual std::string help(const std::string& args) const override
  {
    std::string strhelp = super::help(args);
    return gateway< MethodList, Impl >::schema_help(this->name(), args, strhelp);
  }

};

}}
