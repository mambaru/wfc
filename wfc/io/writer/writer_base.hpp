#pragma once

//#include <wfc/io/writer/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace wfc{ namespace io{ namespace writer{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class writer_base
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< writer_base<A, AspectClass> >
{
public:
  
  typedef writer_base<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::options_type options_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  writer_base(const writer_base& ) = delete;
  void operator = (const writer_base& conf) = delete;

  writer_base(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
    super::create(*this);
  }
  
  void start()
  {
    super::start(*this);
  }
  
  void stop(std::function<void()> finalize)
  {
    super::stop(*this, finalize);
  }
  
  bool status() const
  {
    return this->get_aspect().template get<_status_>();
  }
  
  void write(data_ptr d)
  {
    return this->get_aspect().template get< _write_ >()(*this, std::move(d));
  }

  void async_write(data_ptr d)
  {
    this->get_aspect().template get< _async_write_ >()(*this, std::move(d));
  }
private:
};

}}}
