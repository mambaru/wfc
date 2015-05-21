//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/asio.hpp>
#include <memory>
#include <list>

namespace wfc{

struct ipackage;
struct wfcglobal;

class wfc
{
public:
  typedef std::shared_ptr<ipackage> package_ptr;
  typedef std::list<package_ptr> package_list;

  wfc(std::string program_version, package_list packages);
  int run(int argc, char* argv[]);
private:
  std::string _program_version;
  ::wfc::asio::io_service _io_service;
  std::shared_ptr<wfcglobal> _global;
  package_list _packages;
};

}
