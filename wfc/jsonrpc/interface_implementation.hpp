//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

namespace wfc{ namespace jsonrpc{

template<typename Base >
class interface_implementation
  : public Base
{
public:
  typedef Base super; // JsonrpcHandler
  typedef typename super::interface_type interface_type;
  typedef typename super::data_type data_type;
  typedef typename super::data_ptr  data_ptr;
  typedef typename super::io_id_t   io_id_t;

  typedef typename super::outgoing_handler_t outgoing_handler_t;

private:
};

}}
