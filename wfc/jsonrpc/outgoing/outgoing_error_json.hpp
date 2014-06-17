#pragma once

#include <wfc/jsonrpc/outgoing/outgoing_error.hpp>
#include <wfc/jsonrpc/outgoing/version_member.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_error_json
{
  typedef ::wfc::io::data_type data_type;
  typedef ::wfc::io::data_ptr  data_ptr;
  typedef typename T::target target;
  typedef outgoing_error<target> error_type;
  typedef typename error_type::version_type version_type;
  typedef ::wfc::json::pointer<std::unique_ptr<target>, T> error_json;
  typedef ::wfc::json::pointer<
              std::unique_ptr<data_type>, 
              ::wfc::json::raw_value< data_type> 
          > id_json;
  
  JSON_NAME(error)
  JSON_NAME(id)
  
  typedef ::wfc::json::object<
    error_type,
    typename fas::type_list_n<
      version_member::type,
      ::wfc::json::member<n_error, error_type, std::unique_ptr<target>, &error_type::error, error_json >,
      ::wfc::json::member<n_id,    error_type, data_ptr, &error_type::id,  id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}

