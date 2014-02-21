#pragma once

#include <wfc/io/basic/tags.hpp>

#include <wfc/io/basic/ad_create.hpp>
#include <wfc/io/basic/ad_post.hpp>
#include <wfc/io/basic/ad_wrap.hpp>
#include <wfc/io/basic/ad_dispatch.hpp>

#include <wfc/io/basic/types.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/callback/callback_owner.hpp>
#include <fas/aop.hpp>

#include <boost/asio/strand.hpp>
#include <boost/asio/io_service.hpp>

namespace wfc{ namespace io{ namespace basic{

typedef fas::type_list_n<
  
  fas::type< wfc::io::_io_service_type_, boost::asio::io_service>, 
  fas::type< _strand_type_, boost::asio::strand>,
  fas::type< _owner_type_, wfc::callback_owner>,
  fas::type< wfc::io::_data_type_, data_type>,

  fas::advice< _create_, ad_create>,
  fas::advice< wfc::io::_post_, ad_post>,
  fas::advice< wfc::io::_wrap_, ad_wrap>,
  fas::advice< wfc::io::_dispatch_, ad_dispatch>, 

  fas::value< _strand_, std::shared_ptr<boost::asio::strand> >, 
  fas::value< _owner_, std::shared_ptr<wfc::callback_owner> >, 
  fas::value< _not_alive_, std::function<void()> >, 
  
  fas::group< wfc::io::_create_, _create_>
  
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}
