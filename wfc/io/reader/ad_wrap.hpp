#pragma once

namespace wfc{ namespace inet{ namespace async{
  
struct ad_wrap
{
  template<typename T, typename F>
  F operator()(T& t, F callback)
  {
    return callback;
  }
};

}}}
