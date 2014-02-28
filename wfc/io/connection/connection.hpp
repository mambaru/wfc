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
  typedef std::unique_ptr<data_type> data_ptr;
  typedef typename super::options_type options_type;
  std::function<void(data_ptr)> sender_function;
public:
  
  connection(const connection& ) = delete;
  void operator = (const connection& conf) = delete;

  connection(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc) )
  {
    super::create(*this, conf);
  }

  /*
  connection(descriptor_type&& desc)
    : super( std::move(desc) )
  {
  }

  template<typename Conf>
  void configure(const Conf& conf)
  {
    super::create(*this, conf);
  }

  template<typename Conf>
  void reconfigure(const Conf& conf)
  {
    super::create(*this, conf);
  }
  */
  
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
  sender_function make_sender()
  {
    // Без dispatch
    // TODO: перенести в аспект
    // TODO: перенести в io_base, как заглушку на _outgoing_
    // это как промежуто
    return [this](data_ptr d)
    {
      this->send( std::move(d) );
    };
  }
  */

  /*
  template<typename ...Args>
  typename super::aspect::template advice_cast<_send_>::type::template return_type<self>::type
  send(Args&& ...args)
  {
    // Закидывает в начало цепочки writer'а
    return this->get_aspect().template get< _send_ >()(*this, std::forward<Args>(args)...);
  }

  template<typename ...Args>
  typename super::aspect::template advice_cast<_send_>::type::template return_type<self>::type
  send_string(const std::string& str, Args&& ...args)
  {
    return this->send( std::make_unique<data_type>(str.begin(), str.end()), std::forward<Args>(args)... );
  }
  */

private:

};

}}}
