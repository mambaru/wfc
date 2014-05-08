#pragma once

#include <wfc/inet/conn/rn/ad_splitter.hpp>
#include <wfc/inet/conn/rn/ad_tailor.hpp>
#include <wfc/inet/conn/rn/ad_trimmer.hpp>

#include <wfc/inet/conn/rn/rn.hpp>
#include <wfc/inet/conn/rn/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace rn{

    /*

struct ad_trimmer:
  ::wfc::inet::ad_trimmer<_incoming_, fas::int_<2> > {};
  
  
struct ad_splitter:
  ::wfc::inet::ad_splitter<_trimmer_, rn> {};

struct ad_tailor:
  ::wfc::inet::ad_tailor<_output_, rn> {};
  */

/*
 * _output_ required
 */  
struct aspect: fas::aspect< fas::type_list_n<
  fas::advice<_splitter_, ad_splitter<_trimmer_, rn> >,
  fas::advice<_trimmer_,  ad_trimmer<_incoming_, fas::int_<2> > >,
  fas::advice<_tailor_,   ad_tailor<_output_, rn> >,
  fas::alias<_input_,     _splitter_>,
  fas::alias<_outgoing_,  _tailor_>,
  fas::alias<_write_,     _outgoing_>
>::type >
{};

}}}}
