#pragma once

#include <wfc/io/posix/rn/reader_base.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

class reader_impl
  : public reader_base<>
{
  typedef reader_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  reader_impl(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr)
    : super( std::move(desc), conf, handler )
  {
  }
};
  
}}}}
