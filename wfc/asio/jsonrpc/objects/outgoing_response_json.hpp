#pragma once

#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>
#include <wfc/asio/jsonrpc/objects/outgoing_response.hpp>
#include <wfc/asio/jsonrpc/objects/outgoing_json.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{

  
template<typename T>
struct outgoing_response_json
{
  typedef typename T::target target;
  typedef outgoing_response<target> response_type;
  typedef typename response_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> result_json;
  
  NAME(id);
  NAME(result);
  
  typedef json::object<
    response_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_result, response_type, std::unique_ptr<target>, &response_type::result, result_json >,
      json::member<n_id, response_type, int, &response_type::id>
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}}


