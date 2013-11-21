#pragma once

#include <comet/core/registry.hpp>
#include <memory>

namespace mamba{ namespace comet{

class imodule;
class icore;

struct global
{
  std::weak_ptr<icore> core;
  registry<imodule> modules;
};

}}
