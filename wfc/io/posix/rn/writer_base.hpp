#pragma once

#include <fas/aop.hpp>
#include <wfc/io/posix/rn/writer_aspect.hpp>
#include <wfc/io/writer/writer.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{ 

template<typename A = fas::aspect<> >
class writer_base:
  public wfc::io::writer::writer< typename fas::merge_aspect<A, writer_aspect>::type >
{
  typedef wfc::io::writer::writer< typename fas::merge_aspect<A, writer_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  
  writer_base(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf )
  {
  }
};
  
}}}}
