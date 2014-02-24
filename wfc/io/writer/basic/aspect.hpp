#pragma once

#include <wfc/io/writer/basic/ad_create.hpp>
#include <wfc/io/writer/basic/ad_ready.hpp>
#include <wfc/io/writer/basic/ad_handler.hpp>
#include <wfc/io/writer/basic/ad_make_buffer.hpp>
#include <wfc/io/writer/basic/tags.hpp>
#include <wfc/io/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace writer{  namespace basic{

typedef std::list< wfc::io::basic::data_ptr> data_list;

typedef fas::type_list_n<
  fas::advice<_create_, ad_create>,
  fas::advice<_ready_, ad_ready>,
  fas::advice<_handler_, ad_handler>,
  fas::advice<_make_buffer_, ad_make_buffer>,
  fas::alias< _incoming_, _handler_>,
  
  fas::value< _output_buffer_size_, size_t>,
  fas::value< _outgoing_buffer_list_, data_list>,

  fas::group< wfc::io::_create_, _create_>
  
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
