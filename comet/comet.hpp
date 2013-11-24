#pragma once

#include <comet/core/global.hpp>
#include <memory>
#include <vector>

namespace mamba{ namespace comet{

struct imodule;
struct global;
  
class comet
{
public:
  comet(std::initializer_list< std::pair< std::string, std::shared_ptr<imodule> > > modules );
  void run(int argc, char* argv[]);
private:
  std::shared_ptr<global> _global;
  std::shared_ptr<global::module_registry> _modules;
  std::vector< std::pair< std::string, std::shared_ptr<imodule> > > _module_list;
};

}}
