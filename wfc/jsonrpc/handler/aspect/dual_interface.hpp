#pragma once

#include <wfc/jsonrpc/handler/aspect/target.hpp>
#include <wfc/jsonrpc/handler/aspect/interface_.hpp>
#include <wfc/jsonrpc/handler/aspect/provider.hpp>
#include <wfc/jsonrpc/handler/aspect/startup.hpp>
#include <wfc/jsonrpc/handler/aspect/shutdown.hpp>

#include <wfc/jsonrpc/method/mem_fun/mem_fun_shutdown.hpp>
#include <wfc/jsonrpc/method/mem_fun/mem_fun_startup.hpp>


#include <fas/type_list.hpp>


namespace wfc{ namespace jsonrpc{

// #warning TODO: shutdown_handler
// упрать из этой папки - это вспомогательный элемент
template<typename I, typename P >
struct dual_interface: 
  fas::type_list_n<
    target<I>,
    provider< P >,
    interface_<I>,
    shutdown< mem_fun_shutdown< P, &P::shutdown> >,
    startup< mem_fun_startup< I, P, &P::startup> >
  >::type 
{
};


}} // wfc


