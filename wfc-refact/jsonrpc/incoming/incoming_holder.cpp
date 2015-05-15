#include <wfc/jsonrpc/incoming/incoming_holder.hpp>

namespace wfc{ namespace jsonrpc{

incoming_holder::incoming_holder(data_ptr d )
  : _data(std::move(d))
{
  _time_point = clock_t::now();
}

incoming_holder::data_ptr incoming_holder::parse()
{
  if ( _data == nullptr )
    return nullptr;

  _begin = ::wfc::json::parser::parse_space(_data->begin(), _data->end());
  _end = incoming_json::serializer()( _incoming, _begin, _data->end());
  
  iterator next = ::wfc::json::parser::parse_space( _end, _data->end());
  if ( next == _data->end() || *next=='\0')
    return nullptr;

  return std::make_unique<data_type>(next, _data->end());
}

bool incoming_holder::method_equal_to(const char* name)  const
{
  incoming::iterator beg = _incoming.method.first;
  incoming::iterator end = _incoming.method.second;
  if (beg++==end) return false;
  if (beg==end--) return false;
  for (; beg != end && *beg==*name; ++beg, ++name);
  return beg==end && *name=='\0';
}

std::string incoming_holder::method() const
{
  if ( std::distance(_incoming.method.first, _incoming.method.second ) > 2 )
    return std::string( _incoming.method.first+1, _incoming.method.second-1);
  else
    return std::string();
}

incoming_holder::raw_t incoming_holder::raw_method() const 
{
  return std::make_pair( _incoming.method.first, _incoming.method.second );
}

incoming_holder::raw_t incoming_holder::raw_id() const
{
  return std::make_pair( _incoming.id.first, _incoming.id.second );
}


std::string incoming_holder::error_message(const json::json_error& e) const
{
  if ( _data != nullptr   )
    return e.message( _data->begin(), _data->end() );
  return std::string();
}

std::string incoming_holder::params_error_message(const json::json_error& e) const
{
  return e.message(_incoming.params.first, _incoming.params.second);
}

std::string incoming_holder::result_error_message(const json::json_error& e) const
{
  return e.message(_incoming.result.first, _incoming.result.second);
}

std::string incoming_holder::error_error_message(const json::json_error& e) const
{
  return e.message(_incoming.error.first, _incoming.error.second);
}

std::string incoming_holder::id_error_message(const json::json_error& e) const
{
  return e.message(_incoming.id.first, _incoming.id.second);
}

const incoming& incoming_holder::get()  const 
{
  return _incoming;
}

incoming_holder::clock_t::time_point incoming_holder::time_point() const 
{
  return _time_point;
}

incoming_holder::data_ptr incoming_holder::detach()
{
  _incoming = incoming();
  return std::move(_data);
}

incoming_holder::data_ptr incoming_holder::acquire_params()
{
  std::move( _incoming.params.first, _incoming.params.second, _data->begin() );
  _data->resize( std::distance(_incoming.params.first, _incoming.params.second) );
  _incoming = incoming();
  return std::move(_data);
}

  
}}
