#pragma once

#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>
#include <functional>

namespace wfc{ namespace io{ 
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class io_base
  : public AspectClass<A>
{
public:
  
  typedef descriptor_holder<A, AspectClass> self;
  typedef AspectClass<A> super;
  
  typedef typename super::aspect::template advice_cast<_context_>::type context_type;
  typedef typename super::aspect::template advice_cast<_data_type_>::type data_type;
  typedef typename super::aspect::template advice_cast<_strand_type_>::type strand_type;
  typedef typename super::aspect::template advice_cast<_options_type_>::type options_type;
  typedef typename super::aspect::template advice_cast<_io_service_type_>::type io_service_type;
  
  io_base(descriptor_type desc)
    : 
  {
  }
  
  context_type& context()
  {
    return this->get_aspect().template get<_context_>();
  }

  const context_type& context() const
  {
    return this->get_aspect().template get<_context_>();
  }

  io_service_type& get_io_service()
  {
    // return this->descriptor().get_io_service();
    //return *(this->get_aspect().template get<_io_service_ptr_>());
  }

  const io_service_type& get_io_service() const
  {
    return _io_service;
    // return this->descriptor().get_io_service();
    //return *(this->get_aspect().template get<_io_service_ptr_>());
  }
  
  strand_type& strand()
  {
    return *(this->get_aspect().template get<_strand_>());
  }

  const strand_type& strand() const
  {
    return *(this->get_aspect().template get<_strand_>());
  }
  

  const options_type& options() const
  {
    return _options;
  }

protected:

  template<typename T, typename Handler>
  void dispatch(T& t, Handler h)
  {
    this->get_aspect().template get<_dispatch_>()(t, h);
  }

  template<typename T, typename Handler>
  void post(T& t, Handler h)
  {
    this->get_aspect().template get<_dispatch_>()(t, h);
  }

  template<typename T>
  void create(T& t, const options_type& conf)
  {
    _options = conf;
    t.get_aspect().template gete<_create_>()(t, _options);
  }

  /*
  template<typename T, typename Config>
  void create(T& t, const Config& conf) 
  {
    t.get_aspect().template gete<_create_>()(t);
    t.get_aspect().template gete<_configure_>()(t, conf);
  }

  template<typename T, typename Config, typename Init>
  void create(T& t, const Config& conf, const Init& init) 
  {
    t.get_aspect().template gete<_create_>()(t);
    t.get_aspect().template gete<_configure_>()(t, conf);
    t.get_aspect().template gete<_initialize_>()(t, init);
  }
  */

  /*
  template<typename T, typename Config>
  void configure(T& t, const Config& conf)
  {
    t.get_aspect().template gete<_configure_>()( t, conf );
  }

  template<typename T, typename Init>
  void initialize(T& t, const Init& init)
  {
    t.get_aspect().template gete<_initialize_>()(t, init );
  }
  */

  template<typename T>
  void start(T& t)
  {
    t.get_aspect().template get<_start_>()(t);
  }
  
  template<typename T>
  void stop(T& t)
  {
    t.get_aspect().template get<_stop_>()(t);
  }
  
  template<typename T, typename Handler>
  std::function<void()> wrap(T& t, Handler&& handler)
  {
    return t.get_aspect().template get<_wrap_>()(t, handler);
  }
  
  

private:
  wfc::io_service& _io_service;
  options_type _options;
};

}}
