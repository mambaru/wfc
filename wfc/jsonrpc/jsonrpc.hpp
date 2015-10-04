#pragma once 


//#include <iow/jsonrpc/handler/ihandler.hpp> // убрать, подключить опции
#include <iow/jsonrpc/engine.hpp>

#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/domain_object.hpp>
#include <wfc/memory.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{

using namespace ::iow::jsonrpc;

template<typename HandlerOptions>
struct jsonrpc_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  std::string target;
};

template<typename T>
struct jsonrpc_options_json
{
  typedef T options_type;
  typedef typename options_type::handler_options handler_options;
  JSON_NAME(target)
  JSON_NAME(direct_mode)
  
  //typedef wfc::json::member< n_allow_callback, handler_options, bool, &handler_options::allow_callback> member_type;
  
  typedef wfc::json::object<
    options_type,
    wfc::json::member_list<
      wfc::json::member< n_target, options_type, std::string, &options_type::target>,
      wfc::json::member< n_direct_mode, handler_options, bool, &handler_options::direct_mode>
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};


template<typename ...Args >
struct method_list_builder
{
  typedef ::iow::jsonrpc::method_list<
    Args...,
    interface_<iinterface>,
    target<iinterface>,
    peeper<iinterface>,
    startup_method<iinterface, iinterface, &iinterface::reg_io >,
    shutdown_method<iinterface, &iinterface::unreg_io>
  > base;
  
  typedef typename base::interface_type interface_type;
};

template<typename ...Args >
class method_list
  : public method_list_builder<Args...>::base
  , public method_list_builder<Args...>::interface_type
{
public:
  typedef typename method_list_builder<Args...>::base super;
  typedef typename super::io_id_t   io_id_t;
  typedef typename super::data_type data_type;
  typedef typename super::data_ptr  data_ptr;
  typedef typename super::outgoing_handler_t outgoing_handler_t;
  
  
};

