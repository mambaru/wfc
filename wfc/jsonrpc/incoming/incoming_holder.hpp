#pragma once

#include <wfc/jsonrpc/incoming/incoming.hpp>
#include <wfc/jsonrpc/incoming/incoming_json.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{

class incoming_holder
{
  typedef wfc::io::data_type data_type;
  typedef wfc::io::data_ptr  data_ptr;
  typedef data_type::iterator iterator;
public:
  incoming_holder(data_ptr d, std::weak_ptr< wfc::io::iio> iio, wfc::io::callback clb)
    : iio(iio)
    , handler( clb )
    , _data( std::move(d) )
  {
    _begin = wfc::json::parser::parse_space(_data->begin(), _data->end());
    _end = incoming_json::serializer()( _incoming, _begin, _data->end());
  }
  
  data_ptr detach()
  {
    _incoming = incoming();
    return std::move(_data);
  }
  
  data_ptr tail() const
  {
    if ( _data == nullptr )
      return nullptr;
    
    iterator beg = wfc::json::parser::parse_space( _end, _data->end());
    if ( beg == _data->end() )
      return nullptr;
    
    return std::make_unique<data_type>(beg, _data->end());
  }
  
  bool empty() const { return !$();}
  bool $() const{ return _data != nullptr;}
  
  bool has_method()   const { return $() && _incoming.method.first!=_incoming.method.second;}
  bool has_result() const   { return $() && _incoming.result.first!=_incoming.result.second;}
  bool has_error() const    { return $() && _incoming.error.first!=_incoming.error.second;}
  bool has_id() const       { return $() && _incoming.id.first!=_incoming.id.second;}
  bool has_params() const   { return $() && _incoming.params.first!=_incoming.params.second;}
  
  bool is_request() const     { return this->has_method() && this->has_id();   }
  bool is_response() const    { return this->has_result() && this->has_id();   }
  bool is_notify() const      { return this->has_method() && !this->has_id();  }
  bool is_error() const       { return this->has_error(); }
  bool is_other_error() const   { return this->has_error() && !this->has_id();   }
  bool is_request_error() const { return this->has_error() && this->has_id();   }
  
  bool is_valid() const 
  {
    return this->is_request()
      || this->is_response()
      || this->is_notify()
      || this->is_error();
  }
  
    
  bool method(const char* ch)
  {
    incoming::iterator beg = _incoming.method.first;
    incoming::iterator end = _incoming.method.second;
    if (beg++==end) return false;
    if (beg==end--) return false;
    for (; beg != end && *beg==*ch; ++beg, ++ch);
    return beg==end && *ch=='\0';
  }

  
  template<typename V, typename J = json::value<V> >
  V get_id() const
  {
    V id = V();
    if ( $() )
      typename J::serializer()( id, _incoming.id.first, _incoming.id.second );
    return id;
  }
  
  data_ptr raw_id() const
  {
    if ( this->has_id() )
      return std::make_unique<data_type>(_incoming.id.first, _incoming.id.second);
    else
      return nullptr;
  }
  
  template<typename J>
  std::unique_ptr<typename J::target> get_params() const
  {
    if ( !this->has_params() )
      return nullptr;
    if ( 'n'==*_incoming.params.first)
      return nullptr; // is null 
    auto result = std::make_unique<typename J::target>();
    typename J::serializer()(*result, _incoming.params.first, _incoming.params.second);
    return std::move(result);
  }
  
  template<typename J>
  std::unique_ptr<typename J::value_type> get_error() const
  {
    if ( !this->has_error() )
      return nullptr;
    auto result = std::make_unique<typename J::value_type>();
    typename J::serializer()(*result, _incoming.error.first, _incoming.error.second);
    return std::move(result);
  }
  
  std::weak_ptr<wfc::io::iio> iio;
  wfc::io::callback handler;
  
  const incoming& get()  const 
  {
    return _incoming;
  }
  
private:
  
  data_ptr _data;
  incoming _incoming;
  iterator _begin;
  iterator _end;
};
  
}}
