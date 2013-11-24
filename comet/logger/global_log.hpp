#pragma once

#include <comet/core/ilogger.hpp>
#include <comet/core/global.hpp>

#include <string>
#include <memory>

namespace mamba{ namespace comet{

class global_log
{
public:
  
  void set_registry(std::weak_ptr<global::logger_registry> logreg)
  {
    _log_registry = logreg;
  }
  
  std::weak_ptr<global::logger_registry> get_registry() const
  {
    return _log_registry;
  }
  
  std::shared_ptr<ilogger> find(const std::string& name) const
  {
    if ( auto l = _log_registry.lock() )
      return l->find(name).lock();
    return nullptr;
  }
  
  static global_log& instance()
  {
    static global_log slog;
    return slog;
  }
private:
  std::weak_ptr<global::logger_registry> _log_registry;
};


}}
