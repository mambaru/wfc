#pragma once


namespace wfc{ namespace io{ namespace acceptor{ namespace loop{

struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    //t.dispatch( [&t]() {
      //typename T::lock_guard lk(t.mutex());
      t.get_aspect().template gete<_on_start_>()(t);
      t.get_aspect().template get<_more_>()(t);
    //});
  }
};

}}}}
