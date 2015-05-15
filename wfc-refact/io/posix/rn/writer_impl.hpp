#pragma once

#include <wfc/io/posix/rn/writer_base.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

class writer_impl
  : public writer_base<>
{
  typedef writer_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  writer_impl(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf )
  {
  }
};
  
}}}}
