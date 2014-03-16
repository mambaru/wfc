#pragma once

#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_options_json.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{  
  
struct server_options_json
{
  JSON_NAME(threads)
  
  typedef ::wfc::json::object<
    server_options,
    fas::type_list_n<
      ::wfc::json::member< n_threads, server_options, int, &server_options::threads>, 
      ::wfc::json::base< acceptor_options_json::type >
    >::type
  > type;
};

}}}}}