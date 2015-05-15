#pragma once

#include <wfc/io/acceptor/basic/ad_make_buffer.hpp>
#include <wfc/io/acceptor/basic/ad_create.hpp>
#include <wfc/io/acceptor/basic/ad_ready.hpp>
#include <wfc/io/acceptor/basic/ad_handler.hpp>
#include <wfc/io/acceptor/basic/ad_accept.hpp>
#include <wfc/io/acceptor/basic/tags.hpp>
#include <wfc/io/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace acceptor{  namespace basic{

typedef fas::type_list_n<
  
  fas::advice<_make_buffer_, ad_make_buffer>,
  fas::advice<_create_, ad_create>,
  fas::advice<_ready_, ad_ready>,
  fas::advice<_handler_, ad_handler>,
  fas::advice<_accept_, ad_accept>,
  fas::alias< _incoming_, _handler_>,
  fas::group< wfc::io::_on_create_, _create_>
  
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list/*,
  fas::alias< _outgoing_, wfc::io::reader::_outgoing_>*/
>
{};

}}}}
