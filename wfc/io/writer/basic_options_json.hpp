#pragma once

#include <wfc/io/reader/basic_options.hpp>
#include <wfc/json/json.hpp>


namespace wfc{ namespace io{ namespace writer{
  
struct basic_options_json
{
  JSON_NAME(output_buffer_size)
  
  typedef ::wfc::json::object<
    basic_options,
    fas::type_list_n<
      ::wfc::json::member< n_output_buffer_size, basic_options, size_t, &basic_options::output_buffer_size>
    >::type
  > type;
  
  typedef type::member_list     member_list;
  typedef type::target     target;
  typedef type::serializer serializer;

};

}}}