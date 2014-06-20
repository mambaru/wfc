#pragma once


#include <wfc/jsonrpc/handler/ihandler.hpp>
#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/types.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct f_get_methods
{
  std::vector<std::string> result;
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    result.push_back( t.get_aspect().template get<Tg>().name() );
  }
};


template<typename MethodList>
class handler
  : public MethodList
  , public std::enable_shared_from_this< handler<MethodList> >
{
public:
  typedef handler<MethodList> self;
  typedef MethodList super;
  typedef typename super::handler_interface handler_interface;
  typedef typename super::target_type target_type;
  typedef typename super::provider_type provider_type;
  typedef typename super::context_type context_type;
  
  typedef typename super::holder_type holder_type;
  typedef typename super::io_id_t io_id_t;
  typedef typename super::outgoing_handler_t outgoing_handler_t;
  
  handler(target_type trg = target_type(), provider_type prv = provider_type() )
  {
    this->get_aspect().template get<_target_>() = trg;
    this->get_aspect().template get<_provider_>() = prv;
  }

  virtual std::shared_ptr<handler_interface> clone() const 
  {
    return std::make_shared<self>(*this);
  }
  
  virtual std::vector<std::string> get_methods() const
  {
    // TODO: в аспект
    return fas::for_each_group<_method_>(*this, f_get_methods() ).result;
  }

  virtual void invoke(holder_type holder, outgoing_handler_t outgoing_handler) 
  {
    super::get_aspect().template get<_invoke_>()(*this, std::move(holder), std::move(outgoing_handler) );
  }
  
  io_id_t get_io_id() const
  {
    return _io_id;
  }
  
  virtual void start( io_id_t io_id ) 
  {
    _io_id = io_id;
    this->get_aspect().template get<_startup_>()(*this, io_id);
  }
  
  virtual void stop( io_id_t io_id)
  {
    this->get_aspect().template get<_shutdown_>()(*this, io_id);
  }
  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

  provider_type provider() const
  {
    return this->get_aspect().template get<_provider_>();
  }
  
  context_type& context()
  {
    return this->get_aspect().template get<_context_>();
  }

  const context_type& context() const
  {
    return this->get_aspect().template get<_context_>();
  }

private:
  
  io_id_t _io_id;
  
};

}} // wfc

