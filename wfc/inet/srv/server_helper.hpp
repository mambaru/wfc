#pragma once

#include <wfc/inet/tags.hpp>
#include <wfc/inet/srv/aspect_default.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{

template<typename... Args>
struct server_helper
{
  typedef typename fas::merge_aspect< fas::aspect<Args...>, aspect_default>::type aspect_type;
  typedef typename fas::aspect_class<aspect_type>::aspect aspect;
  
  ///
  /// connection
  ///
  typedef typename aspect::template advice_cast<_connection_aspect_>::type connection_aspect_type;
  
  template<typename ConnAspect>
  using connection_base_t = typename aspect::template advice_cast<_connection_base_>::type::template type<ConnAspect>;
  
  template<typename ConnAspect, template<typename> class ConnBase >
  using connection_t = typename aspect::template advice_cast<_connection_>::type::template type<ConnAspect, ConnBase>;
  
  typedef connection_t<connection_aspect_type, connection_base_t> connection_type;
  typedef typename connection_type::aspect::template advice_cast<_context_>::type connection_context_type;
  
  ///
  /// server
  ///
  typedef typename aspect::template advice_cast<_server_aspect_>::type server_aspect_type;

  typedef fas::aspect_class<server_aspect_type> server_base;
  typedef typename server_base::aspect::template advice_cast<_context_>::type server_context_type;

};

}}