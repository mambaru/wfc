//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/iinterface.hpp>
#include <memory>
#include <string>

namespace wfc{

struct global;

// Multiton and singleton
// 
struct iobject: iinterface
{
  virtual ~iobject(){}

  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual std::string generate(const std::string& type) const = 0;
  virtual bool parse(const std::string& conf) = 0;

  virtual int  startup_priority() const = 0;
  virtual int  shutdown_priority() const = 0;
  virtual void create( std::shared_ptr<global>) = 0;
  virtual void configure(const std::string& conf, const std::string& arg)  = 0;
  virtual void initialize(const std::string& arg) = 0;
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
  virtual void shutdown(const std::string& arg) = 0;
};

template<typename UserOptions >
struct object_options: UserOptions
{
  bool enabled = true;
  bool enable_stat = false;
  int startup_priority = 0;
  int shutdown_priority = 0;
  std::string name;
};

/// Базовый класс для прикладных объектов
/// @tparam Itf регистрируемый интерфейс
/// @tparam Opt опции
template<typename Itf, typename Opt>
class object: public iobject
{
public:
  typedef object_options<Opt> options_type;

  const options_type& options() const
  {
    return _options;
  }
  
  std::shared_ptr<global> global() const
  {
    return _global;
  }
  
  global::registry_type& registry() const
  {
    return _global->registry;
  }

// interface
  std::string name() const
  {
    return _options.name;
  }
  
private:
  options_type _options;
  std::shared_ptr<global> _global;
};


template<typename Obj, typename Config>
class object_impl: iobject
{
  
};

template<typename Opt>
struct singleton_options: Opt
{
  bool enabled = true;
};

template<typename Opt>
struct multiton_options: Opt
{
  bool enabled = true;
  std::string name;
};


template<typename Name, typename Obj, typename Json>
class singleton
  : public object_impl<Obj, singleton_options< typename Obj::options_type > >
{
  typedef singleton<Name, Obj, Json> self;
  typedef object_impl<Obj, singleton_options< typename Obj::options_type > > super;
  
  virtual bool parse(const std::string& conf) 
  {
    // парсиг здесь
  }

  virtual std::string generate(const std::string& type) 
  {
    
  }

};

template<typename Name, typename Obj>
class multiton
{
};

struct imodule: iinterface
{
  virtual ~imodule(){}
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual void create( std::shared_ptr<global>) = 0;
};

}
