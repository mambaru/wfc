//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/jsonrpc/method_list_helper.hpp>

namespace wfc{ namespace jsonrpc{

template<typename ...Args >
class method_list
  : public method_list_helper<Args...>::base
  , public method_list_helper<Args...>::interface_type
{
public:
  typedef typename method_list_helper<Args...>::interface_type interface_type;
  typedef typename method_list_helper<Args...>::base super;
  typedef typename super::io_id_t   io_id_t;
  typedef typename super::data_type data_type;
  typedef typename super::data_ptr  data_ptr;
  typedef typename super::outgoing_handler_t outgoing_handler_t;
};

}}
