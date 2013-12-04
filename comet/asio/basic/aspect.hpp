#pragma once

#include <comet/asio/adv/ad_reader.hpp>
#include <comet/asio/adv/ad_writer.hpp>
#include <comet/asio/basic/tags.hpp>
#include <fas/aop.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace basic{

struct aspect: fas::aspect< fas::type_list_n<
  fas::advice<_input_,    ad_reader>,
  fas::advice<_outgoing_, ad_writer>,
  fas::alias<_read_, _incoming_>,
  fas::stub<_incoming_>
>::type >
{};

}}}}
