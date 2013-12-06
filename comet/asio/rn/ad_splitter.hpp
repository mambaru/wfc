#pragma once

#include <comet/asio/adv/ad_splitter.hpp>
#include <comet/asio/adv/ad_tailor.hpp>
#include <comet/asio/adv/ad_trimmer.hpp>
#include <comet/asio/rn/rn.hpp>
#include <comet/asio/rn/tags.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace rn{


struct ad_trimmer:
  ::mamba::comet::inet::ad_trimmer<_incoming_, fas::int_<2> > {};
  
  
struct ad_splitter:
  ::mamba::comet::inet::ad_splitter<_trimmer_, rn> {};

struct ad_tailor:
  ::mamba::comet::inet::ad_tailor<_output_, rn> {};

}}}}
