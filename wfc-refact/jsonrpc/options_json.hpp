#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/json.hpp>
#include <vector>
#include <string>


namespace wfc{ namespace jsonrpc{

struct options_json
{
  JSON_NAME(workers)
  JSON_NAME(strands)
  JSON_NAME(count)
  JSON_NAME(methods)
  JSON_NAME(threads)
  
  typedef ::wfc::json::object<
    options::worker::strand,
    fas::type_list_n<
      ::wfc::json::member< n_count, options::worker::strand, int, &options::worker::strand::count>,
      ::wfc::json::member< n_methods, options::worker::strand, std::vector<std::string>, &options::worker::strand::methods,
      ::wfc::json::array< std::vector< ::wfc::json::value<std::string> > >
      >
    >::type
  > strand_json;
  
  typedef ::wfc::json::array< std::vector<strand_json> > vect_strand_json;

  
  typedef ::wfc::json::object<
    options::worker,
    fas::type_list_n<
      ::wfc::json::member< n_threads, options::worker, int, &options::worker::threads>,
      ::wfc::json::member< n_strands, options::worker, std::vector<options::worker::strand>, &options::worker::strands, vect_strand_json>
    >::type
  > worker_json;
  
  typedef ::wfc::json::array< std::vector<worker_json> > vect_worker_json;
  
  typedef ::wfc::json::object<
    options,
    fas::type_list_n<
      ::wfc::json::member< n_workers, options, std::vector<options::worker> , &options::workers, vect_worker_json >
    >::type
  > type;
  
  typedef type::serializer serializer;
  typedef type::target target;
};

}}