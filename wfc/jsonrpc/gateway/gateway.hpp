//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_engine.hpp>
#include <wfc/jsonrpc/gateway/gateway_options.hpp>
#include <wjrpc/handler.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class gateway_impl
  : public basic_engine<Interface, JsonrpcHandler, gateway_options>
  , public std::enable_shared_from_this< gateway_impl<Interface, JsonrpcHandler> >
{
  typedef basic_engine<Interface, JsonrpcHandler, gateway_options> super;
  typedef typename super::engine_options engine_options;
  typedef typename super::interface_type interface_type;
  typedef typename super::engine_type    engine_type;

public:
  typedef typename super::io_id_t io_id_t;
  typedef typename super::data_ptr data_ptr;

  virtual ~gateway_impl() {}

  virtual void initialize() override
  {
    _target_id = ::iow::io::create_id<size_t>();
    auto dopt = this->options();
    //engine_options& eopt = static_cast<engine_options&>(dopt);
    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type target_interface;
    target_type target = this->template get_target<target_interface>(dopt.incoming_target_name);
    dopt.target = target;
    dopt.peeper = target;
    this->initialize_engine( dopt);
  }

  virtual void start() override
  {
    auto dopt = this->options();

    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type target_interface;
    target_type target = this->template get_target<target_interface>(dopt.incoming_target_name);

    if ( target!=nullptr && dopt.incoming_reg)
      target->reg_io( this->engine()->get_id(), this->shared_from_this()  );

    this->engine()->unreg_io(_target_id); // видимо какой-то грязный хак
    if ( auto pitf = this->template get_target<ijsonrpc>(dopt.outgoing_target_name, true) )
    {
      _no_outgoing_target = false;
      if ( dopt.outgoing_reg )
        pitf->reg_io( this->engine()->get_id(), this->shared_from_this() );

      std::weak_ptr<ijsonrpc> witf = pitf;
      io_id_t io_id = _target_id;
      this->engine()->reg_jsonrpc( _target_id, [witf, io_id]( ::wjrpc::outgoing_holder holder)
      {
        if ( auto pitf1 = witf.lock() )
        {
          pitf1->perform_outgoing( std::move(holder), io_id );
        }
      });
    }
    else if ( auto pitf2 = this->template get_target<iinterface>(dopt.outgoing_target_name, false) )
    {
      _no_outgoing_target = false;
      if ( dopt.outgoing_reg )
        pitf2->reg_io( _target_id /*this->engine()->get_id()*/, this->shared_from_this() );

      std::weak_ptr<iinterface> witf2 = pitf2;
      io_id_t io_id = _target_id;
      this->engine()->reg_io( _target_id, [witf2, io_id]( data_ptr d, io_id_t /*io_id*/, ::wjrpc::output_handler_t handler )
      {
        if ( auto pitf3 = witf2.lock() )
        {
          pitf3->perform_io( std::move(d), io_id, std::move(handler) );
        }

      });
    }
    else
    {
      _no_outgoing_target = true;
    }
  }

  template<typename Tg, typename Handler, typename Req, typename ...Args>
  void call(Req req, Handler cb, Args... args)
  {
    if ( _no_outgoing_target )
    {
      if ( cb != nullptr )
        cb(nullptr);
    }
    else if ( auto e = this->engine() )
    {
      e->template call<Tg>( std::move(req), _target_id, cb, std::forward<Args>(args)... );
    }
  }

private:
  bool _no_outgoing_target = false;
  std::atomic<io_id_t> _target_id;
};


template<typename MethodList, template<typename> class ItfT = interface_implementation>
class gateway
  : public ItfT< gateway_impl< typename MethodList::interface_type, ::wjrpc::handler< ItfT<MethodList> > > >
{
};

}}
