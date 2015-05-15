#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/types.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/rn/reader/basic_options.hpp>
#include <wfc/io/writer/basic_options.hpp>
#include <wfc/io/rn/writer/basic_options.hpp>



namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

// Вынести основной в io::connection  
struct connection_options
  : ::wfc::io::basic::options
  , ::wfc::io::reader::basic_options
  , ::wfc::io::rn::reader::basic_options
  , ::wfc::io::writer::basic_options
  , ::wfc::io::rn::writer::basic_options
{
  bool keep_alive = true;
};
  
}}}}}
