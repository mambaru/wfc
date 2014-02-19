#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{
  
struct ad_wrap
{
  template<typename StrandWrp>
  std::shared_ptr<StrandWrp> create(StrandWrp&& wrp)
  {
    return std::make_shared<StrandWrp>( std::move(wrp) );
  }
  
  template<typename Strand, typename Callback>
  std::function<void()> rewrap(Strand& strand, Callback callback)
  {
    auto ptr = this->create( std::move( strand->wrap(callback) ) );
    return [ptr](){ (*ptr)();};
  }
  
  template<typename T, typename F>
  std::function<void()> operator()(T& t, F callback)
  {
    
    if ( auto strand = t.get_aspect().template get<_strand_>() )
    {
      return this->owner_wrap(t, rewrap(strand, callback) );
      
      //auto xxx = strand->wrap(callback);
      //return this->owner_wrap(t, callback);
      //return this->owner_wrap(t, strand->wrap(callback) );
      //return strand->wrap( this->owner_wrap(t, callback));
      //return strand->wrap(callback);
    }
    else
    {
      return this->owner_wrap(t, callback);
    }
  }

private:
  
  template<typename T, typename F>
  std::function<void()> owner_wrap(T& t, F callback)
  {
    return t.get_aspect().template get<_owner_>()->wrap(
        callback, 
        [&t]() 
        { 
          std::cout << "_not_alive_" << std::endl;
          //t.get_aspect().template gete<_not_alive_>()(t);
        }
    );
  
  }
  
};

}}}
