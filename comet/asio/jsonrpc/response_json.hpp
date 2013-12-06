#pragma once

#include <comet/json/name.hpp>
#include <comet/json/json.hpp>
#include <comet/asio/jsonrpc/response.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{


struct version_member
{
  NAME(jsonrpc);
  typedef json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};

  
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

template<typename T>
struct outgoing_notify_json
{
  typedef typename T::target target;
  typedef outgoing_notify<target> notify_type;
  typedef typename notify_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> params_json;
  
  NAME(params);
  NAME(jsonrpc);
  
  typedef json::object<
    notify_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_params, notify_type, std::unique_ptr<target>, &notify_type::params, params_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

template<typename T>
struct outgoing_request_json
{
  typedef typename T::target target;
  typedef outgoing_request<target> request_type;
  typedef typename request_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> params_json;
  
  NAME(id);
  NAME(method);
  NAME(params);
  NAME(jsonrpc);
  
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

}}}}


