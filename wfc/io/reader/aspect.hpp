#pragma once


#include <wfc/io/tags.hpp>
#include <wfc/io/reader/tags.hpp>
#include <wfc/io/reader/ad_read.hpp>
#include <wfc/io/reader/ad_async_read.hpp>
#include <wfc/io/reader/ad_read_some.hpp>
#include <wfc/io/reader/ad_async_read_some.hpp>
#include <wfc/io/reader/ad_create_buffer.hpp>
#include <wfc/io/reader/ad_buffer_manager.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <list>
#include <queue>

namespace wfc{ namespace io{ namespace reader{

typedef fas::type_list_n<
  fas::advice< _read_some_, ad_read_some >,
  fas::advice< _async_read_some_, ad_async_read_some >,
  fas::type<_data_type_, std::vector<char> >,
  fas::value<_incoming_list_, std::queue< std::unique_ptr<std::vector<char> > > >,
  fas::value<_input_list_, std::queue< std::unique_ptr<std::vector<char> > > >,
  fas::advice< _buffer_manager_, ad_buffer_manager>,
  fas::value<_callback_list_, std::queue< std::function<void(std::unique_ptr<std::vector<char> >)> > >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  wfc::io::posix::aspect,
  fas::stub<_aborted_>,
  fas::stub<_error_>,
  fas::advice< _read_, ad_read >,
  fas::advice< _async_read_, ad_async_read >,
  fas::alias<_incoming_, _read_>,
  fas::advice< _create_buffer_, ad_create_buffer>
>
{};

}}}
