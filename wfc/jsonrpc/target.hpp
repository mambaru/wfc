#pragma once 

#include <wfc/jsonrpc/ijsonrpc.hpp>

namespace wfc{ namespace jsonrpc{

class target
  : public ijsonrpc
{
public:
  
  virtual ~target()
  {
  }
  
  void target(const std::string& name, std::shared_ptr<wfcglobal> global)
  {
    if ( auto t = global->registry.get<ijsonrpc>(name, true) )
    {
      _target_jsonrpc = t;
      _target_io = t;
    }
    else if ( auto t = global->registry.get<iinterface>(name) )
    {
      _target_io = t;
    }
    
  }
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    _target_io->reg_io(io_id, itf);
  }

  virtual void unreg_io(io_id_t io_id)
  {
    _target_io->unreg_io(io_id);
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    _target_io->perform_io( std::move(d), io_id, std::move(handler) );
  }

  virtual void perform_incoming( ijsonrpc::incoming_holder holder, ijsonrpc::io_id_t io_id, ijsonrpc::rpc_outgoing_handler_t handler) override
  {
    if ( _target_jsonrpc != nullptr )
    {
      _target_jsonrpc->perform_incoming( std::move(handler), io_id, std::move(handler) );
    }
    else
    {
      
    }
  }

  virtual void perform_outgoing( ijsonrpc::outgoing_holder holder, ijsonrpc::io_id_t io_id) override
  {
    if ( _target_jsonrpc != nullptr )
    {
      _target_jsonrpc->perform_outgoing( std::move(handler), io_id);
    }
    else
    {
      
    }
  }
  
private:

  std::shared_ptr<ijsonrpc>   _target_jsonrpc;
  std::shared_ptr<iinterface> _target_io;
};

}}