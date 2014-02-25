#pragma once

#include <wfc/io/writer/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace io{ namespace writer{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class writer
  : public io_base<A, AspectClass>
  , public std::enable_shared_from_this< writer<A, AspectClass> >
{
public:
  
  typedef writer<A, AspectClass> self;
  typedef io_base<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  writer(const writer& ) = delete;
  void operator = (const writer& conf) = delete;

  template<typename Conf>
  writer(descriptor_type&& desc, const Conf& conf)
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

  template<typename ...Args>
  typename super::aspect::template advice_cast<_write_>::type::template return_type<self>::type
  write(Args&& ...args)
  {
    return this->get_aspect().template get< _write_ >()(*this, std::forward<Args>(args)...);
  }


  template<typename ...Args>
  typename super::aspect::template advice_cast<_write_>::type::template return_type<self>::type
  write_string(const std::string& str, Args&& ...args)
  {
    return this->write( std::make_unique<data_type>(str.begin(), str.end()), std::forward<Args>(args)... );
  }

private:

};

}}}
