#pragma once
#include <wfc/callback/callback_owner.hpp>

namespace wfc{ namespace io{ namespace async{

struct ad_create
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_owner_>() = std::make_shared<callback_owner>();
  }
};

}}}
