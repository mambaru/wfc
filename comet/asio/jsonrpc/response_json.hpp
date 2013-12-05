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

template<typename T>
struct notify_json
{
  typedef typename T::target target;
  typedef notify<target> notify_type;
  typedef typename notify_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> params_json;
  
  NAME(params);
  NAME(jsonrpc);
  
  typedef json::object<
    notify_type,
    typename fas::type_list_n<
      json::member<n_jsonrpc, notify_type, version_type, &notify_type::version>,
      json::member<n_params, notify_type, std::unique_ptr<target>, &notify_type::result, params_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

template<typename T>
struct request_json
{
  typedef typename T::target target;
  typedef request<target> request_type;
  typedef typename request_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> params_json;
  
  NAME(id);
  NAME(method);
  NAME(params);
  NAME(jsonrpc);
  
  typedef json::object<
    request_type,
    typename fas::type_list_n<
      json::member<n_jsonrpc, request_type, version_type, &request_type::version>,
      json::member<n_method, request_type, std::string, &request_type::method>,
      json::member<n_params, request_type, std::unique_ptr<target>, &request_type::params, params_json >,
      json::member<n_id, request_type, int, &request_type::id>
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}}}


