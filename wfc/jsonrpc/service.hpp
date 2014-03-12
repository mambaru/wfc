#pragma once

#include <iostream>


#include <wfc/jsonrpc/service/service_base.hpp>
#include <wfc/jsonrpc/service/service_aspect.hpp>
#include <fas/aop.hpp>



namespace wfc{ namespace jsonrpc{
  

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class basic_service:
  public service_base< typename fas::merge_aspect< A, service_aspect>::type, AspectClass>
{
public:
  typedef basic_service<A, AspectClass> self;
  typedef service_base< typename fas::merge_aspect< A, service_aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  basic_service(io_service_type& io_service, const options_type& opt, const handler_base& handler)
    : super( io_service, opt, handler)
  {
  }
};


class service_impl
  : public basic_service< >
{
public:
  typedef basic_service<> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  service_impl(io_service_type& io_service, const options_type& opt, const handler_base& handler)
    : super( io_service, opt, handler)
  {
  }
};

class service
{
public:
  typedef service_options options_type;
  
  service(wfc::io_service& io_service, const options_type& opt, const handler_base& handler)
    : _impl( std::make_shared<service_impl>(io_service, opt, handler) )
  {
  }
  
  void startup_handler(wfc::io::io_id_t io_id, wfc::io::callback writer, wfc::io::add_shutdown_handler add_shutdown )
  {
    _impl->startup_handler(io_id, writer, add_shutdown);
  }

  void operator()( wfc::io::data_ptr d, wfc::io::io_id_t id, wfc::io::callback callback)
  {
    _impl->operator ()( std::move(d), id, callback);
  }

  
  void start()
  {
    _impl->start();
  }

  void stop()
  {
    _impl->start();
  }

private:
  
  std::shared_ptr<service_impl> _impl;
  
};



}} 