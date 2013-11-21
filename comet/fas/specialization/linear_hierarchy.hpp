//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace mamba{ namespace comet{

template<class TL>
class linear_hierarchy
  : public linear_hierarchy< typename type_list_traits<TL>::type >
{
public:
};

template<>
class linear_hierarchy<empty_type>
{
};

}}
