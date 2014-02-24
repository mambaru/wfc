#pragma once

//#include <wfc/io/reader/read/sync/tags.hpp>
//#include <wfc/io/reader/read/async_callback/tags.hpp>
#include <wfc/io/reader/tags.hpp>
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
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  reader(const reader& ) = delete;
  void operator = (const reader& conf) = delete;

  template<typename Conf>
  reader(descriptor_type&& desc, const Conf& conf)
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
  template<typename Callback>
  void read(Callback callback = nullptr)
  {
    this->get_aspect().template get<_read_>()(*this, callback);
  }
  
  data_ptr read()
  {
    return this->get_aspect().template get< _read_ >()(*this);
  }
  */
  
  template<typename ...Args>
  typename super::aspect::template advice_cast<_read_>::type::template return_type<self>::type
  read(Args&& ...args)
  {
    return this->get_aspect().template get< _read_ >()(*this, std::forward<Args>(args)...);
  }
  
  /*
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
  return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

   */
  
  /*
  template<typename T>
  typename T::aspect::template advice_cast<read::sync::_read_>::type::return_type
  */

private:

};

}}}
