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
  
  ~connection() {}

  connection(const connection& ) = delete;
  void operator = (const connection& conf) = delete;

  
  connection(descriptor_type&& desc, const options_type& opt)
    : super( std::move(desc), opt)
  {
    super::create(*this);
  }

  void start()
  {
    typename super::lock_guard lk(super::mutex());
    super::start(*this);
  }
  
  void stop(std::function<void()> finalize)
  {
    typename super::lock_guard lk(super::mutex());
    super::stop(*this, finalize);
  }
  
  bool status() const
  {
    return this->get_aspect().template get<_status_>();
  }
  
private:

};


}}}
