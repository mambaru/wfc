#pragma once

#include <wfc/jsonrpc/outgoing/outgoing_request.hpp>
#include <wfc/jsonrpc/outgoing/version_member.hpp>
#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_request_json
{
  typedef typename T::target target;
  typedef outgoing_request<target> request_type;
  typedef typename request_type::version_type version_type;
  typedef ::wfc::json::pointer<std::unique_ptr<target>, T> params_json;
  
  FAS_NAME(method)
  FAS_NAME(params)
  FAS_NAME(id)

  typedef ::wfc::json::object<
    request_type,
    typename fas::type_list_n<
      version_member::type,
      ::wfc::json::member<n_method, request_type, std::string, &request_type::method>,
      ::wfc::json::member<n_params, request_type, std::unique_ptr<target>, &request_type::params, params_json >,
      ::wfc::json::member<n_id, request_type, int, &request_type::id>
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}
