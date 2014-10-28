#pragma once

#include <cstddef>
#include <ctime>

namespace wfc{ namespace provider{ 

enum class provider_mode 
{
  _auto_,
  simple,       // если нет соединения, то игнор (callback(null) всегда  всегда на запросы)
  connectify,   // Уведомительный. если нет соединения, то очередь. Остальные без подтверждения (callback(null) всегда на запросы)
  insured,      // гарантированная доставка (callback(null) по timeout или limit). Все уведомления в запросы
  sequenced     // последовательная отправка (следующий после подтверждения) (callback(null) по timeout или limit). Все уведомления в запросы
};

// перенести в папку  
// wait_limit=1, queue_limit=N последовательный режим
// wait_limit=N, queue_limit=N надежный режим
// wait_limit=0, queue_limit=0 прокси режим
// wait_limit=N, queue_limit=0 запрещено
// wait_limit=0, queue_limit=N без гарантий (копим очередь queue, только если нет ни одного подключения)
// TODO: mode оставляем, если не defualt, то правим wait_limit и queue_limit в зависимости от режима

struct provider_config
{
  bool enabled = true;
  provider_mode mode = provider_mode::_auto_; // TODO: в конфиг
  size_t wait_limit = 1;    // 1 - sequenced моде, 0 - надежно без гаранитии доставки
  size_t queue_limit = 1024; // 0 - 

  time_t wait_timeout_ms = 0;                        // TODO: в конфиг.  после timeout (callback(null) для всех)

  size_t wait_warning = 0;
  size_t queue_warning = 1024;
};

}}

