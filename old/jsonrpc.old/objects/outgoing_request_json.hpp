#pragma once

#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>
#include <wfc/jsonrpc/objects/outgoing_request.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_request_json
{
  typedef typename T::target target;
  typedef outgoing_request<target> request_type;
  typedef typename request_type::version_type version_type;
  typedef json::pointer<std::unique_ptr<target>, T> params_json;
  
  FAS_NAME(method)
  FAS_NAME(params)
  FAS_NAME(id)

  typedef json::object<
    request_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_method, request_type, std::string, &request_type::method>,
      json::member<n_params, request_type, std::unique_ptr<target>, &request_type::params, params_json >,
      json::member<n_id, request_type, int, &request_type::id>
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}
