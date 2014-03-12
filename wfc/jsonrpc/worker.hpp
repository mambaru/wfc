#pragma once
#include <iostream>

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/io_service.hpp>
#include <wfc/json/json.hpp>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>

#include "service/service_aspect.hpp"



//////
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>

#include <wfc/jsonrpc/handler/handler_base.hpp>


namespace wfc{ namespace jsonrpc{

struct worker_options
  : wfc::io::basic::options
{
  //std::shared_ptr<handler_base> handler;
};

/// WORKER

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class worker_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< worker_base<A, AspectClass> >
{
public:
  typedef worker_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;
  
  worker_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
    super::create(*this);
  }
  
  // TODO: вынести method_handler из incoming_holder
  // TODO: вместо handler_base std::functional
  // или вообще убрать worker и оставить io_base
  void operator()(incoming_holder holder, std::weak_ptr<handler_base> wh, wfc::io::callback callback/*,   TODO: weak_ptr<handler_base> */)
  {

    auto ph = std::make_shared<incoming_holder>(std::move(holder) );
    super::dispatch([this, ph, wh, callback]()
    {
      if ( auto h = wh.lock() )
      {
        h->process( std::move(*ph), callback );
      }
      else
      {
      }
    }); // dispatch
  }
private:
  // Нахрен отсюдова!!! 
  //std::map< wfc::io::iio*, std::shared_ptr<handler_base> > _instance_map;
};

struct worker_aspect: fas::aspect<
  fas::type<wfc::io::_options_type_, worker_options>
> {};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class worker:
  public worker_base< typename fas::merge_aspect< A, worker_aspect>::type, AspectClass>
{
public:
  typedef worker<A, AspectClass> self;
  typedef worker_base< typename fas::merge_aspect< A, worker_aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  worker(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }
};

}} // wfc


