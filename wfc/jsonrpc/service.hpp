#pragma once

#include <iostream>


#include <wfc/jsonrpc/service/service_base.hpp>
#include <wfc/jsonrpc/service/service_aspect.hpp>
#include <fas/aop.hpp>



namespace wfc{ namespace jsonrpc{
  

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class service:
  public service_base< typename fas::merge_aspect< A, service_aspect>::type, AspectClass>
{
public:
  typedef service<A, AspectClass> self;
  typedef service_base< typename fas::merge_aspect< A, service_aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  service(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }

};

}} 