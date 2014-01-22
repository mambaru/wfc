#pragma once

#include <wfc/core/global.hpp>
#include <memory>
#include <vector>

namespace wfc{

struct imodule;
struct global;
  
class wfc
{
public:
  wfc(std::string program_version, std::initializer_list< std::pair< std::string, std::shared_ptr<imodule> > > modules );
  int run(int argc, char* argv[]);
private:
  std::string _program_version;
  std::shared_ptr<global> _global;
  std::shared_ptr<global::module_registry> _modules;
  std::shared_ptr<global::logger_registry> _loggers;
  std::shared_ptr<global::pubsub_registry> _pubsubs;  
  std::vector< std::pair< std::string, std::shared_ptr<imodule> > > _module_list;
};

}
