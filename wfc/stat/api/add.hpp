#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/types.hpp>

namespace wfc{ namespace btp{

namespace request
{
  struct add
  {
    typedef ::wrtstat::aggregated_data aggregated;
    typedef aggregated::data_type data_type;
    typedef ::wrtstat::types::time_type time_type;
    typedef ::wrtstat::types::data_ptr  data_ptr;

    time_type ts = 0;
    std::string name;
    aggregated ag;
    data_type cl;

    typedef std::unique_ptr<add> ptr;
  };
}

namespace response
{
  struct add
  {
    bool result = true;
    typedef std::unique_ptr<add> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}}
