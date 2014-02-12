#pragma once
#include <wfc/inet/conn/stream/tcp/aspect.hpp>
#include <wfc/inet/conn/rn/aspect.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace rn{ namespace stream{ namespace tcp{

struct aspect: fas::aspect<
  fas::alias< wfc::inet::conn::_incoming_, wfc::inet::conn::rn::_input_>,
  fas::alias< wfc::inet::conn::rn::_output_, wfc::inet::conn::stream::_outgoing_>,
  //echo: fas::alias< wfc::inet::conn::rn::_incoming_, wfc::inet::conn::rn::_outgoing_>
  wfc::inet::conn::rn::aspect,
  wfc::inet::conn::stream::tcp::aspect
  /*
  fas::advice<_splitter_, ad_splitter<_trimmer_, rn> >,
  fas::advice<_trimmer_,  ad_trimmer<_incoming_, fas::int_<2> > >,
  fas::advice<_tailor_,   ad_tailor<_output_, rn> >,
  fas::alias<_input_,     _splitter_>,
  fas::alias<_outgoing_,  _tailor_>,
  fas::alias<_write_,     _outgoing_>
  */
>
{};

}}}}}}
