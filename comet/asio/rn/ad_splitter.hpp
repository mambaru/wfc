#pragma once

#include <comet/asio/adv/ad_splitter.hpp>
#include <comet/asio/rn/rn.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace rn{

struct ad_splitter:
  ::mamba::comet::inet::ad_splitter<rn> {};

}}}}
