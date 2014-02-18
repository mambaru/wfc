#pragma once

namespace wfc{ namespace io{ namespace async{
  
struct ad_wrap
{
  template<typename T, typename F>
  F operator()(T& t, F callback)
  {
    if ( auto strand = t.get_aspect().template get<_strand_>() )
    {
      return strand->wrap( this->owner_wrap(t, callback));
      
    }
    else
    {
      return this->owner_wrap(t, callback);
    }
  }

private:
  
  template<typename T, typename F>
  F owner_wrap(T& t, F callback)
  {
    t.get_aspect().template get<_owner_>()->wrap(
        callback, 
        [&t]() 
        { 
          t.get_aspect().template <_not_alive_>()(t);
        }
    );
  }
};

}}}
