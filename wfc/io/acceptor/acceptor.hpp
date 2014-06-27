#pragma once

//#include <wfc/io/reader/read/sync/tags.hpp>
//#include <wfc/io/reader/read/async_callback/tags.hpp>
#include <wfc/io/acceptor/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace acceptor{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class acceptor
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< acceptor<A, AspectClass> >
{
public:
  
  typedef acceptor<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::options_type options_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  acceptor(const acceptor& ) = delete;
  void operator = (const acceptor& conf) = delete;

  
  acceptor(descriptor_type&& desc, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( std::move(desc), conf/*, handler*/)
  {
    super::create(*this);
  }

  void start()
  {
    typename super::lock_guard lk(super::mutex());
    DEBUG_LOG_BEGIN("---- aceptor::start ----");
    super::start(*this);
    DEBUG_LOG_END("---- aceptor::start ----");
  }
  
  void stop(std::function<void()> finalize)
  {
    typename super::lock_guard lk(super::mutex());
    DEBUG_LOG_BEGIN("---- acceptor::stop ----");
    super::stop(*this, finalize);
    //super::get_io_service().reset();
    //while ( 0!=super::get_io_service().poll() ) { super::get_io_service().reset();};
    DEBUG_LOG_END("---- acceptor::stop ----");
  }
  
  template<typename Handler>
  std::function<void()> wrap(Handler&& handler)
  {
    return super::wrap(*this, handler);
  }
  
  template<typename Handler>
  void dispatch(Handler h)
  {
    return super::dispatch(*this, h);
  }

  template<typename Handler>
  void post(Handler h)
  {
    return super::post(*this, h);
  }

  template<typename Config>
  void create(const Config& conf)
  {
    return super::create(*this, conf);
    //t.get_aspect().template gete<_create_>()(t, conf);
  }

  /*
   TODO:
  template<typename ...Args>
  typename super::aspect::template advice_cast<_acceptor_>::type::template return_type<self>::type
  accept(Args&& ...args)
  {
    return this->get_aspect().template get< _accept_ >()(*this, std::forward<Args>(args)...);
  }
  */
  
private:

};

}}}
