//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/iinstance.hpp>
#include <wfc/core/wfcglobal.hpp>
#include <memory>
#include <string>
#include <stdexcept>

namespace wfc{

/**
 * @brief multiton
 */
template<typename DomainObject>
class instance
  : public iinstance
{
public:
  typedef std::recursive_mutex mutex_type;
  typedef DomainObject object_type;
  typedef typename object_type::instance_handler_t instance_handler_t;
  typedef typename object_type::domain_interface domain_interface;
  typedef typename object_type::domain_config domain_config;
  typedef typename object_type::domain_options domain_options;
  typedef typename object_type::customstat_options customstat_options;

  typedef std::shared_ptr<object_type> object_ptr;
  typedef std::shared_ptr<wfcglobal> global_ptr;

  virtual ~instance() 
  {
    _object = nullptr;
    _global = nullptr;
  }

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
  
  void create( const std::shared_ptr<wfcglobal>& g) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _global = g;
  }

  void create( const std::string& obj_name, const std::shared_ptr<wfcglobal>& g) 
  {
    // для синглетонов
    std::lock_guard<mutex_type> lk(_mutex);
    _global = g;
    _object = std::make_shared<object_type>();
    get_(_object)->create_domain(obj_name, _global);
    if ( _global != nullptr )
      _global->registry.set_target(obj_name, _object);
  }

  virtual void configure(const domain_config& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _instance_reconfigured = true;
    _config = opt;
    if ( auto obj = this->create_or_stop_if_() ) 
      get_(obj)->configure_domain(_config);
  }

  virtual void reconfigure_basic( const domain_options& opt )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    static_cast<domain_options&>( _config ) = opt;
    _startup = _startup && !( _object==nullptr && _config.enabled );
    if ( auto obj = this->create_or_stop_if_() ) 
    {
      if ( _was_disabled )
        get_(obj)->configure_domain(_config);
      else
        get_(obj)->reconfigure_domain_basic( _config );
    }
  }

  virtual void reconfigure(const domain_config& opt)  
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _config = opt;

    _instance_reconfigured = true;
    // Reset ready flag for enable startup
    _startup = _startup && !( _object==nullptr && _config.enabled );

    if ( auto obj = this->create_or_stop_if_() ) 
    {
      if ( _was_disabled )
        get_(obj)->configure_domain(_config);
      else
        get_(obj)->reconfigure_domain(_config);
    }
  }

  virtual void initialize() override
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if (auto obj = _object )
    {
      get_(obj)->initialize_domain( );
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

    if ( _instance_reconfigured || _was_disabled)
    {
      this->start_(arg);
    }
    else
    {
      SYSTEM_LOG_MESSAGE("Instance '" << _config.name << "' without restart")
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

  void generate(domain_config& opt, const std::string& type) const 
  {
    auto obj = _object;
    if ( obj == nullptr )
      obj = std::make_shared<object_type>();
    get_(obj)->domain_generate( opt, type );
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
        _was_disabled = true;
        _object = std::make_shared<object_type>();
        get_(_object)->create_domain(_config.name, _global);
      }

      if (_global)
      {
        _global->registry.set_target(_config.name, _object);
      }
    }
    else
    {
      if ( _object != nullptr )
      {
        get_(_object)->stop_domain();
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
    if ( auto obj = get_() )
    {  
      obj->start_domain(); 
    }
    _startup = true;
  }

  void forced_start_(const std::string& ) 
  {
    _config.enabled = true;
    if ( auto obj = this->create_or_stop_if_() )
    {
      get_(obj)->initialize_domain();
      get_(obj)->start_domain();
    }
  }

  void start_(const std::string& arg) 
  {
    if ( !_startup || _was_disabled)
    {
      this->startup_(arg);
    }
    else if ( auto obj = get_() )
    {
      obj->restart_domain();
    }
    else
    {
      this->forced_start_(arg);
    }
    _was_disabled = false;
  }

  void stop_(const std::string& /*arg*/)
  {
    if ( auto obj = get_() )
    {
      if ( _ready_for_stop )
      {
        obj->stop_domain();
      }
      _global->registry.erase(_config.name);
      _object = nullptr;
      _was_disabled = true;
    }
    _ready_for_stop = false;
  }

  instance_handler_t* get_()
  {
    return get_(_object);
  }

  instance_handler_t* get_(object_ptr& obj) const
  {
    if ( obj!=nullptr )
      return obj->inst_handler_();
    return nullptr;
  }

private:
  // Разрешает запуск. После запуска, последующие попытки вызова start игнорируються. Сбрасываеться при переконфигурации
  bool         _instance_reconfigured = true;
  bool         _ready_for_stop = false;
  // Объект запущен (вызван метод старт)
  bool         _startup = false;
  bool         _was_disabled = true;
  global_ptr   _global;
  domain_config  _config;
  object_ptr   _object;
  mutable mutex_type _mutex;
};

}
