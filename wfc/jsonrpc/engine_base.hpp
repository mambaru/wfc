#pragma once 

#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/memory.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename EngineType, typename OptionsType >
class engine_base
  : public domain_object< Interface, OptionsType>
{
public:
  typedef engine_base<Interface, EngineType, OptionsType> self;
  typedef domain_object< Interface, OptionsType> super;
  
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
  
  engine_base()
    : _timer_id(0) 
  {}

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
    if ( _engine != nullptr )
    {
      _engine->perform_io( std::move(d), io_id, std::move(handler) );
    }
  }

  engine_ptr engine() const { return _engine; };

  virtual void stop(const std::string& ) final override
  {
    if ( _engine != nullptr ) 
    {
      if ( auto wf = _workflow.lock() )
        wf->release_timer(_timer_id);
      _engine->stop();

    }
    _engine = nullptr;
  }
protected:
  void initialize_engine(/*engine_options*/ options_type eopt)
  {
    //eopt.engine_args.workflow = this->get_workflow();

    if ( _engine == nullptr ) 
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(eopt, ::iow::io::create_id<size_t>() );
    }
    else
    {
      _engine->reconfigure( eopt );
    }

    _workflow = this->get_workflow();
    if ( auto wf = _workflow.lock() )
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
  }

private:
  timer_id_t _timer_id = 0;
  std::weak_ptr< workflow_type > _workflow;
  engine_ptr _engine;
  
};

}}
