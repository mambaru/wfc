//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wjrpc/incoming/send_error.hpp>

namespace wfc{ namespace jsonrpc{

class target_depr
  : public ijsonrpc
{
public:
  

  virtual ~target_depr()
  {
  }

  target_depr(const std::string& name1, std::shared_ptr<wfcglobal> g)
  {
    if (!g) return;

    if ( auto t1 = g->registry.get<ijsonrpc>(name1, true) )
    {
      _target_jsonrpc = t1;
      _target_io = t1;
    }
    else if ( auto t2 = g->registry.get<iinterface>(name1) )
    {
      _target_io = t2;
    }
  }

  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override
  {
    if ( _target_io!=nullptr )
    {
      _target_io->reg_io(io_id, itf);
    }
  }

  virtual void unreg_io(io_id_t io_id) override
  {
    if ( _target_io!=nullptr )
    {
      _target_io->unreg_io(io_id);
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override
  {
    if ( _target_io!=nullptr )
    {
      _target_io->perform_io( std::move(d), io_id, std::move(handler) );
    }
    else if ( handler!=nullptr )
    {
      handler(nullptr);
    }
  }

  virtual void perform_incoming( ijsonrpc::incoming_holder holder, ijsonrpc::io_id_t io_id, ijsonrpc::outgoing_handler_t handler) override
  {
    if ( _target_jsonrpc != nullptr )
    {
      _target_jsonrpc->perform_incoming( std::move(holder), io_id, std::move(handler) );
    }
    else if (_target_io != nullptr )
    {
      if ( auto d = holder.detach() )
      {
        if ( handler!=nullptr )
        {
          _target_io->perform_io( std::move(d), io_id, [handler](data_ptr d2)
          {
            handler( outgoing_holder( std::move(d2) ) );
          });
        }
        else
        {
          _target_io->perform_io( std::move(d), io_id, nullptr);
        }
      }
      else if (handler!=nullptr)
      {
        handler( ijsonrpc::outgoing_holder() );
      }
    }
    else if (handler!=nullptr)
    {
      handler( ijsonrpc::outgoing_holder() );
    }
  }

  virtual void perform_outgoing( ijsonrpc::outgoing_holder holder, ijsonrpc::io_id_t io_id) override
  {
    if ( _target_jsonrpc != nullptr )
    {
      _target_jsonrpc->perform_outgoing( std::move(holder), io_id);
    }
    else if (_target_io != nullptr )
    {
      if ( auto d = holder.detach() )
      {
        if ( auto rh = holder.result_handler() )
        {
          _target_io->perform_io( std::move(d), io_id, [rh](data_ptr d2)
          {
            incoming_holder iholder( std::move(d2) );
            ::wjson::json_error e;
            iholder.parse(&e);
            if ( !e && iholder )
            {
              rh( std::move(iholder) );
            }
            else
            {
              ijsonrpc::outgoing_handler_t error_handler = [rh](outgoing_holder holder2) 
              {
                auto d3 = holder2.detach();
                rh( incoming_holder( std::move(d3) ) );
              };
              aux::send_error( std::move(iholder), std::make_unique<parse_error>(), error_handler );
            }
          });
        }
        else
        {
          _target_io->perform_io( std::move(d), io_id, nullptr);
        }
      }
      else if ( auto rh = holder.result_handler() )
      {
        rh( ijsonrpc::incoming_holder( data_ptr() ) );
      }
    }
    else if ( auto rh = holder.result_handler() )
    {
      rh( ijsonrpc::incoming_holder( data_ptr() ) );
    }
  }

private:

  std::shared_ptr<ijsonrpc>   _target_jsonrpc;
  std::shared_ptr<iinterface> _target_io;
};

}}
