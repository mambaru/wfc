#pragma once

#include <wfc/io/ip/udp/rn/connection_options.hpp>
#include <wfc/io/rn/reader/basic_options_json.hpp>
#include <wfc/io/writer/basic_options_json.hpp>
#include <wfc/io/reader/basic_options_json.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{  
  
struct connection_options_json
{
  JSON_NAME(keep_alive)
  
  typedef ::wfc::json::object<
    connection_options,
    fas::type_list_n<
      ::wfc::json::base< ::wfc::io::reader::basic_options_json >,
      ::wfc::json::base< ::wfc::io::rn::reader::basic_options_json >,
      ::wfc::json::base< ::wfc::io::writer::basic_options_json >,
      ::wfc::json::member< n_keep_alive, connection_options, bool, &connection_options::keep_alive>
    >::type
  > type;
  
  typedef type::target      target;
  typedef type::serializer  serializer;
  typedef type::member_list member_list;

};

}}}}}