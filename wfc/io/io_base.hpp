#pragma once

#include <wfc/logger.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io/basic/tags.hpp>
#include <wfc/io/types.hpp>

#include <wfc/callback/callback_owner.hpp>
#include <fas/aop.hpp>
#include <functional>

namespace wfc{ namespace io{ 
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class io_base
  : public AspectClass<A>
  , public iio
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
    std::cout << "io_base::~io_base() " << this->get_id() << std::endl;
  }
  
  io_base(io_service_type& io_service, const options_type& opt)
    : _io_service(io_service)
    , _options(opt)
  {
    _id = create_id();
    /*
    static std::atomic<io_id_t> id_counter(0);
    _id = ++id_counter;
    */
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
    // return this->descriptor().get_io_service();
    //return *(this->get_aspect().template get<_io_service_ptr_>());
  }

  const io_service_type& get_io_service() const
  {
    return _io_service;
    // return this->descriptor().get_io_service();
    //return *(this->get_aspect().template get<_io_service_ptr_>());
  }
  
  strand_type& strand()
  {
    if ( nullptr == this->get_aspect().template get<_strand_>() )
      abort();
    return *(this->get_aspect().template get<_strand_>());
  }

  const strand_type& strand() const
  {
    if ( nullptr == this->get_aspect().template get<_strand_>() )
      abort();
    return *(this->get_aspect().template get<_strand_>());
  }

  // Ахтунг!!! owner только внутри strand(), т.к. нифиг не thread safe
  const owner_type& owner() const
  {
    if ( nullptr == this->get_aspect().template get<_owner_>() )
      abort();
    return *(this->get_aspect().template get<_owner_>());
  }

  owner_type& owner()
  {
    if ( nullptr == this->get_aspect().template get<_owner_>() )
      abort();
    return *(this->get_aspect().template get<_owner_>());
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

  void add_release_handler( release_handler handler)
  {
    _release_handlers.push_back(handler);
  }


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

  ::wfc::io::callback callback( std::function<void(data_ptr)> handler)
  {
    auto wrp = this->strand().wrap( this->owner().wrap( [handler]( std::shared_ptr<data_ptr> dd ){
      std::cout << "callback X " << (*dd!=nullptr) <<std::endl; 
      handler(std::move(*dd));
    }, [](){ std::cout << "NOT ALIVE LEVEL1" << std::endl;}));
    
    auto wrp_ptr = std::make_shared< decltype(wrp) >(wrp);
    
    return /*this->owner().wrap(*/[wrp_ptr](data_ptr d)
    {
      std::cout << "callback Y " << (d!=nullptr) <<std::endl; 
      (*wrp_ptr)( std::make_shared<data_ptr>( std::move(d) ) );
    }/*, 
    [](){ std::cout << "NOT ALIVE LEVEL"2 << std::endl;}
      
    )*/;
    /*
    auto starnd_hanlde = this->strand().wrap([handler]( std::shared_ptr<data_ptr> dd){
      handler( std::move(*dd));
    });
    
    auto strand_ptr = std::make_shared< decltype(starnd_hanlde) >(starnd_hanlde);
    return this->owner().wrap( [strand_ptr](data_ptr d) {
      auto dd = std::make_shared<data_ptr>( std::move(d) );
      (*strand_ptr)(dd);
    },
    this->options().not_alive
    );
    */

    
    /*
    return this->owner().wrap( [handler, this](data_ptr d)
      {
        auto dd = std::make_shared<data_ptr>( std::move(d) );
        auto fun  = [dd, handler]()
        {
          handler( std::move(*dd) );
        };
        
        std::cout << " this->get_io_service().dispatch( fun ) ; " << std::endl;
        this->get_io_service().dispatch( fun );
        
        //return callback_status::ready;
      },
      this->options().not_alive
    );
    */
    
  }
  
  /*
  template<typename Handler>
  std::function<void()>
  wrap(Handler handler)
  {
    return this->wrap(*this, handler);
  }
  */
  
  /*
  startup_handler_t  startup_handler = nullptr;
  shutdown_handler_t shutdown_handler = nullptr;
  */

  /*
  void set_startup_handler( startup_handler_t  startup_handler )
  {
    this->startup_handler = startup_handler;
  }

  void set_shutdown_handler( shutdown_handler_t  shutdown_handler )
  {
    this->shutdown_handler = shutdown_handler;
  }
  */


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

  
  /*
  template<typename T, typename Handler>
  //callback_wrapper<Handler, std::function<void()> > 
  
  std::function<void()> wrap(T& t, Handler handler)
  {
    return t.get_aspect().template get<_wrap_>()(t, handler);
  }
  
  */

  template<typename T>
  void create(T& t)
  {
    t.get_aspect().template gete<_create_>()(t, _options);
  }
  
  /*
  startup_handler_t startup_handler() const 
  {
    return this->get_aspect().template get< basic::_startup_handler_>();
  }
  */

  transfer_handler_t transfer_handler() const 
  {
    return this->get_aspect().template get< basic::_transfer_handler_ >();
  }

  /*
  shutdown_handler_t shutdown_handler() const 
  {
    return this->get_aspect().template get< basic::_shutdown_handler_>();
  }
  */

  template<typename T>
  void start(T& t)
  {
    
    
    auto& sh = _options.startup_handler;
    //auto& sh = t.startup_handler;
    
    if ( sh != nullptr )
    {
      /*
      typedef std::function< void(io_id_t, callback, add_shutdown_handler )> startup_handler_t;
      typedef std::function< void( std::function< void() > ) > add_shutdown_handler;
      typedef std::function< void(io_id_t, callback, add_shutdown_handler )> startup_handler_t;
      typedef std::function< void(io_id_t) > shutdown_handler_t;
      typedef std::function< void(data_ptr, io_id_t, callback )> transfer_handler_t;
      */
      
      
      sh(
        _id, 
        this->transfer_handler(),
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
  void stop(T& t)
  {
    std::cout << "io_base::stop..." << std::endl;
    /*
    for ( auto& h : _release_handlers)
      h(t.shared_from_this());
    _release_handlers.clear();
    */
    t.post([&t, this]()
    {
      std::cout << "io_base::stop post" << std::endl;
      for ( auto& h : _release_handlers2)
        h( this->_id );
      _release_handlers2.clear();
      
      auto& sh = _options.shutdown_handler;
      //auto& sh = t.shutdown_handler;
      if ( sh!=nullptr )
      {
        sh( _id );
      }

      t.get_aspect().template get<_stop_>()(t);
      std::cout << "io_base::stop post Done!" << std::endl;
    });
  }
  
private:
  io_service_type& _io_service;
  options_type _options;
  std::list<release_handler> _release_handlers;
  std::list<std::function<void(io_id_t id)> > _release_handlers2;
  
  io_id_t _id;
};

}}

#include <wfc/io/basic/aspect.hpp>

namespace wfc{ namespace io{ 

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class basic_io
  : public io_base< typename fas::merge_aspect<A, basic::aspect>::type, AspectClass> 
{
public:
  typedef basic_io<A, AspectClass> self;
  typedef io_base< typename fas::merge_aspect<A, basic::aspect>::type, AspectClass> super;
  
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  basic_io(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }
};


}}
