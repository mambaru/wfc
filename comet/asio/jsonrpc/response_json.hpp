#pragma once

#include <comet/json/name.hpp>
#include <comet/json/json.hpp>
#include <comet/asio/jsonrpc/response.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

template<typename T>
struct response_json
{
  typedef typename T::target target;
  typedef response<target> response_type;
  typedef typename response_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> result_json;
  
  NAME(id);
  NAME(result);
  NAME(jsonrpc);
  
  typedef json::object<
    response_type,
    typename fas::type_list_n<
      json::member<n_jsonrpc, response_type, version_type, &response_type::version>,
      json::member<n_result, response_type, std::unique_ptr<target>, &response_type::result, result_json >,
      json::member<n_id, response_type, int, &response_type::id>
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}}}


