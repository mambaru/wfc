#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class handler_base
{
public:
  virtual ~handler_base() {}
  virtual std::shared_ptr<handler_base> clone() = 0;
  virtual void process(incoming_holder holder) = 0;
  // ???
  std::function< void( wfc::io::data_ptr /*d*/, wfc::io::data_ptr /*id*/, std::shared_ptr<handler_base>) > outgoing = nullptr;
};

}} // wfc


