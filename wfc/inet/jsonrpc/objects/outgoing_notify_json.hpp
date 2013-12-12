#pragma once

#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>
#include <wfc/inet/jsonrpc/objects/outgoing_response.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{


template<typename T>
struct outgoing_notify_json
{
  typedef typename T::target target;
  typedef outgoing_notify<target> notify_type;
  typedef typename notify_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> params_json;
  
  NAME(method);
  NAME(params);
  
  typedef json::object<
    notify_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_method, notify_type, std::string, &notify_type::method>,
      json::member<n_params, notify_type, std::unique_ptr<target>, &notify_type::params, params_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}}
