#pragma once
#include <iostream>

#include <wfc/jsonrpc/service/service_aspect.hpp>

#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace jsonrpc{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class service_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< service_base<A, AspectClass> >
{
public:
  typedef service_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::aspect::template advice_cast<_worker_type_>::type worker_type;

  service_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
    , handler(std::bind( &self::operator(), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) )
    , tmp_worker(io_service, opt)
  {
    super::create(*this);
  }

  void operator()( data_ptr d, std::weak_ptr<wfc::io::iio> iio, wfc::io::callback&& handler)
  {
    auto dd = std::make_shared<data_ptr>( std::move(d) );
    this->dispatch([this, dd, iio, handler]()
    {
      std::cout << "jsonrpc_base" << std::endl; 
      this->get_aspect().template get<_incoming_>()( *this, std::move(*dd), iio, std::move(handler) );
    });
  }
  
  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }

  wfc::io::handler handler;
  
  worker_type tmp_worker;
};

}} // wfc