template<typename JsonrpcHandler>
class service
  : public domain_object<
              ijsonrpc, 
              jsonrpc_options< 
                typename ::iow::jsonrpc::engine<JsonrpcHandler>::options_type
              > 
           >
{
public:
  typedef ::iow::jsonrpc::engine<JsonrpcHandler> engine_type;
  typedef std::shared_ptr<engine_type> engine_ptr;

  typedef typename engine_type::options_type engine_options;
  typedef jsonrpc_options<engine_options> options_type;

public:

  service()
  {
  }

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
    DEBUG_LOG_MESSAGE("wfc::jsonrpc::startup_io")
    if ( _engine != nullptr )
    {
      _engine->reg_io( io_id, this->wrap([witf](data_ptr d)
      {
        if (auto pitf = witf.lock() )
        {
          pitf->perform_io( std::move(d), 0, nullptr);
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

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( _engine != nullptr )
    {
      _engine->perform_incoming( std::move(holder), io_id, handler );
    }
  }

  virtual void perform_outgoing(outgoing_holder /*holder*/, io_id_t /*io_id*/) override
  {
    abort();
  }

private:
  
  engine_ptr _engine;
  
};

#warning copy-past from service

template<typename JsonrpcHandler>
class gateway
  : public domain_object<
              typename JsonrpcHandler::interface_type, 
              jsonrpc_options< 
                typename ::iow::jsonrpc::engine<JsonrpcHandler>::options_type
              > 
           >
  , public std::enable_shared_from_this< gateway<JsonrpcHandler> >
{
public:
  
  typedef gateway<JsonrpcHandler> self;
  
  typedef domain_object<
              typename JsonrpcHandler::interface_type, 
              jsonrpc_options< 
                typename ::iow::jsonrpc::engine<JsonrpcHandler>::options_type
              > 
           > super_domain;

  typedef JsonrpcHandler handler_type;
  //typedef std::shared_ptr<handler_type> handler_ptr;
  typedef ::iow::jsonrpc::engine<handler_type> engine_type;
  typedef std::shared_ptr<engine_type> engine_ptr;
  typedef typename engine_type::options_type engine_options;
  typedef jsonrpc_options<engine_options> options_type;

public:

  gateway()
    : _handler(std::make_shared<engine_type>())
  {
  }

  virtual void reconfigure() override
  {
    const auto& domain_opt = this->options();
    // typedef typename engine_type::target_type target_type;
    //typedef typename target_type::element_type interface_type;
    
    auto target = this->global()->registry.template get< iinterface >(domain_opt.target);
    
    if ( target != nullptr )
    {
      DOMAIN_LOG_DEBUG("JSONRPC Gateway: Target '" << domain_opt.target << "' founded!")
      engine_options engine_opt = static_cast<engine_options>(domain_opt);
      
      std::weak_ptr<self> wthis = this->shared_from_this();
      engine_opt.outgoing_handler=this->wrap([target, wthis](::iow::io::data_ptr d)
      {
        if ( auto pthis = wthis.lock() )
        {
          auto outgoing_handler = pthis->wrap([wthis](data_ptr d)
          {
            if ( auto pthis = wthis.lock() )
            {
              pthis->_handler->perform_io( std::move(d), 0, nullptr );
            }
          });
          target->perform_io(std::move(d), pthis->get_id(), std::move(outgoing_handler) );
        }
      });
      
      
      engine_opt.target = target;
      engine_opt.peeper = target;
            
      // ПЕРЕНЕСТИ в iow
      /*
      engine_opt.send_request = this->wrap([]( const char* name, result_handler_t, request_serializer_t )
      {
        DOMAIN_LOG_FATAL("engine_opt.send_request!=nullptr notimpl")
        abort();
      });
      engine_opt.send_notify = this->wrap([]( const char* name, result_handler_t, request_serializer_t )
      {
        DOMAIN_LOG_FATAL("engine_opt.send_request!=nullptr notimpl")
        abort();
      }); 
      */
      _handler->start(engine_opt);
    }
    else
    {
      DOMAIN_LOG_ERROR("JSONRPC Gateway: Target '" << domain_opt.target << "' not found")
    }
  }
  
  template<typename Tg, typename ...Args>
  void call(Args... args)
  {
    _handler->template call<Tg>( std::forward<Args>(args)... );
  }
  
  using super_domain::stop;
  using super_domain::start;
  
  
  virtual void reg_io(io_id_t /*id*/, std::weak_ptr<iinterface> /*wsrc*/) override
  {
    DEBUG_LOG_MESSAGE("wfc::jsonrpc::gateway::reg_io")
    // Не нужно, работаем с одним клиентом
    /*
    if ( _handler != nullptr )
    {
      _handler->reg_io(id, [id, wsrc](data_ptr d)
      {
        if ( auto psrc = wsrc.lock() )
        {
          psrc->perform_io( std::move(d), id, nullptr );
        }
      });
    }
    */
    
  }

  virtual void unreg_io(io_id_t /*id*/) override
  {
    DEBUG_LOG_MESSAGE("wfc::jsonrpc::gateway::reg_io")
    // Не нужно, работаем с одним клиентом
    
    /*
    if ( _handler != nullptr )
    {
      _handler->unreg_io(id);
    }
    */
  }

private:
  
  engine_ptr _handler;
  
  /*
#warning убрать!!!!
  using JsonrpcHandler::set;
  using JsonrpcHandler::get;
  */

  
  /*
  virtual void reg_io(io_id_t, std::weak_ptr<iinterface> ) override
  {
    DEBUG_LOG_MESSAGE("wfc::jsonrpc::startup_io")
    if ( _engine != nullptr )
    {
    }
  }

  virtual void unreg_io(io_id_t) override
  {
    if ( _engine != nullptr )
    {
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( _engine != nullptr )
    {
      _engine->invoke( std::move(d), io_id, std::move(handler) );
    }
  }

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( _engine != nullptr )
    {
      _engine->invoke( std::move(holder), io_id, handler );
    }
  }

  virtual void perform_outgoing(outgoing_holder , io_id_t ) override
  {
    abort();
  }
  */

  
};

}}