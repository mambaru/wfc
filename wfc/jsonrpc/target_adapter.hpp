#pragma once

#include <wfc/jsonrpc.hpp>
#include <wfc/logger.hpp>
#include <iow/jsonrpc/incoming/aux.hpp>
#include <memory>




namespace wfc{ namespace jsonrpc{
  
struct target_adapter: ijsonrpc
{
  typedef std::weak_ptr<iinterface> itf_ptr_t;
  typedef std::weak_ptr<ijsonrpc> jsonrpc_ptr_t;
  
  target_adapter() = default;
  target_adapter(const target_adapter&) = default;
  target_adapter(target_adapter&&) = default;

  target_adapter& operator=(const target_adapter&) = default;
  target_adapter& operator=(target_adapter&&) = default;
  
  target_adapter(itf_ptr_t itf, jsonrpc_ptr_t jsonrpc)
    : _itf(itf), _jsonrpc(jsonrpc)
  {};
  
  operator bool () const
  {
    return _itf.lock()!=nullptr || _jsonrpc.lock()!=nullptr;
  }
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override
  {
    if ( auto p = _itf.lock() )
    {
      p->reg_io(io_id, itf); 
    }
  }
  virtual void unreg_io(io_id_t io_id) override
  {
    if ( auto p = _itf.lock() )
    {
      p->unreg_io(io_id);
    }
  }
  
  virtual void perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) override
  {
    if ( auto p = _itf.lock() )
    {
      p->perform_io( std::move(d), io_id, std::move(handler) );
    }
    else if ( handler != nullptr)
    {
      handler(nullptr);
    }
  }
  
  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) override
  {
    if ( auto p = _jsonrpc.lock() )
    {
      p->perform_incoming( std::move(holder), io_id, handler);
    }
    else if ( auto p = _itf.lock() )
    {
      this->perform_io( holder.detach(), io_id, [handler](data_ptr d)
      { 
        if ( handler !=nullptr )
        {
          outgoing_holder holder;
          handler( outgoing_holder( std::move(d) ) );
        }
      });
    }
    else if ( handler != nullptr )
    {
      ::iow::jsonrpc::aux::send_error(std::move(holder), std::make_unique< ::iow::jsonrpc::service_unavailable > (), std::move(handler));
    }
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    if ( auto p = _jsonrpc.lock() )
    {
      p->perform_outgoing( std::move(holder), io_id);
    }
    else if ( auto p = _itf.lock() )
    {
      auto handler = holder.result_handler();
      auto d = holder.detach();

      p->perform_io( std::move(d), io_id, [handler, io_id](data_ptr d) 
      {
        while ( d!=nullptr )
        {
          incoming_holder holder( std::move(d) );
          d = holder.parse();
          handler( std::move(holder) );
        }
      } );
    }
    else if ( auto handler = holder.result_handler() )
    {
      if ( auto call_id = holder.call_id() )
      {
        ::wfc::jsonrpc::outgoing_error< ::wfc::jsonrpc::error > err;
        err.error = std::make_unique<service_unavailable>();
        err.id = std::make_unique<data_type>();
        ::iow::json::value<call_id_t>::serializer()( call_id,  std::back_inserter(*(err.id)) );

        auto d = std::make_unique<data_type>();
        d->reserve(100);
        ::wfc::jsonrpc::outgoing_error_json< ::wfc::jsonrpc::error_json >::serializer()(err, std::back_inserter(*d));
        incoming_holder holder( std::move(d) );
        holder.parse();
        handler( std::move(holder) );
      }
      else
      {
        handler( incoming_holder(nullptr) );
      }
    }
  }
  
private:
  
  itf_ptr_t _itf;
  jsonrpc_ptr_t _jsonrpc;
};


}}
