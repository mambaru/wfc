#pragma once

#include <wfc/jsonrpc/target_adapter.hpp>

namespace wfc{ namespace jsonrpc{

class consumer_adapter
  : public target_adapter
{
public:
  explicit consumer_adapter(target_adapter::itf_ptr_t itf)
    : target_adapter(itf)
  {
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override
  {
    while ( d!=nullptr )
    {
      incoming_holder holder( std::move(d) );
      wjson::json_error e;
      d = holder.parse(&e);
      if ( !e && holder )
      {
        this->perform_incoming( std::move(holder), io_id, [handler](outgoing_holder oholder)
        {
          handler(oholder.detach());
        });
      }
      else if (handler!=nullptr)
      {
        handler(nullptr);
      }
    }
  }
};

}}
