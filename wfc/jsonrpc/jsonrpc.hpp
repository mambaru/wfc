#pragma once 


//#include <iow/jsonrpc/handler/ihandler.hpp> // убрать, подключить опции
#include <iow/jsonrpc/engine.hpp>

#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/domain_object.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace jsonrpc{

using namespace ::iow::jsonrpc;

template<typename HandlerOptions>
struct jsonrpc_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  //bool allow_callback=true;
  std::string target;
};

template<typename T>
struct jsonrpc_options_json
{
  typedef T options_type;
  typedef typename options_type::handler_options handler_options;
  JSON_NAME(target)
  JSON_NAME(allow_callback)
  
  typedef wfc::json::member< n_allow_callback, handler_options, bool, &handler_options::allow_callback> member_type;
  
  typedef wfc::json::object<
    options_type,
    wfc::json::member_list<
      wfc::json::member< n_target, options_type, std::string, &options_type::target>,
      wfc::json::member< n_allow_callback, handler_options, bool, &handler_options::allow_callback>
      /*,
      wfc::json::member< n_allow_callback, handler_options, bool, &handler_options::allow_callback>
      */
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};



template<typename Opt>
struct iengine: ijsonrpc
{
  typedef Opt options_type;
  virtual bool allow_callback() const = 0;
  virtual void reconfigure(const options_type& opt) = 0;
  virtual void start(const options_type& opt) = 0;
  virtual void stop() = 0;
};


template<typename JsonrpcHandler>
class direct_engine
  : public iengine<typename JsonrpcHandler::options_type >
{
  typedef JsonrpcHandler handler_type;
  typedef typename handler_type::options_type options_type;
  typedef std::shared_ptr<handler_type> handler_ptr;
  
public:
  
  virtual bool allow_callback() const override
  {
    return false;
  }
  
  virtual void start(const options_type& opt) override
  {
    if ( _handler==nullptr) 
    {
      _handler = std::make_shared<handler_type>();
      _handler->start(opt);
    }
    else
    {
      _handler->initialize(opt);
    }
    
  }

  virtual void stop() override
  {
    if ( _handler )
    {
      _handler->stop();
    }
  }

  virtual void reconfigure(const options_type& opt) override
  {
    if ( _handler )
    {
      _handler->initialize(opt);
    }
  }
  
  virtual void startup_io(io_id_t, outgoing_handler_t) override
  {
  }
  
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override
  {
    // TODO: try catch
    incoming_holder holder( std::move(d) );
    holder.parse();
    this->perform_incoming( std::move(holder), io_id, std::move(handler) );
  }
  
  virtual void shutdown_io(io_id_t ) override
  {
  }
  
  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    _handler->invoke( std::move(holder), std::move(handler) );
  }
  
private:
  handler_ptr _handler;
};

template<typename JsonrpcHandler>
class duplex_engine
  : public iengine<typename JsonrpcHandler::options_type >
{
  typedef JsonrpcHandler handler_type;
  typedef typename handler_type::options_type options_type;
  typedef std::shared_ptr<handler_type> handler_ptr;

public:

  virtual bool allow_callback() const override
  {
    return true;
  }
  
  virtual void start(const options_type& opt) override
  {
    abort();
  }

  virtual void stop() override
  {
    abort();
  }

  virtual void reconfigure(const options_type& opt) override
  {
    abort();
  }
  
  virtual void startup_io(io_id_t, outgoing_handler_t) override
  {
    abort();
  }
  
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override
  {
    abort();
  }
  
  virtual void shutdown_io(io_id_t ) override
  {
    abort();
  }

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    abort();
  }
  
private:
};


template<typename JsonrpcHandler>
class jsonrpc
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
  
  /*
  typedef JsonrpcHandler handler_type;
  typedef typename JsonrpcHandler::options_type handler_options;
  typedef iengine<handler_options> engine_interface;
  typedef direct_engine<handler_type> direct_engine_t;
  typedef duplex_engine<handler_type> duplex_engine_t;
  typedef std::shared_ptr<engine_interface> engine_ptr;
  //typedef std::shared_ptr<handler_type> handler_ptr;
  */
  /*typedef std::shared_ptr<direct_engine> direct_engine_ptr;
  typedef std::shared_ptr<direct_engine> duplex_engine_ptr;
  */
public:
  jsonrpc()
   // : _handler( std::make_shared<handler_type>() )
   // : _engine( std::make_shared<engine_type>() )
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
    /*engine_opt.target = engine_opt;
    engine_opt.allow_callback
    */
    if ( _engine == nullptr ) 
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(engine_opt);
    }
    else
    {
      _engine->initialize(engine_opt);
    }
    
    /*
    typedef typename handler_type::target_type target_type;
    typedef typename target_type::element_type interface_type;
    //typedef typename handler_type::options_type handler_options;
    handler_options hopt;
    const auto& opt = this->options();
    target_type target = this->global()->registry.template get< interface_type >( opt.target);
    hopt.target = target;

    if ( _engine != nullptr && _engine->allow_callback() != opt.allow_callback )
    {
      _engine->stop();
      _engine = nullptr;
    }
    
    if ( _engine==nullptr )
    {
      if ( opt.allow_callback )
      {
        _engine = std::make_shared<duplex_engine_t>();
      }
      else
      {
        _engine = std::make_shared<direct_engine_t>();
      }
      
      _engine = opt.allow_callback 
                ? engine_ptr( std::make_shared<duplex_engine_t>() )
                : engine_ptr( std::make_shared<direct_engine_t>() );
                */
      /*
      _engine->start(hopt);
    }
    else
    {
      _engine->reconfigure(hopt);
    }
    */
    
    /*if ( opt.allow_callback )
    {
      
    }*/
    /*
    if ( opt.allow_callback )
    {
      _direct_engine = nullptr;
      if ( _duplex_engine == nullptr )
      {
        _duplex_engine = std::make_shared<duplex_engine>();
        _duplex_engine->start(opt);
      }
      else
      {
        _duplex_engine->reconfigure(opt);
      }
    }
    else
    {
      
      _direct_engine 
    }
    */
    
    
    //this->global()->registry
    /*
    opt.target = this->global()->registry.get< handler_type::target_type >(
      ""
    );
    */
    
    /*
    if ( _handler==nullptr )
    {
      _handler = std::make_shared<handler_type>();
      _handler->start(opt);
    }
    else
    {
      _handler->initialize(opt);
    }*/
    
  }

  virtual void startup_io(io_id_t io_id, outgoing_handler_t handler) override
  {
    /*
    if ( _engine != nullptr )
    {
      _engine->startup_io( io_id, std::move(handler) );
    }
    */
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( _engine != nullptr )
    {
      _engine->invoke( std::move(d), io_id, std::move(handler) );
    }

    /*
    incoming_holder holder( std::move(d) );
    holder.parse();
    this->perform_incoming( std::move(holder), io_id, std::move(handler) );
    */
    /*
    std::cout << "jsonrpc domain!!" << std::endl;
    if (handler!=nullptr)
      return handler(nullptr);
    */
  }

  virtual void shutdown_io(io_id_t io_id) override
  {
    /*
    if ( _engine != nullptr )
    {
      _engine->shutdown_io(io_id );
    }
    */
  }

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    if ( _engine != nullptr )
    {
      _engine->invoke( std::move(holder), io_id, handler );
    }
    
  }

private:
  engine_ptr _engine;
  /*
  direct_engine_ptr _direct_engine;
  duplex_engine_ptr _duplex_engine;
  */
};

}}