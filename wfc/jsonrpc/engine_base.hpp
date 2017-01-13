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
  typedef typename super::outgoing_handler_t outgoing_handler_t;
  typedef typename super::timer_id_t timer_id_t;
  typedef ::iow::workflow workflow_type;

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
    DEBUG_LOG_DEBUG( "engine_base::reg_io " << io_id )
    if ( _engine != nullptr )
    {
      _engine->reg_io( io_id, this->wrap([witf]( data_ptr d, io_id_t io_id, ::iow::io::outgoing_handler_t handler)
      {
        if (auto pitf = witf.lock() )
        {
          pitf->perform_io( std::move(d), io_id, handler);
        }
      }));
    }
  }

  virtual void unreg_io(io_id_t io_id) override
  {
    DEBUG_LOG_DEBUG( "engine_base::unreg_io " << io_id )
    if ( _engine != nullptr )
    {
      _engine->unreg_io(io_id);
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( _engine == nullptr )
      return;

    _engine->perform_io( std::move(d), io_id, std::move(handler) );
  }

  engine_ptr engine() const { return _engine; };


protected:

  void initialize_engine(options_type eopt)
  {
    eopt.log_error = [](const std::string& m) { JSONRPC_LOG_ERROR( m ); m.size();};
    eopt.log_fatal = [](const std::string& m) { JSONRPC_LOG_FATAL( m ); m.size();};
    eopt.log_debug = [](const std::string& m) { JSONRPC_LOG_DEBUG( m ); m.size();};
    eopt.log_trace = [](const std::string& m) { JSONRPC_LOG_TRACE( m ); m.size(); };
    
    if ( _engine == nullptr ) 
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(eopt, ::iow::io::create_id<size_t>() );
    }
    else
    {
      _engine->reconfigure( eopt );
    }

    //_workflow = this->get_workflow();
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
            JSONRPC_LOG_MESSAGE( " JSON-RPC remove outdated..." );
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
    
    JSONRPC_LOG_MESSAGE( "JSON-RPC: DEBUG STAT -1-");
    if ( auto stat = this->get_statistics() )
    {
      auto sopt = this->statistics_options();
      if ( auto wf = this->get_workflow() )
      {
        JSONRPC_LOG_MESSAGE( "JSON-RPC: DEBUG STAT -2-");
        wf->release_timer(_stat_timer_id);
        if ( sopt.interval_ms != 0 )
        {
          JSONRPC_LOG_MESSAGE( "JSON-RPC: DEBUG STAT -3-");
          auto name = this->name();
          auto handler_map  = stat->create_value_prototype(name + sopt.handler_map);
          auto result_map   = stat->create_value_prototype(name + sopt.result_map);
          auto result_queue = stat->create_value_prototype(name + sopt.result_queue);
          _stat_timer_id = wf->create_timer( 
            std::chrono::milliseconds(sopt.interval_ms), 
            [handler_map, result_map, result_queue, this]()->bool
            {
              if ( auto stat = this->get_statistics() )
              {
                JSONRPC_LOG_MESSAGE( "JSON-RPC: DEBUG STAT Ready! " << this->name());
                auto si = this->_engine->sizes();
                stat->create_meter(handler_map,  si.handler_map, 0);
                stat->create_meter(result_map,   si.result_map, 0);
                stat->create_meter(result_queue, si.result_queue, 0);
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
  //std::weak_ptr< workflow_type > _workflow;
protected:
  engine_ptr _engine;
};

}}
