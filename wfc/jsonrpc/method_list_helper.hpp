//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/iinterface.hpp>

#include <wjrpc/handler/method_list.hpp>
#include <wjrpc/handler/aspect/interface_.hpp>
#include <wjrpc/handler/aspect/target.hpp>
#include <wjrpc/handler/aspect/peeper.hpp>
#include <wjrpc/method/mem_fun/connect_method.hpp>
#include <wjrpc/method/mem_fun/disconnect_method.hpp>
#include <wfc/jsonrpc/ad_callback.hpp>
#include <wjrpc/handler/aspect/tags.hpp>


namespace wfc{ namespace jsonrpc{

template<typename ...Args >
struct method_list_helper
{
  typedef wjrpc::method_list<
      Args...,
      wjrpc::interface_<iinterface>,
      wjrpc::target<iinterface>,
      wjrpc::peeper<iinterface>,
      wjrpc::connect_method<iinterface, iinterface, &iinterface::reg_io >,
      wjrpc::disconnect_method<iinterface, &iinterface::unreg_io>, 
      fas::advice<wjrpc::_callback_, ad_callback>
  > base;
  
  typedef typename base::interface_type interface_type;
};

}}
