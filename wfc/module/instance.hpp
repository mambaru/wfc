#pragma once

#include <wfc/module/instance_options.hpp>
#include <wfc/module/iinstance.hpp>
#include <wfc/core/global.hpp>

#include <memory>
#include <string>
#include <stdexcept>

namespace wfc{

template<typename DomainObject>
class instance
  : public iinstance
{
public:
  typedef std::recursive_mutex mutex_type;
  typedef DomainObject object_type;
  typedef typename object_type::domain_interface domain_interface;
  typedef typename object_type::config_type config_type;
  typedef typename object_type::basic_options basic_options;
  /*
  typedef typename object_type::options_type domain_options_type;
  typedef domain_instance_options<domain_options_type> options_type;
  */

  typedef std::shared_ptr<object_type> object_ptr;
  typedef std::shared_ptr<wfcglobal> global_ptr;

  virtual ~instance() {}

// iinstance interface
  virtual std::string name() const override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _config.name;
  }

  virtual int startup_priority() const override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _config.startup_priority;
  }

  virtual int shutdown_priority() const override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _config.shutdown_priority;
  }
  
  void create( std::shared_ptr<wfcglobal> g) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _global = g;
  }

  virtual void configure(const config_type& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _instance_reconfigured = true;
    _config = opt;
    if ( auto obj = this->create_or_stop_if_() ) 
      obj->configure_domain(_config);
  }

  virtual void reconfigure_basic( const basic_options& opt )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    static_cast<basic_options&>( _config ) = opt;
    _startup = _startup && !( _object==nullptr && _config.enabled );
    if ( auto obj = this->create_or_stop_if_() ) 
      obj->reconfigure_domain_basic( _config );
  }

  virtual void reconfigure(const config_type& opt)  
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _instance_reconfigured = true;
    _config = opt;

    // Reset ready flag for enable startup
    _startup = _startup && !( _object==nullptr && _config.enabled );

    if ( auto obj = this->create_or_stop_if_() ) 
    {
      obj->reconfigure_domain(_config);
    }
  }

  virtual void initialize() override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if (auto obj = _object )
    {
      obj->initialize_domain( );
    }
  }

  virtual bool is_reconfigured() const override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _instance_reconfigured;
  }

  virtual void start(const std::string& arg) override
  {
    std::lock_guard<mutex_type> lk(_mutex);

    if ( _instance_reconfigured )
    {
      this->start_(arg);
    }
    else
    {
      CONFIG_LOG_MESSAGE("Instance '" << _config.name << "' without restart")
    }
    _instance_reconfigured = false;
    _ready_for_stop = true;
  }

  virtual void stop(const std::string& arg) override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->stop_(arg);
  }

// iinterface

  void generate(config_type& opt, const std::string& type) const 
  {
    auto obj = std::make_shared<object_type>();
    obj->domain_generate( opt, type );
  }

// -------------------------------------------------

  object_ptr object() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _object;
  }

  global_ptr global() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _global;
  }

private:

  object_ptr create_or_stop_if_() 
  {
    // Конфигурирование и регистрация объекта
    // Создание объекта
    if ( _config.enabled )
    {
      if ( _object == nullptr )
      {
        _object = std::make_shared<object_type>();
        _object->create_domain(_config.name, _global);
      }

      if (_global)
      {
        _global->registry.set(_config.name, _object);
      }
    }
    else
    {
      if ( _object != nullptr )
      {
        _object->stop();
        if ( _global )
        {
          _global->registry.erase(_config.name);
        }
        _object = nullptr;
      }
    }
    return _object;
  }

  void startup_(const std::string& ) 
  {
    if ( _object != nullptr )
    {
      _object->start();
    }
    _startup = true;
  }

  void forced_start_(const std::string& ) 
  {
    _config.enabled = true;
    if ( auto obj = this->create_or_stop_if_() )
    {
      obj->initialize_domain();
      obj->start();
    }
  }

  void start_(const std::string& arg) 
  {
    if ( !_startup )
    {
      this->startup_(arg);
    }
    else if ( _object != nullptr )
    {
      _object->ready_domain();
    }
    else
    {
      this->forced_start_(arg);
    }
  }

  void stop_(const std::string& /*arg*/)
  {
    if ( _object != nullptr )
    {
      if ( _ready_for_stop )
        _object->stop();
      _object = nullptr;
    }
    _ready_for_stop = false;
  }

private:
  // Разрешает запуск. После запуска, последующие попытки вызова start игнорируються. Сбрасываеться при переконфигурации
  bool         _instance_reconfigured = true;
  bool         _ready_for_stop = false;
  // Объект запущен (вызван метод старт)
  bool         _startup = false;
  global_ptr   _global;
  config_type  _config;
  object_ptr   _object;

  mutable mutex_type _mutex;
};

}
