#pragma once

#include <fas/aop.hpp>
#include <wfc/io/posix/rn/reader_aspect.hpp>
#include <wfc/io/reader/reader.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{ 

template<typename A = fas::aspect<> >
class reader_base:
  public wfc::io::reader::reader< typename fas::merge_aspect<A, reader_aspect>::type >
{
  typedef wfc::io::reader::reader< typename fas::merge_aspect<A, reader_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  
  reader_base(descriptor_type&& desc, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( std::move(desc), conf/*, handler */)
  {
  }
};
  
}}}}
