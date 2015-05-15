#pragma once

namespace wfc{ namespace io{

template<typename TgMain, typename TgSecond>
struct fork
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template get< TgMain >()(t, std::move(d));
    t.get_aspect().template get< TgSecond >()(t);
  }
};

}}
