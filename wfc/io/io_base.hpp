#pragma once

#include <wfc/logger.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io/basic/tags.hpp>
#include <wfc/io/types.hpp>
#include <wfc/io_service.hpp>

#include <wfc/callback/callback_owner.hpp>
#include <fas/aop.hpp>
#include <functional>

#include <mutex>              
#include <condition_variable>

namespace wfc{ namespace io{ 
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class io_base
  : public AspectClass<A>
  // , public iio
{
public:
  
  typedef io_base<A, AspectClass> self;
  typedef AspectClass<A> super;
  
  typedef typename super::aspect::template advice_cast<_context_>::type context_type;
  typedef typename super::aspect::template advice_cast<_data_type_>::type data_type;
  typedef typename super::aspect::template advice_cast<_strand_type_>::type strand_type;
  typedef typename super::aspect::template advice_cast<_owner_type_>::type owner_type;
  typedef typename super::aspect::template advice_cast<_options_type_>::type options_type;
  typedef typename super::aspect::template advice_cast<_io_service_type_>::type io_service_type;
  typedef std::unique_ptr<data_type> data_ptr;

  ~io_base()
  {
  }
  
  io_base(io_service_type& io_service, const options_type& opt )
    : _io_service(io_service)
    , _options(opt)
    , _stop_flag(ATOMIC_FLAG_INIT)
  {
    _id = create_id();
    this->get_aspect().template get< basic::_transfer_handler_ >() = opt.outgoing_handler;
  }
    
  io_id_t get_id() const { return _id;}
  
  context_type& context()
  {
    return this->get_aspect().template get<_context_>();
  }

  const context_type& context() const
  {
    return this->get_aspect().template get<_context_>();
  }

  io_service_type& get_io_service()
  {
    return _io_service;
  }

  const io_service_type& get_io_service() const
  {
    return _io_service;
  }
  
  strand_type& strand()
  {
    if ( auto ptr = this->get_aspect().template get<_strand_>() )
    {
      return *(ptr.get());
    }
    abort();
    /*
    if ( nullptr == this->get_aspect().template get<_strand_>() )
      abort();
    return *(this->get_aspect().template get<_strand_>());
    */
  }

  const strand_type& strand() const
  {
    /*
    if ( nullptr == this->get_aspect().template get<_strand_>() )
      abort();
    return *(this->get_aspect().template get<_strand_>());
    */
    if ( auto ptr = this->get_aspect().template get<_strand_>() )
    {
      return *(ptr.get());
    }
    abort();

  }

  // Ахтунг!!! owner только внутри strand(), т.к. нифиг не thread safe
  const owner_type& owner() const
  {
    /*
    if ( nullptr == this->get_aspect().template get<_owner_>() )
      abort();
    return *(this->get_aspect().template get<_owner_>());
    */
    if ( auto ptr = this->get_aspect().template get<_owner_>() )
    {
      return *(ptr.get());
    }
    abort();

  }

  owner_type& owner()
  {
    /*
    if ( nullptr == this->get_aspect().template get<_owner_>() )
      abort();
    return *(this->get_aspect().template get<_owner_>());
    */
    if ( auto ptr = this->get_aspect().template get<_owner_>() )
    {
      return *(ptr.get());
    }
    abort();

  }

  const options_type& options() const
  {
    return _options;
  }

  void options(const options_type& opt)
  {
    _options = opt;
  }

///  //////////////////////////////////////////////

/*
  void add_release_handler( release_handler handler)
  {
    _release_handlers.push_back(handler);
  }
  */


  template<typename Handler>
  void dispatch(Handler h)
  {
    this->dispatch(*this, h);
  }

  template<typename Handler>
  void post(Handler h)
  {
    this->post(*this, h);
  }


  
  ::wfc::io::outgoing_handler_t callback( std::function<void(data_ptr)> handler)
  {
    auto wrp = this->owner().wrap( this->strand().wrap( [handler]( std::shared_ptr<data_ptr> dd ){
      
      handler(std::move(*dd));
    }), 
    [](){ 
    }
    );
    
    auto wrp_ptr = std::make_shared< decltype(wrp) >(wrp);
    
    return /*this->owner().wrap(*/[wrp_ptr](data_ptr d)
    {
      (*wrp_ptr)( std::make_shared<data_ptr>( std::move(d) ) );
    };
  }
  
protected:

  template<typename T, typename Handler>
  void dispatch(T& t, Handler h)
  {
    this->get_aspect().template get<_dispatch_>()(t, h);
  }

  template<typename T, typename Handler>
  void post(T& t, Handler h)
  {
    this->get_aspect().template get<_post_>()(t, h);
  }

  template<typename T>
  void create(T& t)
  {
    t.get_aspect().template gete<_create_>()(t, _options);
  }

  
  outgoing_handler_t outgoing_handler() const 
  {
    const auto& handler = this->get_aspect().template get< basic::_transfer_handler_ >();
    if (handler == nullptr)
    {
      //std::function<void(data_ptr)> tmp = [](data_ptr)->void {};
      // ахтунг! self::data_type!= ::wfc::io::data_type;
      return nullptr;
    }
    return handler;
  }
  
  void outgoing_handler(outgoing_handler_t handler) 
  {
    if ( handler == nullptr )
      return;
      
    auto& prev = this->get_aspect().template get< basic::_transfer_handler_ >();
    prev = handler;
  }
  

  template<typename T>
  void start(T& t)
  {
    _stop_flag.clear();
    
    auto& sh = _options.startup_handler;
    
    if ( sh != nullptr )
    {
      sh(
        _id, 
        this->outgoing_handler(),
        //this->options().outgoing_handler,
        [&t, this]( std::function<void(io_id_t id)> release_fun ) 
        {
          t.dispatch( [this, release_fun]()
          {
            this->_release_handlers2.push_back(release_fun);
          });
        }
      );
    }
    
    // Сначала запускаем startup_handler (иначе в mt данные могут прийти раньше )
    t.get_aspect().template get<_start_>()(t);
  }
  
  template<typename T>
  void self_stop(T& t, std::function<void()> finalize)
  {
    if ( !_stop_flag.test_and_set() )
    {
      
      t.get_aspect().template gete<_before_stop_>()(t);
      
      for ( auto& h : this->_release_handlers2)
      {
        h( this->_id );
      }
      
      this->_release_handlers2.clear();
      
      auto& sh = this->_options.shutdown_handler;
      if ( sh != nullptr )
      {
        sh( this->_id );
      }
        
      if ( finalize!=nullptr )
      {
        finalize();
      }
      
      t.get_aspect().template gete<_after_stop_>()(t);
    }
  }
  
  template<typename T>
  void stop(T& t, std::function<void()> finalize)
  {
    if ( !_stop_flag.test_and_set() )
    {
      t.get_aspect().template gete<_before_stop_>()(t);

      if ( finalize!=nullptr )
      {
        finalize();
      }

      t.get_aspect().template gete<_after_stop_>()(t);

    }

    /*
    _io_service.post( this->strand().wrap([&t, finalize]()
    {
      t.self_stop(t, finalize);
    }));
    */
  }
  
  
private:
  io_service_type& _io_service;
  options_type _options;
  //std::list<release_handler> _release_handlers;
  std::list<std::function<void(io_id_t id)> > _release_handlers2;
  
  io_id_t _id;
  std::atomic_flag _stop_flag;
  
};

}}
