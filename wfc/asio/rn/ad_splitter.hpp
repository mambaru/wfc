#pragma once

#include <wfc/asio/adv/ad_splitter.hpp>
#include <wfc/asio/adv/ad_tailor.hpp>
#include <wfc/asio/adv/ad_trimmer.hpp>
#include <wfc/asio/rn/rn.hpp>
#include <wfc/asio/rn/tags.hpp>

namespace wfc{ namespace inet{ namespace rn{


struct ad_trimmer:
  ::wfc::inet::ad_trimmer<_incoming_, fas::int_<2> > {};
  
  
struct ad_splitter:
  ::wfc::inet::ad_splitter<_trimmer_, rn> {};

struct ad_tailor:
  ::wfc::inet::ad_tailor<_output_, rn> {};

}}}
