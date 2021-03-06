//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/domain_options.hpp>

namespace wfc{

/**
 * @brief domain_config
 */
template<typename CustomOptions, typename StatOpt /*=nostat */>
struct domain_config_t
  : domain_options_t<StatOpt>
  , CustomOptions
{
  typedef domain_options_t<StatOpt> domain_options;
  typedef CustomOptions custom_options;
  typedef typename domain_options::stat_options stat_options;
};

}
