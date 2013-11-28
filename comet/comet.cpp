#include <comet/comet.hpp>
#include <comet/module/imodule.hpp>
#include <comet/core/icore.hpp>
#include <comet/core/global.hpp>
#include <iostream>
#include "build_info.h"

namespace mamba{ namespace comet{

comet::comet(std::string program_version, std::initializer_list< std::pair< std::string, std::shared_ptr<imodule> > > modules )
  : _program_version(program_version)
  , _global( std::make_shared<global>() )
  , _module_list(modules)
{
}

int comet::run(int argc, char* argv[])
{
  _modules = std::make_shared<global::module_registry>();
  _loggers = std::make_shared<global::logger_registry>();
  _global->program_version = _program_version;
  _global->comet_version = comet_build_info_string;
  _global->modules = _modules;
  _global->loggers = _loggers;
  for (auto m: _module_list)
  {
    _modules->set(m.first, m.second);
    //m.second->create(_global);
    // else
      //std::cerr << "Warning! module name '" << m.first << "' is not unique. Ignored." << std::endl;
  }
  _modules->for_each([this](const std::string& name, std::weak_ptr<imodule> module){
    if (auto m = module.lock())
      m->create(name,  _global);
  });

  

  if ( auto core = _global->core.lock() )
    return core->run(argc, argv, _global);
  return 0;
}

}}
