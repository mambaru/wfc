//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/memory.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wfc/jsonrpc/statistics_options.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename EngineType, typename OptionsType >
class engine_base
  : public domain_object< Interface, OptionsType, statistics_options>
{
public:
  typedef engine_base<Interface, EngineType, OptionsType> self;
  typedef domain_object< Interface, OptionsType, statistics_options> super;
  
  typedef Interface interface_type;
  typedef EngineType engine_type;
  typedef OptionsType options_type;
  
  typedef std::shared_ptr<engine_type> engine_ptr;

  typedef typename options_type::engine_options engine_options;
  
  typedef typename super::io_id_t io_id_t;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::data_type data_type;
  typedef typename super::output_handler_t output_handler_t;
  typedef wflow::workflow workflow_type;
  typedef workflow_type::timer_id_t timer_id_t;

public:

  virtual ~engine_base()
  {
    _engine=nullptr;
  }


  virtual void stop() final override
  {
    if ( _engine != nullptr ) 
    {
      if ( auto wf = this->get_workflow() )
      {
        wf->release_timer(_timer_id);
        wf->release_timer(_stat_timer_id);
      }
      _engine->stop();

    }
    _engine = nullptr;
  }


  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> witf) override
  {
    if ( this->suspended() )
      return;
    if ( _engine != nullptr )
    {
      _engine->reg_io( io_id, this->wrap([witf]( data_ptr d, io_id_t io_id, output_handler_t handler)
      {
        if (auto pitf = witf.lock() )
        {
          pitf->perform_io( std::move(d), io_id, handler);
        }
      }, nullptr));
    }
  }

  virtual void unreg_io(io_id_t io_id) override
  {
    if ( this->suspended() )
      return;
    if ( _engine != nullptr )
    {
      _engine->unreg_io(io_id);
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override
  {
    if ( this->suspended() )
    {
      if (handler!=nullptr)
        handler(std::move(d) );
      return;
    }
    
    if ( _engine == nullptr )
      return;

    _engine->perform_io( std::move(d), io_id, std::move(handler) );
  }

  engine_ptr engine() const { return _engine; };


protected:

  void initialize_engine(options_type eopt)
  {
    if ( _engine == nullptr ) 
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(eopt, ::iow::io::create_id<size_t>() );
    }
    else
    {
      _engine->reconfigure( eopt );
    }

    if ( auto wf = this->get_workflow() )
    {
      wf->release_timer( _timer_id );
      if ( eopt.remove_outdated_ms != 0 )
      {
        auto engine = this->_engine;
        _timer_id = wf->create_timer(
          std::chrono::milliseconds(eopt.remove_outdated_ms),
          [engine]() -> bool 
          {
            JSONRPC_LOG_DEBUG( " JSON-RPC remove outdated...");
            if ( size_t count = engine->remove_outdated() )
            {
              JSONRPC_LOG_WARNING( "JSON-RPC: " << count << " calls is outdated.");
            }
            return true;
          }
        );
      }
    }
    this->initialize_statistics_();
  }

private:
  void initialize_statistics_() 
  {
    if ( auto stat = this->get_statistics() )
    {
      auto sopt = this->statistics_options();
      if ( auto wf = this->get_workflow() )
      {
        wf->release_timer(_stat_timer_id);
        if ( sopt.interval_ms != 0 )
        {
          auto name = this->name();
          auto handler_map  = stat->create_value_meter(name + sopt.handler_map);
          auto result_map   = stat->create_value_meter(name + sopt.result_map);
          auto result_queue = stat->create_value_meter(name + sopt.result_queue);
          _stat_timer_id = wf->create_timer( 
            std::chrono::milliseconds(sopt.interval_ms), 
            [handler_map, result_map, result_queue, this]()->bool
            {
              if ( auto stat = this->get_statistics() )
              {
                auto si = this->_engine->sizes();
                handler_map.create(si.handler_map, 0);
                result_map.create(si.result_map, 0);
                result_queue.create(si.result_queue, 0);
                return true;
              }
              return false;
            }
          );
        }
      }
    }
  }

  
private:
  timer_id_t _timer_id = 0;
  timer_id_t _stat_timer_id = 0;
protected:
  engine_ptr _engine;
};

}}
