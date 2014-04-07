#include <wfc/wfc.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/global.hpp>
#include <iostream>
#include "build_info.h"

namespace wfc{

wfc::wfc(std::string program_version, std::initializer_list< std::pair< std::string, std::shared_ptr<imodule> > > modules )
  : _program_version(program_version)
  , _module_list(modules)
{
  
}

int wfc::run(int argc, char* argv[])
{
  _global = std::make_shared<global>(_io_service);
  global::static_global = _global;
  _modules = std::make_shared<global::module_registry>();
  _loggers = std::make_shared<global::logger_registry>();
  _pubsubs = std::make_shared<global::pubsub_registry>();
  _global->program_version = _program_version;
  _global->wfc_version = wfc_build_info_string;
  _global->modules = _modules;
  _global->loggers = _loggers;
  _global->pubsubs = _pubsubs;
  
  for (auto m: _module_list)
  {
    //std::cout << m.second.use_count() << std::endl;
    _modules->set(m.first, m.second);
  }
  
  /*
  _modules->for_each([this](const std::string& name, std::weak_ptr<imodule> module)
  {
    if (auto m = module.lock())
    {
      std::cout << m.use_count() << std::endl;
    }
  });*/

   
  _modules->for_each([this](const std::string& name, std::weak_ptr<imodule> module)
  {
    if (auto m = module.lock())
      m->create(name,  _global);
  });

  
  if ( auto startup = _global->startup.lock() )
  {
    if ( !startup->startup(argc, argv) )
      return 0;
  }
  else
  {
    std::cerr << "no startup module" << std::endl;
  }
  
  int status = 0;

  if ( auto core = _global->core.lock() )
    status = core->run(_global);
  
  std::cout << "wfc::run finalize ... " << std::endl;
  /*
  _pubsubs->clear();
  _pubsubs.reset();
  
  _loggers.reset();
  
  _module_list.clear();
  _modules->for_each([this](const std::string& name, std::weak_ptr<imodule> module)
  {
    if (auto m = module.lock())
    {
      std::cout << m.use_count() << std::endl;;
    }
  });

  _modules->clear();
  _modules.reset();
  
  global::static_global.reset();
  _global.reset();
  */
  std::cout << "...wfc::run finalize" << std::endl;
  return status;
}

}
