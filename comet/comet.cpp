#include <comet/comet.hpp>
#include <comet/core/imodule.hpp>
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

void comet::run(int argc, char* argv[])
{
  _modules = std::make_shared<global::module_registry>();
  _loggers = std::make_shared<global::logger_registry>();
  _global->program_version = _program_version;
  _global->comet_version = comet_build_info;
  _global->modules = _modules;
  _global->loggers = _loggers;
  for (auto m: _module_list)
  {
    if ( _modules->insert(m.first, m.second) )
      m.second->create(_global);
    else
      std::cerr << "Warning! module name '" << m.first << "' is not unique. Ignored." << std::endl;
  }

  if ( auto core = _global->core.lock() )
  {
    // std::cout << "before run" << std::endl;
    core->run(argc, argv, _global);
    //std::cout << "after run" << std::endl;
  }
}

}}
