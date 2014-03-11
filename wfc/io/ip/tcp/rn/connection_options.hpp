#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/types.hpp>
#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/rn/writer/aspect.hpp>



namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{
  
struct connection_options
  : wfc::io::basic::options
  , wfc::io::rn::reader::basic_options
  , wfc::io::rn::writer::basic_options
{
  bool keep_alive = false;
  /*
  size_t acceptors = 1;
  size_t threads = 0;
  
  size_t output_buffer_size = 1024;
  size_t input_buffer_size = 1024;
  std::function<void()> not_alive = nullptr;
  std::function<void()> release_function = nullptr;

  wfc::io::handler handler = nullptr;
  */
};
  
}}}}}
