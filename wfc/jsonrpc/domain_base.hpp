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
  typedef domain_base<Interface, EngineType, OptionsType> self;
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

  virtual ~domain_base()
  {
    _engine=nullptr;
  }

  /*
  template<typename Target>
  void io_outgoing_result_(Target target, data_ptr d)
  {
    using namespace std::placeholders;
    this->perform_io(
      std::move(d), 
      this->get_id(),
      std::bind(&self::io_outgoing_handler_<Target>, this, target, _1)
    );
  }

  */
  
  /*
  template<typename Target>
  void io_outgoing_handler_(Target target, data_ptr d)
  {
    using namespace std::placeholders;
    target->perform_io( 
      std::move(d),
      this->get_id(), 
      std::bind(&self::io_outgoing_result_<Target>, this, target, _1) 
    );
    
  }
  */

  void reconfigure_(const engine_options& opt)
  {
    if ( _engine == nullptr ) 
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(opt);
    }
    else
    {
      _engine->reconfigure(opt);
    }
  }


  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> witf) override
  {
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
  
  /*template<typename Tg, typename Req, typename ...Args>
  void call(Req req, Args... args)
  {
    if ( _engine != nullptr )
    {
      this->_engine->template call<Tg>( std::forward<Args>(args)... );
    }
    else
    {
      // TODO: 
      abort();
    }
  }
  */

  engine_ptr engine() const { return _engine; };
private:

  engine_ptr _engine;
  
};

}}
