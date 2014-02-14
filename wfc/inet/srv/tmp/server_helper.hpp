#pragma once

#include <wfc/inet/tags.hpp>
#include <wfc/inet/srv/tmp/aspect_default.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{

template<typename... Args>
struct server_helper
{
  /*typedef typename fas::merge_aspect< fas::aspect<Args...>, aspect_default>::type aspect_type;
   * */
  
  typedef fas::aspect< 
    Args... ,
    connection_aspect< conn::echo::rn::stream::tcp::aspect >,
    server_aspect< aspect_server_tcp >,
    connection_base_class< fas::aspect_class >,
    connection_class< connection >
  > aspect_type;
 
  typedef typename fas::aspect_class<aspect_type>::aspect aspect;
  
  ///
  /// connection
  ///
  typedef typename aspect::template advice_cast<_connection_aspect_>::type connection_aspect_type;
  
  template<typename ConnAspect>
  using connection_base_t = typename aspect::template advice_cast<_connection_base_class_>::type::template type<ConnAspect>;
  
  template<typename ConnAspect, template<typename> class ConnBase >
  using connection_t = typename aspect::template advice_cast<_connection_class_>::type::template type<ConnAspect, ConnBase>;
  
  typedef connection_t<connection_aspect_type, connection_base_t> connection_type;
  typedef typename connection_type::aspect::template advice_cast<_context_>::type connection_context_type;
  typedef typename connection_type::aspect::template advice_cast<_socket_type_>::type socket_type;
  
  ///
  /// server
  ///
  typedef typename aspect::template advice_cast<_server_aspect_>::type server_aspect_type;

  typedef fas::aspect_class<server_aspect_type> server_base;

  typedef typename server_base::aspect
    ::template advice_cast<_connection_manager_type_>::type
    ::template apply<connection_type>::type connection_manager_type;
  
  typedef typename server_base::aspect::template advice_cast<_context_>::type server_context_type;
  typedef typename server_base::aspect::template advice_cast<_acceptor_type_>::type acceptor_type;
  typedef typename server_base::aspect::template advice_cast<_configurator_>::type::config_type config_type;
  

};

}}