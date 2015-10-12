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
  virtual ~service()
  {
    std::cout << "~service() " << std::endl;
  }

  virtual void perform_incoming( ijsonrpc::incoming_holder holder, ijsonrpc::io_id_t io_id, ijsonrpc::rpc_outgoing_handler_t handler) override
  {
    this->engine()->perform_incoming( std::move(holder), io_id, handler );
  }

  virtual void perform_outgoing( ijsonrpc::outgoing_holder /*holder*/, ijsonrpc::io_id_t /*io_id*/) override
  {
    // TODO :  Преобразование outgoing_holder в incoming_holder и в engine + плюс захват response handler
    // Плюс нужен call_id реестр
    abort();
  }
};

}}