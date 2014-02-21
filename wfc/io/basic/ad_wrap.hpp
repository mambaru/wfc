#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{
  
struct ad_wrap
{
  template<typename T, typename F>
  std::function<void()> operator()(T& t, F callback)
  {
    if ( auto strand = t.get_aspect().template get<_strand_>() )
    {
      return this->owner_wrap(t, strand_wrap(strand, callback) );
    }
    else
    {
      return this->owner_wrap(t, callback);
    }
  }

private:

  template<typename StrandWrp>
  std::shared_ptr<StrandWrp> make_wrap_ptr(StrandWrp&& wrp)
  {
    return std::make_shared<StrandWrp>( std::move(wrp) );
  }
  
  template<typename Strand, typename Callback>
  std::function<void()> strand_wrap(Strand& strand, Callback callback)
  {
    auto ptr = this->make_wrap_ptr( std::move( strand->wrap(callback) ) );
    return [ptr]()
    { 
      (*ptr)();
    };
  }

  template<typename T, typename F>
  std::function<void()> owner_wrap(T& t, F callback)
  {
    return t.get_aspect().template get<_owner_>()->wrap(
        callback, 
        t.get_aspect().template get<_not_alive_>()
    );
  
  }
  
};

}}}
