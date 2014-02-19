#pragma once

#include <wfc/io/posix/ad_create.hpp>
#include <wfc/io/posix/tags.hpp>
#include <wfc/io/tags.hpp>

#include <boost/asio/posix/stream_descriptor.hpp>

#include <memory>

namespace wfc{ namespace io{ namespace posix{

typedef boost::asio::posix::stream_descriptor descriptor_type;
typedef std::shared_ptr<descriptor_type> descriptor_ptr;

typedef fas::type_list_n<
  fas::type< _descriptor_type_, descriptor_type >,
  fas::advice< _create_, ad_create >,
  fas::value< _descriptor_ptr_, descriptor_ptr >,
  fas::group< wfc::io::_create_, _create_ >
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}
