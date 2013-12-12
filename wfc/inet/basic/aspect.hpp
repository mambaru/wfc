#pragma once

#include <wfc/inet/basic/ad_reader.hpp>
#include <wfc/inet/basic/ad_writer.hpp>
#include <wfc/inet/basic/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace basic{

//struct context {};

struct aspect: fas::aspect< fas::type_list_n<
  fas::value<_context_,   void* >,
  fas::advice<_reader_,    ad_reader>,
  fas::advice<_writer_,    ad_writer>,
  fas::alias<_outgoing_, _writer_>/*,
  fas::stub<_incoming_>*/
>::type >
{};

}}}