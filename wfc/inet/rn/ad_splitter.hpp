#pragma once

#include <wfc/inet/adv/ad_splitter.hpp>
#include <wfc/inet/adv/ad_tailor.hpp>
#include <wfc/inet/adv/ad_trimmer.hpp>
#include <wfc/inet/rn/rn.hpp>
#include <wfc/inet/rn/tags.hpp>

namespace wfc{ namespace inet{ namespace rn{


struct ad_trimmer:
  ::wfc::inet::ad_trimmer<_incoming_, fas::int_<2> > {};
  
  
struct ad_splitter:
  ::wfc::inet::ad_splitter<_trimmer_, rn> {};

struct ad_tailor:
  ::wfc::inet::ad_tailor<_output_, rn> {};

}}}
