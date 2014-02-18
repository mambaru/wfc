#pragma once

#include <wfc/io/base/aspect.hpp>
#include <wfc/io/posix/tags.hpp>
#include <wfc/io/context.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io/posix/tags.hpp>
#include <wfc/io/posix/init.hpp>
#include <wfc/io/posix/ad_initialize.hpp>

#include <boost/asio/posix/stream_descriptor.hpp>

namespace wfc{ namespace io{ namespace posix{

typedef fas::type_list_n<
  fas::type<_descriptor_type_, boost::asio::posix::stream_descriptor>,
  fas::value<_descriptor_ptr_, std::shared_ptr<boost::asio::posix::stream_descriptor> >,
  fas::advice<_initialize_, ad_initialize>,
  fas::group< wfc::io::_initialize_, _initialize_>,
  wfc::io::base::advice_list
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::type<_init_type_, init >,
  fas::type<_config_type_, wfc::io::base::config>,
  context<>
>
{};

}}}
