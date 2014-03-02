#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>
#include <functional>
#include <boost/asio.hpp>
namespace wfc{ namespace io{ 
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class descriptor_holder
  : public io_base<A, AspectClass>
{
public:
  
  typedef descriptor_holder<A, AspectClass> self;
  typedef io_base<A, AspectClass> super;
  typedef typename super::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;
  typedef typename super::options_type options_type;
  
  descriptor_holder(descriptor_type desc, const options_type& conf)
    : super( desc.get_io_service(), conf)
    , _descriptor( std::move(desc) )
  {
  }
  
  descriptor_type& descriptor()
  {
    return _descriptor;
  }

  const descriptor_type& descriptor() const
  {
    return _descriptor;
  }
  
private:
  
  descriptor_type _descriptor;

};

}}
