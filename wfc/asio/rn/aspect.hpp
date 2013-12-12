#pragma once

#include <wfc/asio/rn/ad_splitter.hpp>
#include <wfc/asio/rn/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace rn{

struct aspect: fas::aspect< fas::type_list_n<
  fas::advice<_splitter_, ad_splitter >,
  fas::advice<_tailor_,   ad_tailor >,
  fas::advice<_trimmer_,  ad_trimmer >,
  
  fas::alias<_input_,     _splitter_>,
  fas::alias<_outgoing_,  _tailor_>
  
  // fas::advice<_outgoing_,    ad_tailor>
  // fas::stub<_incoming_>
  // fas::stub<_output_>

  
  /*,
  fas::advice<_outgoing_, ad_writer>,
  fas::alias<_read_, _incoming_>,
  fas::stub<_incoming_>
  */
>::type >
{};

}}}
