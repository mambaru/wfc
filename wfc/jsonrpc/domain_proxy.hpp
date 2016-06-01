#pragma once

#include "target_adapter.hpp"

#include <wfc/domain_object.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename Config>
class domain_proxy
  : public ::wfc::domain_object< ijsonrpc, Config>
{
  typedef ::wfc::domain_object< ijsonrpc, Config> super;
  typedef std::weak_ptr<ijsonrpc> jsonrpc_wptr;
public:
  typedef typename super::io_id_t  io_id_t;
  typedef typename super::data_ptr  data_ptr;
  typedef typename super::io_outgoing_handler_t  io_outgoing_handler_t;
  typedef typename super::rpc_outgoing_handler_t  rpc_outgoing_handler_t;
  typedef typename super::incoming_holder incoming_holder;
  typedef typename super::outgoing_holder outgoing_holder;
  
  domain_proxy() {}
  
  target_adapter get_adapter(  const std::string& name ) const
  {
    auto itf = this->template get_target< iinterface >( name );
    auto jrpc = this->template get_target< ijsonrpc >( name, true );
    return target_adapter(itf, jrpc);
  }
  
  virtual void reconfigure() override
  {
    _adapter = this->get_adapter( this->options().target );
  }
  
// iinterface
  virtual void reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf) override
  {
    _adapter.reg_io( io_id, itf );
  }
  
  virtual void unreg_io(io_id_t io_id) override
  {
    _adapter.unreg_io( io_id);
  }
  
  virtual void perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) final override
  {
    if ( this->suspended() )
    {
      _adapter.perform_io( std::move(d), io_id, handler );
      return;
    }
    
    //TODO: проверить исключения jsonrpc
    while ( d != nullptr )
    {
      incoming_holder holder( std::move(d), false);
      d = holder.parse();
      this->perform_incoming( std::move(holder), io_id, [handler](outgoing_holder holder)
      {
        handler( holder.detach() );
      } );
    }
  }
  
// ijsonrpc
  virtual void perform_incoming( incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) override
  {
    _adapter.perform_incoming( std::move(holder), io_id, handler);
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    _adapter.perform_outgoing( std::move(holder), io_id);
  }
  
  using super::get_target;
  target_adapter get_target() const { return _adapter;}
  
public:
  target_adapter _adapter;
};

}}
