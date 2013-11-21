//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace mamba{ namespace comet{

template<int> struct static_check;

template<>
struct static_check<true>
{
  enum { result = 1 };
};

}}
