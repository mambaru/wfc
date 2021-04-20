//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wflow/mutex.hpp>

namespace wfc{ 

using wflow::empty_mutex;
using wflow::rwlock;
using wflow::read_lock;
using wflow::spinlock;
using wflow::longspinlock;
using wflow::spinlock_n;

}
