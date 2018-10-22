//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/domain_config_json.hpp>
#include <wfc/module/icomponent.hpp>
#include <wfc/module/component_features.hpp>
#include <wfc/core/wfcglobal.hpp>

#include <wfc/json.hpp>

#include <fas/typemanip.hpp>
#include <memory>
#include <string>
#include <stdexcept>

namespace wfc{
  
template< 
  typename Name,
  typename Instance,
  typename CustomJson,
  int Features /*= component_features::Multiton*/, 
  typename StatJson /*= defstat_json*/
>
class basic_component
  : public icomponent
{
public:
   enum 
   {
     is_singleton       = ( Features & int(component_features::Singleton) )!=0,
     ignore_reconfigure = ( Features & int(component_features::IgnoreReconfigure) )!=0
   };

  typedef Name        component_name;
  typedef Instance    instance_type;
  
  typedef CustomJson  custom_json;
  typedef typename std::conditional< 
    instance_type::domain_options::statistics_enabled,
    StatJson,
    empty_stat_json_t<typename StatJson::target>
  >::type stat_json;
 
  typedef domain_config_json_t<custom_json, Features, stat_json> domain_config_json;
  typedef typename domain_config_json::domain_options_json domain_options_json;

  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef typename instance_type::domain_config domain_config;
  typedef std::map<std::string, std::string> instance_config_map;
  typedef typename instance_type::domain_interface domain_interface;
  typedef json::dict_map< json::value<std::string> > instance_map_json;
  typedef typename std::conditional<
    is_singleton,
    domain_config,
    std::vector<domain_config>
  >::type component_config;

  typedef typename std::conditional<
    is_singleton,
    instance_config_map,
    std::vector<instance_config_map>
  >::type component_map_config;
  
  typedef typename std::conditional<
    is_singleton,
    domain_config_json,
    ::wfc::json::array< std::vector< domain_config_json> >
  >::type component_json;

  typedef typename std::conditional<
    is_singleton,
    instance_map_json,
    ::wfc::json::array< instance_map_json >
  >::type component_map_json;
  
  typedef std::shared_ptr<instance_type> instance_ptr;

  typedef typename std::conditional<
    is_singleton,
    instance_ptr,
    std::map< std::string, instance_ptr>
  >::type instance_map;

  struct config_pair 
  { 
    std::string basic;
    std::string custom;
  };
  
  typedef typename std::conditional<
    is_singleton,
    config_pair,
    std::map< std::string, config_pair>
  >::type config_map;

  typedef typename std::conditional<
    is_singleton,
    std::string,
    std::list< std::string>
  >::type start_list;
    
  virtual ~basic_component()
  {
    if ( auto g = _global )
      g->registry.erase( "component", this->name() );
  }

  virtual std::string name() const override
  {
    return component_name()();
  }

  virtual std::string description() const override
  {
    return "no description";
  }

  virtual std::string help() const override
  {
    return std::string("Sorry, no help for '") + this->name() + "'.";
  }

  virtual std::string interface_name() const override
  {
    return typeid( domain_interface ).name();
  }

  virtual std::string generate(const std::string& type) override
  {
    component_config opt;
    this->generate_config(opt, type);
    typename component_json::serializer serializer;
    std::string result;
    serializer( opt, std::back_inserter( result ) );
    return result;
  }

  virtual bool parse(const std::string& stropt, json::json_error* err) override
  {
    component_config opt;
    return this->unserialize_(opt, stropt, err);
  }

  virtual void create( std::shared_ptr<wfcglobal> g) override
  {
    _global = g;
    this->create_(fas::bool_<is_singleton>());
  }
  
  virtual bool configure(const std::string& stropt, json::json_error* err) override
  {
    return this->configure_(stropt, fas::bool_<is_singleton>(), err );
  }

  // only for external control
  virtual void start(const std::string& arg) override
  {
    this->start_(arg, fas::bool_<is_singleton>() );
  }

  virtual void stop(const std::string& arg) override
  {
    this->stop_(arg, fas::bool_<is_singleton>() );
  }

  virtual void generate_config(component_config& opt, const std::string& type)
  {
    this->generate_config_(opt, type, fas::bool_<is_singleton>() );
  }

private:

  std::string serialize_basic_( const domain_config& opt )
  {
    typedef typename domain_options_json::serializer serializer;
    std::string str;
    serializer()( opt, std::back_inserter(str) );
    return str;
  }

  std::string serialize_custom_( const domain_config& opt )
  {
    typedef typename domain_config_json::custom_json::serializer serializer;
    std::string str;
    serializer()( opt, std::back_inserter(str) );
    return str;
  }

  static bool unserialize_( component_config& opt,  const std::string& str, json::json_error* err )
  {
    typename component_json::serializer serializer;
    auto beg = str.begin();
    beg = json::parser::parse_space(beg, str.end(), err);
    serializer( opt, beg, str.end(), err );
    if ( err == nullptr )
      return true;
    
    return !*err;
  }
  
  void create_(fas::true_)
  {
    _instances = std::make_shared<instance_type>();
    domain_config opt;
    if ( _global != nullptr )
    {
      _global->registry.set("instance", this->name(), _instances);
    }
    json::json_error err;
    _instances->create(this->name(), _global);
    if ( !this->configure_("{}", fas::true_(), &err) )
    {
      SYSTEM_LOG_FATAL("Singleton '" << this->name() << "' default initialize: " << json::strerror::message(err) )
    }
  }

  static void create_(fas::false_) { }

  bool configure_(const std::string& stropt, fas::true_, json::json_error* err)
  {
    component_config opt;
    if ( !this->unserialize_(opt, stropt, err ) )
      return false;
    opt.name = this->name();
    
    config_pair op;
    op.basic = this->serialize_basic_(opt);
    op.custom = this->serialize_custom_(opt);
    
    if ( _config.basic.empty() )
    {
      // Это дефолтная конфигурация синглетона при создании объекта
      _instances->configure(opt);
    }
    else if ( _config.custom != op.custom )        
    {
      _instances->reconfigure(opt);
      SYSTEM_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (fully)")
    }
    else if ( _config.basic != op.basic ) 
    {
      _instances->reconfigure_basic(opt);
      SYSTEM_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (basic)")
    }
    else
    {
      SYSTEM_LOG_MESSAGE("Singleton '" << opt.name << "' without reconfiguration")
    }
    _config = op;
    return true;
  }

  void reconfigure_fully_(instance_ptr obj, const domain_config& opt, fas::false_)
  {
    obj->reconfigure(opt);
    SYSTEM_LOG_MESSAGE("Instance '" << opt.name << "' is reconfigured (fully)")
  }

  void reconfigure_fully_(instance_ptr , const domain_config& opt, fas::true_)
  {
    SYSTEM_LOG_WARNING("Instance '" << opt.name << "' not reconfigured (is forbidden by the developer)")
  }

  bool configure_(const std::string& stropt, fas::false_, json::json_error* err)
  {
    std::set<std::string> stop_list;
    for (const auto& item : _instances )
      stop_list.insert(item.first);
    
    component_config optlist;
    if ( !this->unserialize_( optlist, stropt, err ) )
      return false;

    for (const auto& opt : optlist )
    {
      stop_list.erase(opt.name);
      config_pair op;
      op.basic = this->serialize_basic_(opt);
      op.custom = this->serialize_custom_(opt);
      
      auto itr = _instances.find(opt.name);
      if ( itr == _instances.end() )
      {
        auto inst =  std::make_shared<instance_type>();
        itr = _instances.insert( std::make_pair( opt.name, inst) ).first;
        if ( _global ) _global->registry.set("instance", opt.name, inst);
        inst->create( _global );
        itr->second->configure(opt);
        SYSTEM_LOG_MESSAGE("Instance '" << opt.name << "' is initial configured")
        _config.insert(std::make_pair(opt.name, op) );
      }
      else
      {
        auto oitr = _config.find(opt.name);
        if ( oitr == _config.end() ) abort();
      
        if ( oitr->second.custom != op.custom )
        {
          reconfigure_fully_(itr->second, opt, fas::bool_<ignore_reconfigure!=0>() );
        }
        else if ( oitr->second.basic != op.basic )
        {
          itr->second->reconfigure_basic(opt);
          SYSTEM_LOG_MESSAGE("Instance '" << opt.name << "' is reconfigured (basic)")
        }
        else
        {
          SYSTEM_LOG_MESSAGE("Instance '" << opt.name << "' without reconfiguration")
        }
        oitr->second = op;
      }
    } // for optlist

    for ( const auto& n: stop_list )
    {
      auto itr = _instances.find(n);
      if ( itr == _instances.end() )
      {
        if ( _global ) _global->registry.erase("instance", n);
        itr->second->stop("");
        _instances.erase(itr);
        SYSTEM_LOG_MESSAGE("Instance '" << n << "' is deleted")
      }
    }
    return true;
  }

  void start_(const std::string& arg, fas::true_)
  {
    _instances->start(arg);
  }

  void start_(const std::string& arg, fas::false_)
  {
    for ( auto& p : _instances )
    {
      p.second->start(arg);
    }
  }

  void stop_(const std::string& arg, fas::true_)
  {
    _instances->stop(arg);
  }

  void stop_(const std::string& arg, fas::false_)
  {
    for ( auto& p : _instances )
    {
      p.second->stop(arg);
    }
  }
  
  static void generate_config_(component_config& opt, const std::string& type, fas::true_) 
  {
    instance_type().generate( opt, type);
  }

  void generate_config_(component_config& opt, const std::string& type, fas::false_) 
  {
    domain_config inst;
    instance_type().generate( inst, type);
    if ( inst.name.empty() )
      inst.name = this->name() + "1";
    opt.push_back(inst);
  }

private:
  
  global_ptr   _global;
  instance_map _instances;
  config_map  _config;
};

}
