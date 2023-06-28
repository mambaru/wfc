//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/json.hpp>
#include <wfc/statistics/statistics_options.hpp>
#include <wrtstat/wrtstat_options_json.hpp>

namespace wfc{ namespace statistics{

struct statistics_options_json
{
  JSON_NAME(prefixes)
  typedef json::object<
    statistics_options,
    json::member_list<
      json::base< wrtstat::wrtstat_options_json >,
      json::member< n_prefixes, statistics_options, std::vector<std::string>, &statistics_options::prefixes,
                    json::vector_of_strings<> >
     >,
    json::strict_mode
  > meta;

  typedef meta::target target;
  typedef meta::serializer serializer;
  typedef meta::member_list member_list;
};

}}

