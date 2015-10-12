#pragma once 

#include <wfc/iinterface.hpp>

#include <iow/jsonrpc/handler/method_list.hpp>
#include <iow/jsonrpc/handler/aspect/interface_.hpp>
#include <iow/jsonrpc/handler/aspect/target.hpp>
#include <iow/jsonrpc/handler/aspect/peeper.hpp>
#include <iow/jsonrpc/method/mem_fun/connect_method.hpp>
#include <iow/jsonrpc/method/mem_fun/disconnect_method.hpp>


namespace wfc{ namespace jsonrpc{

template<typename ...Args >
struct method_list_helper
{
  typedef ::iow::jsonrpc::method_list<
    Args...,
    ::iow::jsonrpc::interface_<iinterface>,
    ::iow::jsonrpc::target<iinterface>,
    ::iow::jsonrpc::peeper<iinterface>,
    ::iow::jsonrpc::connect_method<iinterface, iinterface, &iinterface::reg_io >,
    ::iow::jsonrpc::disconnect_method<iinterface, &iinterface::unreg_io>
  > base;
  
  typedef typename base::interface_type interface_type;
};

}}
