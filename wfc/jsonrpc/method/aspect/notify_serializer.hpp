#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify_json.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template< size_t ReserveSize = 80 >
struct notify_serializer
{
  typedef fas::metalist::advice metatype;
  typedef _notify_serializer_ tag;
  typedef notify_serializer<ReserveSize> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  template<typename T, typename ParamsJson>
  static inline auto 
  serialize(
    const char* name, 
    std::unique_ptr<typename ParamsJson::target> req
  ) -> typename T::data_ptr
  {
    typedef typename ParamsJson::target params_type;
    outgoing_notify<params_type> notify;
    notify.params = std::move(req);
    notify.method = name; 
    auto d = std::make_unique< typename T::data_type>();
    d->reserve(ReserveSize); 
    typedef outgoing_notify_json<ParamsJson> send_json;
    typename send_json::serializer()(notify, std::inserter( *d, d->end() ));
    return std::move(d);
  }
};

struct notify_serializer_proxy
{
  typedef fas::metalist::advice metatype;
  typedef _notify_serializer_ tag;
  typedef notify_serializer_proxy advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  template<typename T, typename ParamsJson>
  static inline auto 
  serialize(
    const char* name, 
    std::unique_ptr<typename ParamsJson::target> req
  ) -> typename T::data_ptr
  {
    return T::aspect::template advice_cast<_notify_serializer_>::type
            ::template serialize<T, ParamsJson>( name, std::move(req) );
  }
};


}} // wfc


