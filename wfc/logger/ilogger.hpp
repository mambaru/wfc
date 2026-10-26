//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <wlog/logger_fun.hpp>
#include <wlog/logger/basic_logger_options.hpp>

#include <string>

namespace wfc{
  
struct ilogger: iinterface
{
  virtual ~ilogger(){}
  typedef wlog::formatter_fun formatter_fun;
  typedef wlog::writer_fun writer_fun;
  typedef wlog::basic_logger_options options_type;
  typedef std::unique_ptr<options_type> options_ptr;

  virtual options_ptr options() { return nullptr; }
  
  virtual formatter_fun file_formatter()   { return nullptr;}
  virtual formatter_fun stdout_formatter() { return nullptr;}
  virtual formatter_fun syslog_formatter() { return nullptr;}
  
  virtual writer_fun file_writer()   { return nullptr;}
  virtual writer_fun stdout_writer() { return nullptr;}
  virtual writer_fun syslog_writer() { return nullptr;}
  
};

}
