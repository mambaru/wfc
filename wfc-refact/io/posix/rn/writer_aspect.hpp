#pragma once

#include <wfc/io/posix/rn/writer_options.hpp>
#include <wfc/io/writer/writer.hpp>
#include <wfc/io/rn/writer/aspect.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

 
struct writer_aspect: 
  fas::aspect<
    //fas::stub< wfc::io::_stop_>, // tmp
    fas::type< wfc::io::_options_type_, writer_options >,
    wfc::io::rn::writer::aspect2<wfc::io::writer::_incoming_>,
    wfc::io::writer::stream< boost::asio::posix::stream_descriptor, wfc::io::rn::writer::_incoming_>
    /*,
    wfc::io::rn::writer::writer_aspect<wfc::io::writer::_incoming_>,
    wfc::io::writer::stream< boost::asio::posix::stream_descriptor, wfc::io::rn::writer::_incoming_>,
    wfc::io::writer::error_log
    */
  >
{
};
  
}}}}