#pragma once

namespace wfc{
  
struct component_features
{
  typedef enum {
    Defaults = 0,
    Multiton = 0,
    Singleton = 1,         // нет имени в json-конфиге (enabled)
    DisabledEnabled = 2,   // Fixed. нет флага включения/отключение (enabled)
    DisabledPriority = 4,  // Extraordinary. нет полей приоритета
    DisabledSuspend  = 8,  // DisableSuspend
    DisabledWorkflow  = 16/*, // CommonWorkflow
    DisabledStatistics  = 32 // CommonWorkflow*/
  } type;
};


}
