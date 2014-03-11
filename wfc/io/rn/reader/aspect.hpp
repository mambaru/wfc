#pragma once

#include <wfc/io/rn/reader/tags.hpp>
#include <wfc/io/rn/reader/ad_splitter.hpp>
#include <wfc/io/rn/reader/ad_trimmer.hpp>
#include <wfc/io/rn/reader/ad_ready.hpp>
#include <wfc/io/rn/reader/check/ad_check.hpp>
#include <wfc/io/reader/aspect.hpp>
#include <wfc/io/rn/rn.hpp>
#include <wfc/io/basic/types.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

typedef fas::type_list_n<

  fas::alias<_incoming_, _splitter_>,
  
  fas::advice<_splitter_, ad_splitter>,
  fas::advice<_trimmer_,  ad_trimmer>,
  fas::advice<_ready_,    ad_ready>,
  fas::advice<_separator_, rn>,
  
  fas::value<_buffer_, basic::data_ptr>
  
>::type advice_list;
  
struct aspect: fas::aspect< 
  fas::stub<_check_>,
  advice_list
>
{};

struct basic_options
  : wfc::io::reader::basic_options
{
  
  size_t rn_limit_error = static_cast<size_t>(-1);
  size_t rn_limit_warning= static_cast<size_t>(-1);
  
};

struct options
  : basic_options
  , wfc::io::basic::options
{
  //std::function<callback_status()> not_alive = nullptr;
};


template<typename TgOutgoing>
struct aspect2: fas::aspect<
  fas::type< wfc::io::_options_type_, options >,
  fas::alias<_outgoing_, TgOutgoing>,
  fas::alias<_incoming_, _splitter_>,
  fas::advice<_splitter_, ad_splitter>,
  fas::advice<_trimmer_,  ad_trimmer>,
  fas::advice<_ready_,    ad_ready>,
  fas::advice<_separator_, rn>,
  fas::value<_buffer_, basic::data_ptr>,
  fas::advice<_check_, check::ad_check>

>{};


}}}}
