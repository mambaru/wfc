#pragma once


#include <wfc/module/instance_options_json.hpp>
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
  typename DomainJson,
  int Features /*= component_features::Multiton*/, 
  typename StatJson /*= defstat_json*/
>
class basic_component
  : public icomponent
{
public:
  enum { is_singleton = ( Features & int(component_features::Singleton) )!=0 };

  typedef Name        component_name;
  typedef Instance    instance_type;
  
  typedef DomainJson  domain_json;
  typedef typename std::conditional< 
    instance_type::basic_options::statistics_enabled,
    StatJson,
    empty_stat_json_t<typename StatJson::target>
  >::type stat_json;
 
  typedef instance_options_json<domain_json, Features, stat_json> instance_json;
  typedef typename instance_json::basic_json basic_json;

  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef typename instance_type::config_type instance_config;
  typedef std::map<std::string, std::string> instance_config_map;
  typedef typename instance_type::domain_interface domain_interface;
  typedef json::dict_map< json::value<std::string> > instance_map_json;
  typedef typename std::conditional<
    is_singleton,
    instance_config,
    std::vector<instance_config>
  >::type component_config;

  typedef typename std::conditional<
    is_singleton,
    instance_config_map,
    std::vector<instance_config_map>
  >::type component_map_config;

  
  typedef typename std::conditional<
    is_singleton,
    instance_json,
    ::wfc::json::array< std::vector< instance_json> >
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

  struct config_pair { std::string instance, domain;};
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

  std::string serialize_base_instance_( const instance_config& opt )
  {
    typedef typename basic_json::serializer serializer;
    std::string str;
    serializer()( opt, std::back_inserter(str) );
    return str;
  }

  std::string serialize_domain_( const instance_config& opt )
  {
    typedef typename domain_json::serializer serializer;
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
    instance_config opt;
    if ( _global != nullptr )
      _global->registry.set("instance", this->name(), _instances);
    _instances->create(this->name(), _global);
    /*
    opt.name = this->name();
    opt.enabled = true;
    _instances->reconfigure(opt);
    _instances->initialize();
    */
  }

  static void create_(fas::false_) { }

  bool configure_(const std::string& stropt, fas::true_, json::json_error* err)
  {
    component_config opt;
    if ( !this->unserialize_(opt, stropt, err ) )
      return false;
    opt.name = this->name();
    
    config_pair op;
    op.instance = this->serialize_base_instance_(opt);
    op.domain = this->serialize_domain_(opt);
    
    if ( _config.instance.empty() )
    {
      _instances->configure(opt);
      SYSTEM_LOG_MESSAGE("Singleton '" << opt.name << "' is initial configured ")
    }
    else if ( _config.domain != op.domain )        
    {
      _instances->reconfigure(opt);
      SYSTEM_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (fully)")
    }
    else if ( _config.instance != op.instance ) 
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
      op.domain = this->serialize_domain_(opt);
      op.instance = this->serialize_base_instance_(opt);
      
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
      
        if ( oitr->second.domain != op.domain )
        {
          itr->second->reconfigure(opt);
          SYSTEM_LOG_MESSAGE("Instance '" << opt.name << "' is reconfigured (fully)")
        }
        else if ( oitr->second.instance != op.instance )
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
    instance_config inst;
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
