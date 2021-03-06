//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/build_info.hpp>
#include <wfc/asio.hpp>
#include <memory>
#include <list>

namespace wfc{

struct ipackage;
struct ibuild_info;
struct wfcglobal;

class basic_wfc
{
public:
  typedef std::shared_ptr<ipackage> package_ptr;
  typedef std::list<package_ptr> package_list;

  virtual ~basic_wfc();
  basic_wfc(std::shared_ptr<ibuild_info> bi, const package_list& packages);
  int run(int argc, char* argv[], std::string helpstring = std::string() );

  void add_package(package_ptr ppack);
  void add_packages(const package_list& packages);

private:
  boost::asio::io_context _io_context;
  std::shared_ptr<wfcglobal> _global;
  package_list _packages;
};

template<typename BuildInfo>
class wfc: public basic_wfc
{
public:
  explicit wfc(const basic_wfc::package_list& packages)
    : basic_wfc(::wfc::make_build_info<BuildInfo>(), packages)
  {}
};

}
