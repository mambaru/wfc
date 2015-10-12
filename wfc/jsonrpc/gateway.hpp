#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_domain.hpp>
#include <iow/jsonrpc/handler.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class gateway_impl
  : public basic_domain<Interface, JsonrpcHandler>
{
  typedef basic_domain<Interface, JsonrpcHandler> super;

  typedef typename super::engine_options engine_options;
  typedef typename super::interface_type interface_type;
  typedef typename super::engine_type    engine_type;

public:
  typedef typename super::io_id_t io_id_t;
  typedef typename super::data_ptr data_ptr;

  virtual void reconfigure() override
  {
    auto dopt = this->options();

    engine_options eopt = static_cast<engine_options>(dopt);
    super::reconfigure_(eopt);

    const auto& registry = this->global()->registry;

    if ( auto pitf = registry.template get<ijsonrpc>(dopt.target, true) )
    {
      io_id_t target_id = ::iow::io::create_id<size_t>();
      std::weak_ptr<ijsonrpc> witf = pitf;
      auto io_id = this->engine()->get_id();
      this->engine()->reg_jsonrpc(target_id, [witf, io_id]( ::iow::jsonrpc::outgoing_holder holder)
      {
        if ( auto pitf = witf.lock() )
        {
          pitf->perform_outgoing( std::move(holder), io_id );
        }
      });
    }
    else if ( auto pitf = registry.template get<iinterface>(dopt.target, true) )
    {
      io_id_t target_id = ::iow::io::create_id<size_t>();
      std::weak_ptr<iinterface> witf = pitf;
      this->engine()->reg_io(target_id, [witf]( data_ptr d, io_id_t io_id, ::iow::io::outgoing_handler_t handler )
      {
        if ( auto pitf = witf.lock() )
        {
          pitf->perform_io( std::move(d), io_id, std::move(handler) );
        }
      });
    }

  }
private:

};

template<typename MethodList, template<typename> class ItfT = interface_implementation>
class gateway
  : public ItfT< gateway_impl< typename MethodList::interface_type, ::iow::jsonrpc::handler< ItfT<MethodList> > > >
{
public:
  virtual ~gateway()
  {
  }
};

}}