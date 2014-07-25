#pragma once

#include <wfc/io/ip/udp/rn/acceptor_options.hpp>
#include <wfc/io/ip/udp/rn/connection_options.hpp>
#include <wfc/io/ip/udp/rn/connection_options_json.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{  
  
struct acceptor_options_json
{
  JSON_NAME(host)
  JSON_NAME(port)
  JSON_NAME(connection)
  JSON_NAME(backlog)
  
  typedef ::wfc::json::object<
    acceptor_options,
    fas::type_list_n<
      ::wfc::json::member< n_backlog,    acceptor_options, size_t,      &acceptor_options::backlog>, 
      ::wfc::json::member< n_host,       acceptor_options, std::string, &acceptor_options::host>, 
      ::wfc::json::member< n_port,       acceptor_options, std::string, &acceptor_options::port>, 
      ::wfc::json::member< n_connection, acceptor_options, connection_options, &acceptor_options::connection, connection_options_json>
    >::type
  > type;
  
  typedef type::member_list member_list;
  typedef type::target      target;
  typedef type::serializer  serializer;
};

}}}}}