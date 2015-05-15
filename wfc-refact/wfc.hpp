//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/asio.hpp>
#include <memory>
#include <vector>

namespace wfc{

struct imodule;
struct global;
  
class wfc
{
public:
  typedef std::shared_ptr<imodule> module_ptr;
  typedef std::pair< std::string, module_ptr > module_pair;
  typedef std::initializer_list< module_pair > modules_list;
  
  wfc(std::string program_version, modules_list modules );
  int run(int argc, char* argv[]);
private:
  std::string _program_version;
  ::wfc::asio::io_service _io_service;
  std::shared_ptr<global> _global;
};

}
