#pragma once

//#include <wfc/jsonrpc/worker_options.hpp>
#include <vector>
#include <string>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace jsonrpc{

  //wfc::json::member<n_size, generate, size_t, &generate::size>
struct options_json
{
  JSON_NAME(services)
  JSON_NAME(queues)
  JSON_NAME(count)
  JSON_NAME(methods)
  JSON_NAME(threads)
  
  typedef ::wfc::json::object<
    options::service::queue,
    fas::type_list_n<
      ::wfc::json::member< n_count, options::service::queue, int, &options::service::queue::count>,
      ::wfc::json::member< n_methods, options::service::queue, std::vector<std::string>, &options::service::queue::methods,
      ::wfc::json::array< std::vector< ::wfc::json::value<std::string> > >
      >
    >::type
  > queue_json;
  
  typedef ::wfc::json::array< std::vector<queue_json> > vect_queue_json;

  
  typedef ::wfc::json::object<
    options::service,
    fas::type_list_n<
      ::wfc::json::member< n_threads, options::service, int, &options::service::threads>,
      ::wfc::json::member< n_queues, options::service, std::vector<options::service::queue>, &options::service::queues, vect_queue_json>
    >::type
  > service_json;
  
  typedef ::wfc::json::array< std::vector<service_json> > vect_service_json;
  
  typedef ::wfc::json::object<
    options,
    fas::type_list_n<
      ::wfc::json::member< n_services, options, std::vector<options::service> , &options::services, vect_service_json >
    >::type
  > type;
};
  /*
struct options
  : worker_options
{
  struct service
  {
    struct queue
    {
      int count = 1;
      std::vector<std::string> methods = {"*"};
    };
    
    int threads = 0;
    std::vector<queue> queues = {queue()};
  };
  std::vector< service> services = { service() };
};
*/

}}