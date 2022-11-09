//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2022
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/logger.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wjrpc/incoming/send_error.hpp>
#include <wjrpc/outgoing/outgoing_error.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>
#include <wjrpc/errors/errors.hpp>
#include <wjrpc/errors/error_json.hpp>
#include <wjrpc/types.hpp>
#include <wjson/json.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{

struct target_adapter: ijsonrpc
{
  typedef std::weak_ptr<iinterface> itf_ptr_t;
  typedef std::weak_ptr<ijsonrpc> jsonrpc_ptr_t;
  typedef ::wjrpc::call_id_t call_id_t;

  target_adapter() = default;

  explicit target_adapter(itf_ptr_t itf)
    : _itf(itf), _jsonrpc()
  {
    if (auto pitf = itf.lock())
      _jsonrpc = std::dynamic_pointer_cast<ijsonrpc>(pitf);
  }

  target_adapter(itf_ptr_t itf, jsonrpc_ptr_t jsonrpc)
    : _itf(itf), _jsonrpc(jsonrpc)
  {}

  target_adapter(const target_adapter& other)
    : ijsonrpc() 
    , _itf(other._itf)
    , _jsonrpc(other._jsonrpc)
  {}

  target_adapter& operator=(const target_adapter& other)
  {
    _itf = other._itf;
    _jsonrpc = other._jsonrpc;
    return *this;
  }

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
    this->perform_io_( std::move(d), io_id, std::move(handler) );
  }

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( auto p1 = _jsonrpc.lock() )
    {
      p1->perform_incoming( std::move(holder), io_id, handler);
    }
    else if ( auto p2 = _itf.lock() )
    {
      if ( handler!=nullptr )
      {
        this->perform_io_( holder.detach(), io_id, [handler](data_ptr d)
        {
          handler( outgoing_holder( std::move(d) ) );
        });
      }
      else
        this->perform_io_( holder.detach(), io_id, nullptr );
    }
    else if ( handler != nullptr )
    {
      ::wjrpc::aux::send_error(std::move(holder), std::make_unique< ::wjrpc::service_unavailable > (), std::move(handler));
    }
    else
    {
    }
  }

  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    if ( auto p1 = _jsonrpc.lock() )
    {
      p1->perform_outgoing( std::move(holder), io_id);
    }
    else if ( auto p2 = _itf.lock() )
    {
      auto handler = holder.result_handler();
      auto d = holder.detach();

      p2->perform_io( std::move(d), io_id, [handler](data_ptr d2)
      {
        while ( d2!=nullptr )
        {
          incoming_holder iholder( std::move(d2) );
          wjson::json_error e;
          d2 = iholder.parse(&e);
          if ( !e && iholder )
          {
            handler( std::move(iholder) );
          }
          else
          {
            ijsonrpc::outgoing_handler_t err_handler = [handler](outgoing_holder err)
            {
              auto d3 = err.detach();
              handler( incoming_holder( std::move(d3)));
            };

            wjrpc::aux::send_error(std::move(iholder), std::make_unique< wjrpc::parse_error>(), err_handler );
          }
        }
      });
    }
    else if ( auto handler = holder.result_handler() )
    {
      if ( auto call_id = holder.call_id() )
      {
        auto eholder = create_error_holder_< wjrpc::service_unavailable >(call_id);
        handler( std::move(eholder) );
      }
      else
      {
        handler( incoming_holder( data_ptr() ) );
      }
    }
  }
private:
  void perform_io_(data_ptr d, io_id_t io_id, output_handler_t handler)
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

  template<typename Error>
  incoming_holder create_error_holder_(call_id_t call_id)
  {
    wjrpc::outgoing_error< wjrpc::error > err;
    err.error = std::make_unique<Error>();
    err.id = std::make_unique<data_type>();
    wjson::value<call_id_t>::serializer()( call_id,  std::back_inserter(*(err.id)) );

    auto d = std::make_unique<data_type>();
    d->reserve(100);
    wjrpc::outgoing_error_json< wjrpc::error_json >::serializer()(err, std::back_inserter(*d));
    incoming_holder holder( std::move(d) );
    holder.parse(nullptr);
    return holder;
  }
private:

  itf_ptr_t _itf;
  jsonrpc_ptr_t _jsonrpc;
};


}}
