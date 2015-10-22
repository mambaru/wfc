//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <string>

namespace wfc{

struct ibuild_info
{
  virtual ~ibuild_info(){}
  virtual bool enabled() = 0;
  virtual std::string name() = 0;
  virtual std::string version() = 0;
  virtual std::string verex() = 0;
  virtual std::string build_type() = 0;
  virtual std::string build_date() = 0;
  virtual std::string build_flags() = 0;
  virtual std::string build_count() = 0;
  virtual std::string branch() = 0;
  virtual std::string commit() = 0;
  virtual std::string commit_message() = 0;
  virtual std::string commit_date() = 0;
  virtual std::string commit_author() = 0;
  virtual std::string initial_author() = 0;
  virtual std::string all_authors() = 0;
};

}
