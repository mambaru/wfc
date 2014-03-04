#pragma once

#include <wfc/io/posix/rn/reader_options.hpp>
#include <wfc/io/reader/aspect.hpp>
#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

 
struct reader_aspect: 
  fas::aspect<
    
    fas::stub< wfc::io::_stop_>, // tmp
    
    wfc::io::rn::reader::aspect2<wfc::io::reader::_incoming_>,
    wfc::io::reader::stream< boost::asio::posix::stream_descriptor, wfc::io::rn::reader::_incoming_>,
    wfc::io::reader::error_log
  >
{
};
  
}}}}