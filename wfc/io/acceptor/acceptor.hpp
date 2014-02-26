#pragma once

//#include <wfc/io/reader/read/sync/tags.hpp>
//#include <wfc/io/reader/read/async_callback/tags.hpp>
#include <wfc/io/acceptor/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace acceptor{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class acceptor
  : public io_base<A, AspectClass>
  , public std::enable_shared_from_this< acceptor<A, AspectClass> >
{
public:
  
  typedef acceptor<A, AspectClass> self;
  typedef io_base<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  acceptor(const acceptor& ) = delete;
  void operator = (const acceptor& conf) = delete;

  template<typename Conf>
  acceptor(descriptor_type&& desc, const Conf& conf)
    : super( std::move(desc) )
  {
    super::create(*this, conf);
  }

  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
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
