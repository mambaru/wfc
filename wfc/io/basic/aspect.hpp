#pragma once

#include <wfc/io/basic/tags.hpp>

#include <wfc/io/basic/ad_initialize.hpp>
#include <wfc/io/basic/ad_create.hpp>
#include <wfc/io/basic/ad_post.hpp>
#include <wfc/io/basic/ad_wrap.hpp>
#include <wfc/io/basic/ad_dispatch.hpp>

#include <wfc/io/basic/init.hpp>
#include <wfc/io/basic/config.hpp>
#include <wfc/io/basic/types.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/callback/callback_owner.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace basic{

typedef fas::type_list_n<
  fas::advice<_initialize_, ad_initialize>,
  fas::advice<_create_, ad_create>,
  fas::group< wfc::io::_initialize_, _initialize_>,
  fas::group< wfc::io::_create_, _create_>, 
  fas::type< wfc::io::_io_service_type_, boost::asio::io_service>, 
  fas::value< wfc::io::_io_service_ptr_, std::shared_ptr<boost::asio::io_service> >,
  fas::type< _strand_type_, boost::asio::strand>,
  fas::value< _strand_, std::shared_ptr<boost::asio::strand> >, 
  fas::type< _owner_type_, wfc::callback_owner>,
  fas::value< _owner_, std::shared_ptr<wfc::callback_owner> >, 
  fas::type< wfc::io::_data_type_, data_type>,
  fas::advice< wfc::io::_post_, ad_post>,
  fas::advice< wfc::io::_wrap_, ad_wrap>,
  fas::advice< wfc::io::_dispatch_, ad_dispatch> 
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  //fas::type<_init_type_, init>,
  fas::type<_config_type_, config>
>
{};

}}}
