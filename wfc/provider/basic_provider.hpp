#pragma once

#include <wfc/provider/provider_config.hpp>
#include <wfc/provider/provider_base.hpp>
#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>
#include <wfc/logger.hpp>
#include <memory>
#include <map>
#include <list>

namespace wfc{ namespace provider{ 


template<typename Itf, template<typename> class Derived, typename Mutex /*= ::wfc::spinlock std::mutex*/  >  
class basic_provider
  : public provider_base<Itf, Mutex>
  , public std::enable_shared_from_this< Derived<Itf> >
{
  
public:
  
  typedef provider_base<Itf, Mutex> super;
  
  basic_provider(const provider_config& conf)
    : super(conf)
  {}
};

}}
