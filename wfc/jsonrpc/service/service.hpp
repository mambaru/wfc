#pragma once 

//#include <iow/jsonrpc/handler/ihandler.hpp> // убрать, подключить опции


#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_engine.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <iow/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/service/service_options.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class service_base
  : public basic_engine<Interface, JsonrpcHandler, service_options>
{
  typedef basic_engine<Interface, JsonrpcHandler, service_options> super;
  typedef typename super::engine_type engine_type;
  typedef typename engine_type::options_type engine_options;
public:
  
  virtual void initialize() override
  {
    auto dopt = this->options();
    //engine_options& eopt = static_cast<engine_options&>(dopt);
    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type interface_type;
    target_type target = this->global()->registry.template get< interface_type >(dopt.target_name);
    dopt.target = target;
    dopt.peeper = target;
    this->initialize_engine(dopt);
  }
};

template<typename MethodList, template<typename> class Impl = interface_implementation >
class service
  : public service_base< ijsonrpc, ::iow::jsonrpc::handler< Impl<MethodList> > >
{
  typedef service_base< ijsonrpc, ::iow::jsonrpc::handler< Impl<MethodList> > > super;
public:
  
  virtual void perform_incoming( ijsonrpc::incoming_holder holder, ijsonrpc::io_id_t io_id, ijsonrpc::rpc_outgoing_handler_t handler) override
  {
    if ( auto e = this->engine())
    {
      e->perform_jsonrpc( std::move(holder), io_id, handler );
    }
    else if ( handler!=nullptr && holder.is_request() )
    {
      handler( ijsonrpc::outgoing_holder() );
    }
  }

  virtual void perform_outgoing( ijsonrpc::outgoing_holder /*holder*/, ijsonrpc::io_id_t /*io_id*/) override
  {
    // TODO :  Преобразование outgoing_holder в incoming_holder и в engine + плюс захват response handler
    // Плюс нужен call_id реестр
    // TODO: сделать в endine 
    abort();
  }
private:
  
};

}}