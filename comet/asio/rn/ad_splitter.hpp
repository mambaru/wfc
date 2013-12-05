#pragma once

#include <comet/asio/adv/ad_splitter.hpp>
#include <comet/asio/rn/rn.hpp>
#include <comet/asio/rn/tags.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace rn{

struct ad_splitter:
  ::mamba::comet::inet::ad_splitter<_incoming_, rn> {};

}}}}
