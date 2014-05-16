#pragma once


#include <wfc/io/basic_io.hpp>
#include <wfc/io/basic/aspect.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ 

class strand
 : public basic_io< fas::aspect< fas::type< _options_type_, basic::options > > >
{
  typedef basic_io< fas::aspect< fas::type< _options_type_, basic::options > > > super;
public:
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  strand(io_service_type& io_service, const options_type& opt =  options_type() )
    : super( io_service, opt)
  {
    super::create(*this);
  }
};
  
  /*
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class basic_io
  : public io_base< typename fas::merge_aspect<A, basic::aspect>::type, AspectClass> 
{
public:
  typedef basic_io<A, AspectClass> self;
  typedef io_base< typename fas::merge_aspect<A, basic::aspect>::type, AspectClass> super;
  
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  basic_io(io_service_type& io_service, const options_type& opt =  options_type() )
    : super( io_service, opt)
  {
  }
};
*/


}}
