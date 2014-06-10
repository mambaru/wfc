#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_request.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_request_json.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  

template< size_t ReserveSize = 80 >
struct request_serializer
{
  typedef fas::metalist::advice metatype;
  typedef _request_serializer_ tag;
  typedef request_serializer<ReserveSize> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  template<typename T, typename ParamsJson>
  static inline auto 
  serialize(
    const char* name, 
    std::unique_ptr<typename ParamsJson::target> req, 
    typename T::call_id_t id
  ) -> typename T::data_ptr
  {
    typedef typename ParamsJson::target request_type;
    outgoing_request<request_type> request;
    request.params = std::move(req);
    request.method = name; 
    request.id = id;
    auto d = std::make_unique< typename T::data_type>();
    d->reserve(ReserveSize);
    typedef outgoing_request_json<ParamsJson> send_json;
    typename send_json::serializer()(request, std::inserter( *d, d->end() ));
    return std::move(d);
  }
};

struct request_serializer_proxy
{
  typedef fas::metalist::advice metatype;
  typedef _request_serializer_ tag;
  typedef request_serializer_proxy advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  template<typename T, typename ParamsJson>
  static inline auto 
  serialize(
    const char* name, 
    std::unique_ptr<typename ParamsJson::target> req, 
    typename T::call_id_t id
  ) -> typename T::data_ptr
  {
    return T::aspect::template advice_cast<_request_serializer_>::type
            ::template serialize<T, ParamsJson>( name, std::move(req), id );

  }
};


}} // wfc


