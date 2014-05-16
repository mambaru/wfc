#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/basic_io.hpp>
#include <fas/aop.hpp>
#include <functional>
#include <boost/asio.hpp>
namespace wfc{ namespace io{ 

struct _holder_create_;

struct ad_holder_create
{
  template<typename T, typename Tmp>
  void operator()(T& t, Tmp)
  {
    t.get_aspect().template get<_status_>()=true;
  }
};
  
struct holder_aspect: fas::aspect<
  fas::advice< _holder_create_, ad_holder_create>,
  fas::group< _create_, _holder_create_>,
  fas::value< _status_, bool>,
  fas::value< _error_code_, boost::system::error_code>
>{};

  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class descriptor_holder
  : public basic_io< typename fas::merge_aspect<A, holder_aspect>::type , AspectClass>
{
public:
  
  typedef descriptor_holder<A, AspectClass> self;
  typedef basic_io< typename fas::merge_aspect<A, holder_aspect>::type , AspectClass> super;
  typedef typename super::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;
  typedef typename super::options_type options_type;
  

  descriptor_holder(descriptor_type&& desc, const options_type& conf/*, ::wfc::io::incoming_handler handler = nullptr*/)
    : super( desc.get_io_service(), conf)
    , _descriptor( std::move(desc) )
    // , _handler(handler)
    , _handler(conf.incoming_handler)
  {
  }
  
  descriptor_type& descriptor()
  {
    return _descriptor;
  }

  const descriptor_type& descriptor() const
  {
    return _descriptor;
  }
  
  bool status() const 
  {
    return this->get_aspect().template get< ::wfc::io::_status_>();
  }
  
  const boost::system::error_code& error_code() const 
  {
    return this->get_aspect().template get< ::wfc::io::_error_code_>();
  }

  template<typename T>
  void self_stop(T& t, std::function<void()> finalize)
  {
    if ( this->descriptor().is_open() )
      this->descriptor().close();
    super::self_stop(t, finalize);
  }

  template<typename T>
  void stop_impl(T& t, std::function<void()> finalize)
  {
      if ( t.descriptor().is_open() )
      {
        t.descriptor().cancel();
        t.descriptor().close();
      }
    
      super::stop(t, finalize);
  }
  template<typename T>
  void stop(T& t, std::function<void()> finalize)
  {
    std::atomic<bool> flag(false);
    
    super::get_io_service().post( super::strand().wrap( [this, &t, &flag, finalize](){
      this->stop_impl(t, finalize);
      flag = true;
    }));
    
    
    while (!flag)
    {
      super::get_io_service().reset();
      super::get_io_service().poll();
    }
  }

private:
  
  descriptor_type _descriptor;

public:
  // TODO: в аспект
  ::wfc::io::incoming_handler_t _handler;
  

};

}}
