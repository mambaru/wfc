#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_domain.hpp>
#include <wfc/jsonrpc/gateway_options.hpp>
#include <iow/jsonrpc/handler.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class gateway_impl
  : public basic_domain<Interface, JsonrpcHandler, gateway_options>
  , public std::enable_shared_from_this< gateway_impl<Interface, JsonrpcHandler> >
{
  typedef basic_domain<Interface, JsonrpcHandler, gateway_options> super;
  typedef typename super::engine_options engine_options;
  typedef typename super::interface_type interface_type;
  typedef typename super::engine_type    engine_type;

public:
  typedef typename super::io_id_t io_id_t;
  typedef typename super::data_ptr data_ptr;

  virtual ~gateway_impl() {}

  virtual void reconfigure() override
  {
    _target_id = ::iow::io::create_id<size_t>();
    auto dopt = this->options();
    engine_options eopt = static_cast<engine_options>(dopt);
    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type interface_type;
    target_type target = this->global()->registry.template get< interface_type >(dopt.incoming_target);
    eopt.target = target;
    eopt.peeper = target;

    super::reconfigure_(eopt);

    const auto& registry = this->global()->registry;

    if ( auto pitf = registry.template get<ijsonrpc>(dopt.outgoing_target, true) )
    {
      std::weak_ptr<ijsonrpc> witf = pitf;
      io_id_t io_id = _target_id;
      this->engine()->reg_jsonrpc( _target_id, [witf, io_id]( ::iow::jsonrpc::outgoing_holder holder)
      {
        if ( auto pitf = witf.lock() )
        {
          pitf->perform_outgoing( std::move(holder), io_id );
        }
      });
    }
    else if ( auto pitf = registry.template get<iinterface>(dopt.outgoing_target, false) )
    {
      std::weak_ptr<iinterface> witf = pitf;
      this->engine()->reg_io( _target_id, [witf]( data_ptr d, io_id_t io_id, ::iow::io::outgoing_handler_t handler )
      {
        if ( auto pitf = witf.lock() )
        {
          auto tmp = [handler](data_ptr d) 
          { 
            handler( std::move(d) );
          };
          pitf->perform_io( std::move(d), io_id, std::move(handler) );
        }

      });
    }
  }

  template<typename Tg, typename Req, typename ...Args>
  void call(Req req, Args... args)
  {
    this->engine()->template call<Tg>( std::move(req), _target_id, std::forward<Args>(args)... );
  }

  /**
   * Используеться только шлюзом, в основном для предварительного коннекта
   * если первый запрос должен идти от сервера 
   */
  void connect(io_id_t io_id)
  {
    const auto& dopt = this->options();
    if ( auto pitf = this->global()->registry.template get<iinterface>(dopt.target) )
    {
      pitf->reg_io(io_id, this->shared_from_this());
    }
  }

  void disconnect(io_id_t io_id)
  {
    const auto& dopt = this->options();
    if ( auto pitf = this->global()->registry.template get<iinterface>(dopt.target) )
    {
      pitf->unreg_io(io_id);
    }
  }

private:
  std::atomic<io_id_t> _target_id;
};

template<typename MethodList, template<typename> class ItfT = interface_implementation>
class gateway
  : public ItfT< gateway_impl< typename MethodList::interface_type, ::iow::jsonrpc::handler< ItfT<MethodList> > > >
{
};

}}
