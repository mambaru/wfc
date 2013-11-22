#include <comet/comet.hpp>
#include <comet/core/imodule.hpp>
#include <comet/core/icore.hpp>
#include <comet/core/global.hpp>
#include <iostream>


namespace mamba{ namespace comet{

comet::comet(std::initializer_list< std::pair< std::string, std::shared_ptr<imodule> > > modules )
  : _global( std::make_shared<global>() )
  , _modules(modules)
{
}

void comet::run(int argc, char* argv[])
{
  for (auto m: _modules)
  {
    if ( _global->modules.insert(m.first, m.second) )
      m.second->create(_global);
    else
      std::cerr << "Warning! module name '" << m.first << "' is not unique. Ignored." << std::endl;
  }

  if ( auto core = _global->core.lock() )
  {
    std::cout << "before run" << std::endl;
    core->run(argc, argv, _global);
    std::cout << "after run" << std::endl;
  }
}

}}
