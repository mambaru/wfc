#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{

struct ad_create
{
  template<typename T>
  void operator()(T& t)
  {
    typedef typename T::aspect::template advice_cast<_owner_type_>::type owner_type;
    t.get_aspect().template get<_owner_>() = std::make_shared<owner_type>();
  }
};

}}}
