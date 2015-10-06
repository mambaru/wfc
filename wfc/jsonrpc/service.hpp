#pragma once 

//#include <iow/jsonrpc/handler/ihandler.hpp> // убрать, подключить опции


#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_domain.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <iow/jsonrpc/handler.hpp>

namespace wfc{ namespace jsonrpc{

template<typename MethodList, template<typename> class Impl = interface_implementation >
class service
  : public basic_domain< ijsonrpc, ::iow::jsonrpc::handler< Impl<MethodList> > >
{
public:
  virtual void perform_incoming( ijsonrpc::incoming_holder holder, ijsonrpc::io_id_t io_id, ijsonrpc::rpc_outgoing_handler_t handler) override
  {
    this->_engine->perform_incoming( std::move(holder), io_id, handler );
  }

  virtual void perform_outgoing( ijsonrpc::outgoing_holder holder, ijsonrpc::io_id_t io_id) override
  {
    abort();
  }
};

}}