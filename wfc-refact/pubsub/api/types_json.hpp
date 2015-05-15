#pragma once

#include <wfc/pubsub/api/types.hpp>
#include <wfc/json/json.hpp>

namespace wfc{ namespace pubsub{

typedef std::string identity_t;
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;
typedef size_t cursor_t;
typedef std::string key_t;

struct types_json
{
  typedef ::wfc::json::value< std::string > string_type;
  typedef string_type identity_type;
  typedef string_type key_type;
  typedef ::wfc::json::value< size_t > cursor_type;
  typedef json::raw_value< pubsub::data_type > data_type;
  typedef ::wfc::json::pointer< pubsub::data_ptr, data_type> data_ptr;
};

}}
