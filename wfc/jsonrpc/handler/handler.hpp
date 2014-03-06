#pragma once


#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <wfc/jsonrpc/handler/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct f_invoke
{
  incoming_holder& holder;
  
  f_invoke(incoming_holder& holder)
    : holder( holder )
  {
  }
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if ( holder.$() )
    {
      if ( holder.method( t.get_aspect().template get<Tg>().name() ) )
      {
        t.get_aspect().template get<Tg>()(t, std::move(holder) );
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

  virtual std::shared_ptr<handler_base> clone()
  {
    std::cout << "CLONE!!!" << std::endl;
    return std::make_shared<self>(*this);
  }

  virtual void process(incoming_holder holder)
  {
    std::cout << "instance process!!!" << std::endl;
    fas::for_each_group<_method_>(*this, f_invoke( holder ) );
  }
  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

};

}} // wfc


