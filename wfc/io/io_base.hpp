#pragma once

#include <wfc/io/tags.hpp>
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

  
  io_base(io_service_type& io_service, const options_type& opt)
    : _io_service(io_service)
    , _options(opt)
  {
  }
    
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

  const owner_type& owner() const
  {
    return *(this->get_aspect().template get<_owner_>());
  }

  owner_type& owner()
  {
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

  wfc::io::callback callback( std::function<void(data_ptr)> handler)
  {
    return this->owner().wrap( [handler, this](data_ptr d)->callback_status 
      {
        auto dd = std::make_shared<data_ptr>( std::move(d) );
        auto fun  = [dd, handler]()
        {
          handler( std::move(*dd) );
        };
        
        this->get_io_service().dispatch( fun );
        
        return callback_status::ready;
      },
      this->options().not_alive
    );
    
    /*
    auto hh = t.owner.wrap( t.strand().wrap()  )
    return [handler](data_ptr d) -> callback_status
    {
      auto pd = std::make_shared<data_ptr>(d);
    }
    */
    
    //return this->callback(*this, handler);
  }
  
  /*
  template<typename Handler>
  std::function<void()>
  wrap(Handler handler)
  {
    return this->wrap(*this, handler);
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
    t.get_aspect().template get<_create_>()(t, _options);
  }
  
  template<typename T>
  void start(T& t)
  {
    t.get_aspect().template get<_start_>()(t);
  }
  
  template<typename T>
  void stop(T& t)
  {
    for ( auto& h : _release_handlers)
      h(t.shared_from_this());
    _release_handlers.clear();
    t.get_aspect().template get<_stop_>()(t);
  }
  
private:
  io_service_type& _io_service;
  options_type _options;
  std::list<release_handler> _release_handlers;
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
