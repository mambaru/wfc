#pragma once


#include <wfc/io/rn/reader/aspect/tags.hpp>
#include <wfc/io/rn/reader/aspect/ad_splitter.hpp>
#include <wfc/io/rn/reader/aspect/ad_trimmer.hpp>
#include <wfc/io/rn/reader/aspect/ad_ready.hpp>
#include <wfc/io/rn/reader/aspect/ad_check.hpp>

#include <wfc/io/rn/rn.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace io{ namespace rn{ namespace reader{

//template<typename TgOutgoing>
struct aspect: fas::aspect<
  //fas::alias<_outgoing_, TgOutgoing>,
  fas::alias<_input_, _splitter_>,
  fas::advice<_splitter_, ad_splitter>,
  fas::advice<_trimmer_,  ad_trimmer>,
  fas::advice<_ready_,    ad_ready>,
  fas::advice<_separator_, rn>,
  fas::value<_buffer_, basic::data_ptr>,
  fas::advice<_check_, ad_check>

>{};


}}}}
