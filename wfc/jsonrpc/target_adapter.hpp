//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/jsonrpc.hpp>
#include <wfc/logger.hpp>
#include <wjrpc/incoming/send_error.hpp>
#include <wjson/json.hpp>
#include <memory>


namespace wfc{ namespace jsonrpc{
  
struct target_adapter: ijsonrpc
{
  typedef std::weak_ptr<iinterface> itf_ptr_t;
  typedef std::weak_ptr<ijsonrpc> jsonrpc_ptr_t;
  
  target_adapter() {};
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
  
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override
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
  
  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( auto p = _jsonrpc.lock() )
    {
      p->perform_incoming( std::move(holder), io_id, handler);
    }
    else if ( auto p = _itf.lock() )
    {
      if ( handler!=nullptr )
      {
        this->perform_io( holder.detach(), io_id, [handler](data_ptr d)
        { 
          outgoing_holder holder;
          handler( outgoing_holder( std::move(d) ) );
        });
      }
      else
        this->perform_io( holder.detach(), io_id, nullptr );
    }
    else if ( handler != nullptr )
    {
      ::wjrpc::aux::send_error(std::move(holder), std::make_unique< ::wjrpc::service_unavailable > (), std::move(handler));
    }
    else
    {
    }
  }
  
  template<typename Error>
  incoming_holder create_error_holder_(call_id_t call_id)
  {
    jsonrpc::outgoing_error< ::wfc::jsonrpc::error > err;
    err.error = std::make_unique<Error>();
    err.id = std::make_unique<data_type>();
    json::value<call_id_t>::serializer()( call_id,  std::back_inserter(*(err.id)) );

    auto d = std::make_unique<data_type>();
    d->reserve(100);
    jsonrpc::outgoing_error_json< ::wfc::jsonrpc::error_json >::serializer()(err, std::back_inserter(*d));
    incoming_holder holder( std::move(d) );
    holder.parse(nullptr);
    return std::move(holder);
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
          ::wjson::json_error e;
          d = holder.parse(&e);
          if ( !e && holder )
          {
            handler( std::move(holder) );
          }
          else
          {
            ijsonrpc::outgoing_handler_t err_handler = [handler](outgoing_holder err)
            {
              auto d = err.detach();
              handler( incoming_holder( std::move(d)));
            };
            
            aux::send_error(std::move(holder), std::make_unique<parse_error>(), err_handler );
          }
        }
      });
    }
    else if ( auto handler = holder.result_handler() )
    {
      if ( auto call_id = holder.call_id() )
      {
        auto holder = create_error_holder_<service_unavailable>(call_id);
        handler( std::move(holder) );
      }
      else
      {
        handler( incoming_holder( data_ptr() ) );
      }
    }
  }
  
private:
  
  itf_ptr_t _itf;
  jsonrpc_ptr_t _jsonrpc;
};


}}
