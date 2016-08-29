#pragma once 

#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wjrpc/incoming/send_error.hpp>

namespace wfc{ namespace jsonrpc{

class target
  : public ijsonrpc
{
public:
  

  virtual ~target()
  {
  }

  target(const std::string& name, std::shared_ptr<wfcglobal> g)
  {
    if (!g) return;
    
    if ( auto t = g->registry.get<ijsonrpc>(name, true) )
    {
      _target_jsonrpc = t;
      _target_io = t;
    }
    else if ( auto t = g->registry.get<iinterface>(name) )
    {
      _target_io = t;
    }
  }

  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    if ( _target_io!=nullptr )
    {
      _target_io->reg_io(io_id, itf);
    }
  }

  virtual void unreg_io(io_id_t io_id)
  {
    if ( _target_io!=nullptr )
    {
      _target_io->unreg_io(io_id);
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
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

  virtual void perform_incoming( ijsonrpc::incoming_holder holder, ijsonrpc::io_id_t io_id, ijsonrpc::rpc_outgoing_handler_t handler) override
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
          _target_io->perform_io( std::move(d), io_id, [handler](data_ptr d)
          {
            handler( outgoing_holder( std::move(d) ) );
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
          _target_io->perform_io( std::move(d), io_id, [rh](data_ptr d)
          {
            incoming_holder holder( std::move(d) );
            ::wjson::json_error e;
            holder.parse(&e);
            if ( !e && holder )
            {
              rh( std::move(holder) );
            }
            else
            {
              ijsonrpc::rpc_outgoing_handler_t error_handler = [rh](outgoing_holder holder) 
              {
                auto d = holder.detach();
                rh( incoming_holder( std::move(d) ) );
              };
              aux::send_error( std::move(holder), std::make_unique<parse_error>(), error_handler );
            }
            /*
            // TODO: error try catch 
            holder.parse([rh](outgoing_holder holder) {
              auto d = holder.detach();
              rh( incoming_holder( std::move(d) ) );
            });
            */
            
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