#pragma once

#include <cstddef>
#include <ctime>

#define WFC_PROVIDER_DEFAULT_QUEUE_LIMIT   1048576
#define WFC_PROVIDER_DEFAULT_QUEUE_WARNING 1024
#define WFC_PROVIDER_DEFAULT_MAX_WAIT 128

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
  provider_mode mode = provider_mode::_auto_;
  size_t max_waiting = WFC_PROVIDER_DEFAULT_MAX_WAIT;
  size_t queue_limit = WFC_PROVIDER_DEFAULT_QUEUE_LIMIT;
  time_t wait_timeout_ms = 0;
  size_t queue_warning = WFC_PROVIDER_DEFAULT_QUEUE_WARNING;
};

}}

