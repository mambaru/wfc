#pragma once

#include <wfc/jsonrpc/outgoing/outgoing_error.hpp>
#include <wfc/jsonrpc/outgoing/version_member.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_error_json
{
  typedef typename T::target target;
  typedef outgoing_error<target> error_type;
  typedef typename error_type::version_type version_type;
  typedef json::pointer<std::unique_ptr<target>, T> error_json;
  typedef json::pointer<std::unique_ptr< ::wfc::io::data_type>, json::raw_value< ::wfc::io::data_type> > id_json;
  
  JSON_NAME(error)
  JSON_NAME(id)
  
  typedef json::object<
    error_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_error, error_type, std::unique_ptr<target>,    &error_type::error, error_json >,
      json::member<n_id,    error_type, ::wfc::io::data_ptr, &error_type::id,    id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}

