#pragma once

#include <wfc/io/reader/read/sync/tags.hpp>
#include <wfc/io/reader/read/async/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace reader{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class reader
  : public io_base<A, AspectClass>
  , public std::enable_shared_from_this< reader<A, AspectClass> >
{
public:
  
  typedef reader<A, AspectClass> self;
  typedef io_base<A, AspectClass> super;
  
  typedef typename super::aspect::template advice_cast<_data_type_>::type data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  reader(const reader& ) = delete;
  void operator = (const reader& conf) = delete;

  template<typename Conf>
  reader(const Conf& conf) 
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

  template<typename Callback>
  void async_read(Callback callback = nullptr/*std::function<void(data_ptr)>*/)
  {
    this->get_aspect().template get<read::async::_read_>()(*this, callback);
  }
  
  data_ptr read()
  {
    return this->get_aspect().template get< read::sync::_read_ >()(*this);
    
  }

private:

};

}}}
