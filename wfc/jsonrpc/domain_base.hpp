#pragma once 

#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/memory.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename EngineType, typename OptionsType >
class domain_base
  : public domain_object< Interface, OptionsType>
{
public:
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

public:
  
  virtual void reconfigure() override
  {
    const auto& domain_opt = this->options();
    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type interface_type;
    target_type target = this->global()->registry.template get< interface_type >(domain_opt.target);
    engine_options engine_opt = static_cast<engine_options>(domain_opt);
    engine_opt.target = target;
    engine_opt.peeper = target;

    if ( _engine == nullptr ) 
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(engine_opt);
    }
    else
    {
      _engine->reconfigure(engine_opt);
    }
  }

  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> witf) override
  {
    if ( _engine != nullptr )
    {
      _engine->reg_io( io_id, this->wrap([witf]( iow::jsonrpc::outgoing_holder holder)
      {
        if (auto pitf = witf.lock() )
        {
          pitf->perform_io( std::move( holder.detach() ), 0, nullptr);
        }
      }));
    }
  }

  virtual void unreg_io(io_id_t io_id) override
  {
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
  
  template<typename Tg, typename ...Args>
  void call(Args... args)
  {
    this->_engine->template call<Tg>( std::forward<Args>(args)... );
  }

protected:

  engine_ptr _engine;
  
};

}}