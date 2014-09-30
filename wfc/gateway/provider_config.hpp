#pragma once

#include <cstddef>
#include <ctime>

namespace wfc{ namespace gateway{ 

enum class provider_mode 
{
  notification, // Уведомительный. если нет соединения, то очередь. Остальные без подтверждения (callback(null) всегда на запросы)
  simple,       // если нет соединения, то игнор (callback(null) всегда  всегда на запросы)
  ensured,      // гарантированная доставка (callback(null) по timeout или limit). Все уведомления в запросы
  sequenced     // последовательная отправка (следующий после подтверждения) (callback(null) по timeout или limit). Все уведомления в запросы
};

// перенести в папку  
struct provider_config
{
  bool enabled = true;
  provider_mode mode = provider_mode::simple; // TODO: в конфиг
  bool sequence_mode = true;                  // TODO: удалить
  time_t timeout = 60;                        // TODO: в конфиг.  после timeout (callback(null) для всех)
  size_t queue_limit = 1024*1024;
  size_t queue_warning = 1024*10;
};

}}

