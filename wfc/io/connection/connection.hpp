#pragma once

#include <wfc/io/connection/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace io{ namespace connection{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class connection
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< connection<A, AspectClass> >
{
public:
  
  typedef connection<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::options_type options_type;
  
public:

  connection(const connection& ) = delete;
  void operator = (const connection& conf) = delete;

  
  connection(descriptor_type&& desc, const options_type& opt, wfc::io::handler handler = nullptr)
    : super( std::move(desc), opt, handler)
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
  
  bool status() const
  {
    return this->get_aspect().template get<_status_>();
  }
  
  /*
  data_ptr read()
  {
    return this->get_aspect().template get< _read_ >()(*this);
  }

  void async_read(wfc::io::callback handler)
  {
    this->get_aspect().template get< _async_read_ >()(*this, handler);
  }
  */
private:

};

  /*
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class connection
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< connection<A, AspectClass> >
{
public:
  
  typedef connection<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef typename super::options_type options_type;
  std::function<void(data_ptr)> sender_function;
public:
  
  connection(const connection& ) = delete;
  void operator = (const connection& conf) = delete;

  connection(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
    super::create(*this);
    
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    super::descriptor().io_control(non_blocking_io);

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
  
  
};*/

}}}
