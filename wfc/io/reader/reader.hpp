#pragma once

//#include <wfc/io/reader/read/sync/tags.hpp>
//#include <wfc/io/reader/read/async_callback/tags.hpp>
#include <wfc/io/reader/tags.hpp>
#include <wfc/io/reader/aspect.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/io/types.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace reader{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class reader
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< reader<A, AspectClass> >
{
public:
  
  typedef reader<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::options_type options_type;
  
public:

  reader(const reader& ) = delete;
  void operator = (const reader& conf) = delete;

  reader(descriptor_type&& desc, const options_type& opt)
    : super( std::move(desc), opt)
  {
    super::create(*this);
  }

  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }
  
  /*
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
  */

  data_ptr read()
  {
    return this->get_aspect().template get< _read_ >()(*this);
  }

  void async_read(wfc::io::callback handler)
  {
    this->get_aspect().template get< _async_read_ >()(*this, handler);
  }

  /*
  template<typename ...Args>
  typename super::aspect::template advice_cast<_read_>::type::template return_type<self>::type
  read(Args&& ...args)
  {
    return this->get_aspect().template get< _read_ >()(*this, std::forward<Args>(args)...);
  }
  */
  
private:

};

}}}
