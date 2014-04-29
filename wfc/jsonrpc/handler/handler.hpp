#pragma once


#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct f_invoke
{
  incoming_holder& holder;
  ::wfc::io::callback& callback;
  bool founded;
  
  f_invoke(incoming_holder& holder, ::wfc::io::callback& callback)
    : holder( holder )
    , callback(callback)
    , founded(false)
  {
  }
  
  operator bool () const
  {
    return founded;
  }
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if ( !founded && holder.$() )
    {
      if ( holder.method( t.get_aspect().template get<Tg>().name() ) )
      {
        t.get_aspect().template get<Tg>()(t, std::move(holder), callback );
        founded = true;
      }
    }
  }
};

struct f_get_methods
{
  std::vector<std::string> result;
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    result.push_back( t.get_aspect().template get<Tg>().name() );
  }
};


template<typename Instanse>
class handler
  : public Instanse
  , public std::enable_shared_from_this< handler<Instanse> >
{
public:
  typedef handler<Instanse> self;
  typedef Instanse super;
  typedef typename Instanse::target_type target_type;
  typedef typename Instanse::provider_type provider_type;
  
  handler(target_type trg = target_type(), provider_type prv = provider_type() )
  {
    this->get_aspect().template get<_target_>() = trg;
    this->get_aspect().template get<_provider_>() = prv;
    
    {
      auto ptr = this->get_aspect().template get<_provider_>();
      if (auto p = ptr.lock() )
      {
        std::cout << "PROVIDER READY" << std::endl;
      }
      else
      {
        std::cout << "PROVIDER NOT READY" << std::endl;
      }
    }
  }

  virtual std::shared_ptr<handler_base> clone(/*outgoing_request_handler_t request_handler*/) const 
  {
    std::cout << "handler::clone()" << std::endl;
    {
      auto ptr = this->get_aspect().template get<_provider_>();
      if (auto p = ptr.lock() )
      {
        std::cout << "PROVIDER READY" << std::endl;
      }
      else
      {
        std::cout << "PROVIDER NOT READY" << std::endl;
      }
    }
    
    return std::make_shared<self>(*this);
  }
  
  virtual std::vector<std::string> get_methods() const
  {
    return fas::for_each_group<_method_>(*this, f_get_methods() ).result;
  }

  virtual void process(incoming_holder holder, ::wfc::io::callback callback) 
  {
    if ( !fas::for_each_group<_method_>(*this, f_invoke( holder, callback ) ) )
    {
      // В аспект!
      typedef outgoing_error_json< error_json::type >::type json_type;
      outgoing_error<error> error_message;
      error_message.error = std::make_unique<error>(procedure_not_found());
      error_message.id = std::move( holder.raw_id() );
              
      auto d = std::make_unique< ::wfc::io::data_type>();
      typename json_type::serializer()(error_message, std::inserter( *d, d->end() ));
      callback( std::move(d) );
    };
  }
  
  ::wfc::io::io_id_t get_id() const
  {
    return _id;
  }
  
  virtual void start(::wfc::io::io_id_t id) 
  {
    _id = id;
    this->get_aspect().template get<_startup_>()(*this, id);
  }
  
  virtual void stop(::wfc::io::io_id_t id)
  {
    this->get_aspect().template get<_shutdown_>()(*this, id);
  }

  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

  provider_type provider() const
  {
    return this->get_aspect().template get<_provider_>();
  }

private:
  ::wfc::io::io_id_t _id;
};

}} // wfc


