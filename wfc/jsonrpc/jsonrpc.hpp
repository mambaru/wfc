#pragma once 


#include <iow/jsonrpc/handler/ihandler.hpp> // убрать, подключить опции

#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/domain_object.hpp>

namespace wfc{ 

template<typename HandlerOptions>
struct jsonrpc_options: HandlerOptions
{
  
};

template<typename JsonrpcHandler>
class jsonrpc
  : public domain_object<
              ijsonrpc, 
              jsonrpc_options< typename JsonrpcHandler::options_type> 
           >
{
  
  typedef JsonrpcHandler handler_type;
  typedef std::shared_ptr<handler_type> handler_ptr;
  
public:
  
  virtual void reconfigure()
  {
  }

  virtual void startup_io(io_id_t /*io_id*/, outgoing_handler_t /*handler*/)
  {
    
  }

  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, outgoing_handler_t handler)
  {
    if (handler!=nullptr)
      return handler(nullptr);
  }
  
  virtual void shutdown_io(io_id_t /*io_id*/)
  {
    
  }

private:
  
  handler_ptr _handler;
};

}