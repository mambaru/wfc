//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "target_adapter.hpp"

#include <wfc/domain_object.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{

template<typename Config, typename StatConfig>
class domain_proxy
  : public ::wfc::domain_object< ijsonrpc, Config, StatConfig>
{
  typedef ::wfc::domain_object< ijsonrpc, Config, StatConfig> super;
  typedef std::weak_ptr<ijsonrpc> jsonrpc_wptr;
public:
  typedef typename super::io_id_t  io_id_t;
  typedef typename super::data_ptr  data_ptr;
  typedef typename super::output_handler_t  output_handler_t;
  typedef typename super::outgoing_handler_t  outgoing_handler_t;
  typedef typename super::incoming_holder incoming_holder;
  typedef typename super::outgoing_holder outgoing_holder;
  
  domain_proxy() {}

  virtual void initialize() override
  {
    _adapter = this->get_adapter( this->options().target );
  }
  
// iinterface
  virtual void reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf) override
  {
    super::reg_io(io_id, itf);
    _adapter.reg_io( io_id, itf );
  }
  
  virtual void unreg_io(io_id_t io_id) override
  {
    super::unreg_io(io_id);
    _adapter.unreg_io( io_id);
  }
  
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override
  {
    if ( this->suspended() )
    {
      _adapter.perform_io( std::move(d), io_id, handler );
      return;
    }
    
    while ( d != nullptr )
    {
      incoming_holder holder( std::move(d));

      wjson::json_error e;
      d = holder.parse(&e);
      if ( holder.is_valid() )
      {
        outgoing_handler_t jsonrpc_handler = nullptr;
        if ( holder.is_request() )
          jsonrpc_handler = [handler](outgoing_holder holder2) { handler( holder2.detach() ); };
        this->perform_incoming( std::move(holder), io_id,  jsonrpc_handler);
      }
      else 
      {
        outgoing_handler_t error_handler = nullptr;
        if ( holder.has_id() )
        {
          error_handler = [handler](outgoing_holder oholder)
          {
            auto d2 = oholder.detach();
            if ( d2!=nullptr ) { JSONRPC_LOG_ERROR( d2 )}
            handler( std::move(d2) );
          };
        }
        
        if ( e )
        {
          JSONRPC_LOG_ERROR( "domain_proxy: Parse error: " << holder.error_message(e) );
          if ( error_handler != nullptr )
            this->send_error< wjrpc::parse_error >( std::move(holder), error_handler);
        }
        else
        {
          JSONRPC_LOG_ERROR( "domain_proxy: Invalid JSON-RPC: " << holder.str() );
          if ( error_handler != nullptr )
            this->send_error< wjrpc::invalid_request >( std::move(holder), error_handler);
        }
      }
    }
  }
  
// ijsonrpc
  virtual void perform_incoming( incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    _adapter.perform_incoming( std::move(holder), io_id, handler);
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    _adapter.perform_outgoing( std::move(holder), io_id);
  }
  
  template<typename Err>
  static void send_error(incoming_holder holder, outgoing_handler_t handler)
  {
    if ( handler != nullptr )
      wjrpc::aux::send_error(std::move(holder), std::make_unique< Err > (), std::move(handler));
  }
  
  using super::get_target;
  target_adapter get_target() const { return _adapter;}
 
  target_adapter get_adapter(  const std::string& aname, bool disabort = false ) const
  {
    auto itf = this->template get_target< iinterface >( aname, disabort );
    auto jrpc = this->template get_target< ijsonrpc >( aname, true );
    return target_adapter(itf, jrpc);
  }

public:
  target_adapter _adapter;
};

}}
