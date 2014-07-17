#pragma once

#include <wfc/io/rn/writer/aspect/tags.hpp>
#include <wfc/io/rn/writer/aspect/ad_ready.hpp>
#include <wfc/io/rn/writer/aspect/ad_tailor.hpp>
#include <wfc/io/rn/rn.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace writer{

  
typedef fas::type_list_n<
  fas::advice<_tailor_, ad_tailor>,
  fas::advice<_ready_, ad_ready>,
  fas::alias<_incoming_, _tailor_>,
  fas::advice<_separator_, rn>
>::type advice_list;
  
struct aspect: fas::aspect< 
  advice_list
>
{};

/*
struct options
  : basic_options
  , ::wfc::io::basic::options
{
};
*/


template<typename TgOutgoing>
struct aspect2: fas::aspect< 
  advice_list,
  fas::type< ::wfc::io::_options_type_, basic_options >,
  fas::alias<_outgoing_, TgOutgoing>
>
{};

}}}}
