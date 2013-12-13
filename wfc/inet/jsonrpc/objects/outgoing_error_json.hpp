#pragma once

#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>
#include <wfc/inet/jsonrpc/objects/outgoing_error.hpp>
#include <wfc/inet/jsonrpc/objects/outgoing_json.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{

template<typename T>
struct outgoing_error_json
{
  typedef typename T::target target;
  typedef outgoing_error<target> error_type;
  typedef typename error_type::version_type version_type;
  typedef json::pointer<std::unique_ptr<target>, T> error_json;
  typedef json::pointer<std::unique_ptr<int>, json::value<int> > id_json;
  
  NAME(error)
  NAME(id)
  
  typedef json::object<
    error_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_error, error_type, std::unique_ptr<target>,  &error_type::error,  error_json >,
      json::member<n_id,     error_type, std::unique_ptr<int>,    &error_type::id,     id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}}


