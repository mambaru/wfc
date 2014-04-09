#pragma once

#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/io/ip/tcp/rn/connection_options_json.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 
  
struct client_options_json
{
  JSON_NAME(host)
  JSON_NAME(port)
  JSON_NAME(connection)
  JSON_NAME(threads)
  
  typedef ::wfc::json::object<
    client_options,
    fas::type_list_n<
      ::wfc::json::member< n_threads, client_options, int, &client_options::threads>, 
      ::wfc::json::member< n_host, client_options, std::string, &client_options::host>, 
      ::wfc::json::member< n_port, client_options, std::string, &client_options::port>, 
      ::wfc::json::member< n_connection, client_options, connection_options, &client_options::connection, connection_options_json::type>
    >::type
  > type;
  
  typedef type::target     target;
  typedef type::serializer serializer;
};

}}}}}