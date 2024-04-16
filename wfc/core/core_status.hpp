//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2024
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wfc{

enum class core_status
{
  OK,
  WARINING,
  ERROR,
  ABORT,
  HANG //  Зависание одного из потоков
};

enum class core_stage
{
  UNDEFINED,
  CONFIGURING,
  INITIALIZING,
  STARTING,
  IS_RUN,
  STOPPING
};

}
