#pragma once

namespace wfc{
  
struct component_features
{
  typedef enum {
    Multiton = 0,
    Singleton = 1,        // нет имени в json-конфиге (enabled)
    Fixed = 2,            // нет флага включения/отключение (enabled)
    Extraordinary = 4,    // нет полей приоритета
    DisableSuspend  = 8,
    CommonWorkflow  = 16,
    Workflow1  = 32
  } type;
};


}
