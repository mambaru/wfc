#pragma once

#include <wfc/io/ip/tcp/rn/connection_options.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{  
  
struct connection_options_json
{
  JSON_NAME(keep_alive)
  
  typedef ::wfc::json::object<
    connection_options,
    fas::type_list_n<
      ::wfc::json::member< n_keep_alive, connection_options, bool, &connection_options::keep_alive>
    >::type
  > type;
  
  typedef type::target     target;
  typedef type::serializer serializer;

};

}}}}}