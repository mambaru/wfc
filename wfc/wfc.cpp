#include <wfc/wfc.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/global.hpp>
#include <iostream>
#include "build_info.h"

namespace wfc{

wfc::wfc(std::string program_version, std::initializer_list< std::pair< std::string, std::shared_ptr<imodule> > > modules )
  : _program_version(program_version)
  , _global( std::make_shared<global>() )
  , _module_list(modules)
{
}

int wfc::run(int argc, char* argv[])
{
  _modules = std::make_shared<global::module_registry>();
  _loggers = std::make_shared<global::logger_registry>();
  _global->program_version = _program_version;
  _global->wfc_version = wfc_build_info_string;
  _global->modules = _modules;
  _global->loggers = _loggers;
  
  for (auto m: _module_list)
  {
    _modules->set(m.first, m.second);
  }
   
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
    std::cout << "no startup module" << std::endl;

  if ( auto core = _global->core.lock() )
    return core->run(/*argc, argv, */_global);
  return 0;
}

}
