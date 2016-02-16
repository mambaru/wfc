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
  typedef typename object_type::options_type domain_options_type;
  typedef instance_options<domain_options_type> options_type;

  typedef std::shared_ptr<object_type> object_ptr;
  typedef std::shared_ptr<wfcglobal> global_ptr;

  virtual ~instance() {}

// iobject interface
  virtual std::string name() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _options.name;
  }

  virtual int startup_priority() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _options.startup_priority;
  }

  virtual int shutdown_priority() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _options.shutdown_priority;
  }

  virtual void start(const std::string& arg) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    if ( _ready_for_start )
      this->start_(arg);
    _ready_for_start = false;
  }

  virtual void stop(const std::string& arg)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->stop_(arg);
  }

  virtual void initialize() 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _ready_for_start )
      this->initialize_();
  }

// iinterface
  virtual void reg_io(io_id_t, std::weak_ptr<iinterface>) override
  {
  }

  virtual void perform_io(data_ptr, io_id_t, outgoing_handler_t handler)
  {
    if ( handler!=nullptr )
      handler(nullptr);
  }

  virtual void unreg_io(io_id_t)
  {
  }

//

  void generate(options_type& opt, const std::string& type) const 
  {
    object_type::generate( opt, type );
  }

  void create( std::shared_ptr<wfcglobal> g) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _global = g;
  }

  void configure(const options_type& opt)  
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _ready_for_start = true;
    _options = opt;
    // Reset ready flag for enable startup
    _startup = _startup && !( _object==nullptr && _options.enabled );
    this->configure_(_options.enabled);
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

  options_type options() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _options;
  }
  
private:
  
  void configure_(bool enabled) 
  {
    // Конфигурирование и регистрация объекта
    // Создание объекта
    if ( enabled )
    {
      if ( _object == nullptr )
      {
        _object = std::make_shared<object_type>();
      }

      if (_global)
      {
        _global->registry.set(_options.name, _object);
      }
    }
    else
    {
      if ( _object != nullptr )
      {
        _object->stop("");
        if ( _global )
        {
          _global->registry.erase(_options.name);
        }
        _object = nullptr;
      }
    }
  }
  
  void initialize_() 
  {
    if ( _object != nullptr )
    {
      _object->initialize(_options.name, _global, static_cast<const domain_options_type&>(_options) );
    }
  }

  void startup_(const std::string& arg) 
  {
    if ( _object != nullptr )
    {
      _object->start(arg);
    }
    _startup = true;
  }

  void forced_start_(const std::string& arg) 
  {
    configure_(true);
    initialize_();
    _object->start(arg);
  }

  void start_(const std::string& arg) 
  {
    if ( !_startup )
    {
      this->startup_(arg);
    }
    else if ( _object == nullptr )
    {
      this->forced_start_(arg);
    }
  }

  void stop_(const std::string& arg)
  {
    if ( _object != nullptr )
    {
      _object->stop(arg);
      _object = nullptr;
    }
  }

private:

  bool         _ready_for_start = true;
  bool         _startup = false;
  global_ptr   _global;
  options_type _options;
  object_ptr   _object;

  mutable mutex_type _mutex;
};

}
