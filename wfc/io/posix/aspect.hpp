#pragma once

#include <wfc/io/posix/tags.hpp>
#include <wfc/io/tags.hpp>

#include <boost/asio/posix/stream_descriptor.hpp>

#include <memory>

namespace wfc{ namespace io{ namespace posix{

typedef boost::asio::posix::stream_descriptor descriptor_type;

typedef fas::type_list_n<
  fas::type< _descriptor_type_, descriptor_type>
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}
