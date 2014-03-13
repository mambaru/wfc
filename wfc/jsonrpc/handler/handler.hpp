#pragma once


#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <wfc/jsonrpc/handler/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct f_invoke
{
  incoming_holder& holder;
  ::wfc::io::callback& callback;
  
  f_invoke(incoming_holder& holder, ::wfc::io::callback& callback)
    : holder( holder )
    , callback(callback)
  {
  }
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if ( holder.$() )
    {
      if ( holder.method( t.get_aspect().template get<Tg>().name() ) )
      {
        t.get_aspect().template get<Tg>()(t, std::move(holder), callback );
      }
    }
  }
};

template<typename Instanse>
class handler
  : public Instanse
{
public:
  typedef handler<Instanse> self;
  typedef Instanse super;
  typedef typename Instanse::target_type target_type;
  
  handler(target_type trg = target_type() )
  {
    this->get_aspect().template get<_target_>() = trg;
  }

  virtual std::shared_ptr<handler_base> clone(/*outgoing_request_handler_t request_handler*/) const 
  {
    return std::make_shared<self>(*this);
  }

  virtual void process(incoming_holder holder, ::wfc::io::callback callback) const
  {
    
    fas::for_each_group<_method_>(*this, f_invoke( holder, callback ) );
  }
  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

};

}} // wfc


