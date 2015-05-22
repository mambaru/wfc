//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/ibuild_info.hpp>
#include <wfc/asio.hpp>
#include <memory>
#include <list>

namespace wfc{

struct ipackage;
struct ibuild_info;
struct wfcglobal;

class wfc
{
public:
  typedef std::shared_ptr<ipackage> package_ptr;
  typedef std::list<package_ptr> package_list;

  wfc(std::shared_ptr<ibuild_info> bi, package_list packages);
  int run(int argc, char* argv[]);
private:
  ::wfc::asio::io_service _io_service;
  std::shared_ptr<wfcglobal> _global;
  package_list _packages;
};

}
