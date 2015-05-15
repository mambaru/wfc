#pragma once

#include <wfc/io/writer/aspect/aspect.hpp>
#include <wfc/io/writer/writer_base.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace wfc{ namespace io{ namespace writer{
/*
struct options
  :  ::wfc::io::basic::options
  , basic_options
{
  
};


struct writer_aspect: fas::aspect
<
  fas::stub<_write_>,
  fas::stub<_async_write_>,
  fas::stub<_options_type_, basic_options>
>
{};
  

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
using writer = writer_base< typename fas::merge_aspect< A, writer_aspect>::type, AspectClass>;
*/
}}}
