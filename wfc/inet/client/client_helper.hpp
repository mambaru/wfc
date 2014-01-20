#pragma once 

#include <wfc/inet/client/tags.hpp>
#include <wfc/inet/client/aspect_client_default.hpp>
#include <wfc/inet/tags.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace inet{
  
template<typename... Args>
struct client_helper
{
  typedef typename fas::merge_aspect< fas::aspect<Args...>, aspect_client_default>::type aspect_type;
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
  /// client
  ///
  typedef typename aspect::template advice_cast<_client_aspect_>::type client_aspect_type;
  typedef fas::aspect_class<client_aspect_type> client_base;
  
  typedef typename client_base::aspect
    ::template advice_cast<_connection_manager_type_>::type
    ::template apply<connection_type>::type connection_manager_type;

  
  typedef typename client_base::aspect::template advice_cast<_context_>::type client_context_type;
  typedef typename client_base::aspect::template advice_cast<_configurator_>::type::config_type config_type;
  
  
  

};

}}