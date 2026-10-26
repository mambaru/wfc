//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>

#include <wfc/iinterface.hpp>
#include <wfc/core/core_status.hpp>

namespace wfc{

struct wfcglobal;

struct icore: iinterface
{
  virtual ~icore() {}
  virtual int run() = 0;
  virtual void core_reconfigure() = 0;
  virtual void core_stop() = 0;
  virtual void core_abort( const std::string& ) = 0;
  virtual void core_restart() = 0;

  virtual void set_status(core_status, const std::string&) = 0;
  virtual void set_stage(core_stage) = 0;
  typedef std::vector<std::pair<core_status, std::string>> status_list_t;
  virtual core_status get_status(core_stage*, status_list_t* ) = 0;
  virtual std::string get_status_text(size_t errlogs, size_t wrnlogs) = 0;
};

}
