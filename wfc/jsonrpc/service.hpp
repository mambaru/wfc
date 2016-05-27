#pragma once 

//#include <iow/jsonrpc/handler/ihandler.hpp> // убрать, подключить опции


#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_domain.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <iow/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/service_options.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class service_impl
  : public basic_domain<Interface, JsonrpcHandler, service_options>
{
  typedef basic_domain<Interface, JsonrpcHandler, service_options> super;
  typedef typename super::engine_type engine_type;
  typedef typename engine_type::options_type engine_options;
public:
  
  virtual void reconfigure() override
  {
    auto dopt = this->options();
    engine_options& eopt = static_cast<engine_options&>(dopt);
    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type interface_type;
    target_type target = this->global()->registry.template get< interface_type >(dopt.target);
    eopt.target = target;
    eopt.peeper = target;

    super::reconfigure_(dopt);
  }
};

template<typename MethodList, template<typename> class Impl = interface_implementation >
class service
  : public service_impl< ijsonrpc, ::iow::jsonrpc::handler< Impl<MethodList> > >
{
  typedef service_impl< ijsonrpc, ::iow::jsonrpc::handler< Impl<MethodList> > > super;
public:
  virtual ~service()
  {
  }
  
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