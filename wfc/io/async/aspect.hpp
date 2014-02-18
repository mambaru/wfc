#pragma once

#include <wfc/io/base/tags.hpp>
#include <wfc/io/base/ad_initialize.hpp>
#include <wfc/io/base/init.hpp>
#include <wfc/io/base/config.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace base{

typedef fas::type_list_n<
  //fas::type<_data_type_, std::vector<char> >,
  fas::advice<_initialize_, ad_initialize>,
  fas::group< wfc::io::_initialize_, _initialize_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  //fas::type<_init_type_, init>,
  fas::type<_config_type_, config>
>
{};

}}}
