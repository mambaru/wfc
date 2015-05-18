#pragma once

namespace wfc{

template<typename ObjBase>  
struct object_options: ObjBase
{
  bool enabled = true;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
};
  
}
