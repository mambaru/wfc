#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class simple_provider
  : public basic_provider<Itf, simple_provider>
  
{
  typedef basic_provider<Itf, simple_provider> super;
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
public:

  simple_provider(const provider_config& conf)
    : super(conf)
    , _drop_count(0)
  {
  }
  
  size_t drop_count() const 
  {
    return _drop_count;
  }
  
  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    super::startup_(client_id, std::move(ptr) );
    if ( _current_drop_count!=0 )
    {
      _current_drop_count = 0;
      DAEMON_LOG_WARNING("provider(simple): Подключен новый получатель. Потеряно запросов: " << _current_drop_count << "(" << _drop_count << " всего)")
    }
  }

  template<typename Req, typename Callback, typename... Args>
  void post( 
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    if ( auto cli = super::get() )
    {
      (cli.get()->*mem_ptr)( std::move(req), std::move(callback), std::forward<Args>(args)... );
    }
    else
    {
      ++_current_drop_count;
      ++_drop_count;
    }
  }
  
private:
  std::atomic<size_t> _drop_count;
  std::atomic<size_t> _current_drop_count;

};

}}
