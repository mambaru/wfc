#pragma once

#include <wfc/io/rn/reader/basic_options.hpp>
//#include <wfc/io/reader/basic_options_json.hpp>
#include <wfc/json/json.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{
  
struct basic_options_json
{
  JSON_NAME(rn_limit_error)
  JSON_NAME(rn_limit_warning)

  
  typedef ::wfc::json::object<
    basic_options,
    fas::type_list_n<
      //::wfc::json::base< ::wfc::io::reader::basic_options_json >,
      ::wfc::json::member< n_rn_limit_error,   basic_options, size_t, &basic_options::rn_limit_error>,
      ::wfc::json::member< n_rn_limit_warning, basic_options, size_t, &basic_options::rn_limit_warning>
    >::type
  > type;
  
  typedef type::member_list     member_list;
  typedef type::target     target;
  typedef type::serializer serializer;

};

}}}}