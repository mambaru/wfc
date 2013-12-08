#pragma once

#include <comet/asio/jsonrpc/method/method_implementation.hpp>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

template<typename Tg, typename M>
struct method:
  fas::type_list_n<
    fas::advice< Tg, method_implementation<M> >,
    fas::group<_method_, Tg>
  >::type
{};

}}}}


