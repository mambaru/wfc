#pragma once

#include <wfc/gateway/provider/sequenced_provider.hpp>
#include <wfc/gateway/provider/simple_provider.hpp>
#include <wfc/gateway/provider_config.hpp>

#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>

#include <fas/typemanip.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class provider
  //: public iprovider<Itf>
{
public:
  typedef Itf interface_type;
  typedef std::shared_ptr<interface_type> interface_ptr;
  
  //typedef ::wfc::spinlock basic_mutex_type;
  typedef std::recursive_mutex basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;

  
  typedef simple_provider<interface_type> simple_provider_t;
  typedef sequenced_provider<interface_type> sequenced_provider_t;
  
  typedef std::shared_ptr<simple_provider_t> simple_provider_ptr;
  typedef std::shared_ptr<sequenced_provider_t> sequenced_provider_ptr;
  
  typedef iprovider<Itf> provider_base_type;
  typedef provider_base_type* provider_base_ptr;

  provider(const provider_config& conf)
    : _conf(conf)
    , _simple( std::make_shared<simple_provider_t>(conf) )
    , _sequenced( std::make_shared<sequenced_provider_t>(conf) )
  {
  }
  
  sequenced_provider_ptr get_sequenced() const 
  {
    return _sequenced;
  }
  
  void reconfigure(const provider_config& conf)
  {
    std::lock_guard<mutex_type> lk(_mutex); 
    _conf = conf;
    this->reconfigure_(fas::type2type<simple_provider_t>() );
    this->reconfigure_(fas::type2type<sequenced_provider_t>() );
  }
  
  interface_ptr get(size_t& client_id) const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return this->base_()->get();
  }

  interface_ptr get() const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return this->base_()->get();
  }

  
  void startup(size_t clinet_id, interface_ptr ptr ) 
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    this->base_()->startup(clinet_id, std::move(ptr) );
  }
  
  size_t ready_count() const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return this->base_()->ready_count();
  }

  void shutdown(size_t clinet_id) 
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    this->base_()->shutdown(clinet_id);
  }
  
  template<typename Req, typename Callback, typename... Args>
  void post( 
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    wfc::read_lock<mutex_type> lk(_mutex);
    switch (_conf.mode)
    {
      case provider_mode::notification: 
        abort();
        break;
      case provider_mode::simple:  
        _simple->post(mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)... );
        break;
      case provider_mode::ensured: 
        abort();
        break;
      case provider_mode::sequenced: 
        _sequenced->post(mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)... );
        break;
    }
  }



/*  
  typedef provider_base<Itf> abstract_type;
  typedef std::shared_ptr<abstract_type> abstract_ptr;
  
  typedef sequence_provider<Itf> sequence_type;
  typedef simple_provider<Itf>   simple_type;
  
  typedef Itf interface_type;
  
  provider(const provider_config& conf)
  {
    if ( conf.sequence_mode  )
    {
      _impl = std::make_shared<sequence_type>(conf);
    }
    else
    {
      _impl = std::make_shared<simple_type>(conf);
    }
  }
  
  void startup(size_t clinet_id, std::shared_ptr<interface_type> ptr ) 
  {
    _impl->startup(clinet_id, ptr );
  }

  void shutdown(size_t clinet_id) 
  {
    _impl->shutdown(clinet_id);
  }

  
private:
  
  abstract_ptr _impl;
  */
private:

  void reconfigure_(fas::type2type<simple_provider_t> )
  {
    // TODO: пересоздать объект если необходимо
    this->base_()->reconfigure(_conf);
  }

  void reconfigure_(fas::type2type<sequenced_provider_t> )
  {
    // TODO: пересоздать объект если необходимо
    this->base_()->reconfigure(_conf);
  }
  
  provider_base_ptr base_() const
  {
    switch (_conf.mode)
    {
      case provider_mode::notification: 
        abort();
      case provider_mode::simple:  
        return _simple.get();
      case provider_mode::ensured: 
        abort();
        break;
      case provider_mode::sequenced: 
        return _sequenced.get();
    }
    abort();
  }
  
  
  /*
      void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(size_t, Resp)> callback, 
    Args... args
    */


private:
  mutable mutex_type _mutex;
  provider_config _conf;
  simple_provider_ptr _simple;
  sequenced_provider_ptr _sequenced;
};

}}
