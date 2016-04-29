#pragma once

namespace wfc{
  
enum class component_features
{
  Multiton = 0,
  Singleton = 1,        // нет имени в json-конфиге (enabled)
  Fixed = 2,            // нет флага включения/отключение (enabled)
  Extraordinary = 4,    // нет полей приоритета
  SuspendSupport = 8
};


}
