#pragma once

#include <wfc/jsonrpc/outgoing/outgoing_result.hpp>
#include <wfc/jsonrpc/outgoing/version_member.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace jsonrpc{


template<typename T>
struct outgoing_result_json
{
  typedef typename T::target target;
  typedef outgoing_result<target> result_type;
  typedef typename result_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> result_json;
  typedef json::pointer<wfc::io::data_ptr, json::raw_value< wfc::io::data_type> > id_json;
  
  JSON_NAME(id)
  JSON_NAME(result)
  
  typedef json::object<
    result_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_result, result_type, std::unique_ptr<target>,  &result_type::result, result_json >,
      json::member<n_id,     result_type, wfc::io::data_ptr, &result_type::id,   id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}

