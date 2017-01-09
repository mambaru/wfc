#pragma once


#include <wfc/module/instance_options_json.hpp>
#include <wfc/module/icomponent.hpp>
#include <wfc/module/component_features.hpp>
#include <wfc/core/global.hpp>

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
  int Features = component_features::Multiton
>
class basic_component
  : public icomponent
{
public:
  enum { is_singleton = ( Features & int(component_features::Singleton) )!=0 };

  typedef Name        component_name;
  typedef Instance    instance_type;
  
  typedef DomainJson  domain_json;
  typedef base_instance_options_json<Features> base_instance_json;
  typedef domain_instance_options_json<domain_json, Features> instance_json;

  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef typename instance_type::config_type instance_config;
  typedef typename instance_type::domain_interface domain_interface;
  
  typedef typename std::conditional<
    is_singleton,
    instance_config,
    std::vector<instance_config>
  >::type component_config;

  typedef typename std::conditional<
    is_singleton,
    instance_json,
    ::wfc::json::array< std::vector< instance_json> >
  >::type component_json;
  
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

  virtual std::string name() override
  {
    return component_name()();
  }

  virtual std::string description() override
  {
    return "no description";
  }

  virtual std::string interface_name() override
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
    return std::move(result);
  }

  virtual bool parse(const std::string& stropt) override
  {
    component_config opt;
    this->unserialize_(opt, stropt );
    return true;
  }

  virtual void create( std::shared_ptr<wfcglobal> g) override
  {
    _global = g;
    this->create_(fas::bool_<is_singleton>());
  }
  
  virtual void configure(const std::string& stropt, const std::string& /*arg*/) override
  {
    this->configure_(stropt, fas::bool_<is_singleton>() );
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

  void serialize_base_instance_( const instance_config& opt,  std::string& str )
  {
    typedef typename base_instance_json::serializer serializer;
    str.clear();
    serializer()( opt, std::back_inserter(str) );
  }

  void serialize_domain_( const instance_config& opt,  std::string& str )
  {
    typedef typename domain_json::serializer serializer;
    str.clear();
    serializer()( opt, std::back_inserter(str) );
  }

  void unserialize_( component_config& opt,  const std::string& str )
  {
    typename component_json::serializer serializer;
    ::wjson::json_error e;
    serializer( opt, str.begin(), str.end(), &e );
    if ( e )
    {
      std::stringstream ss;
      ss << "Json unserialize error for component '"<< this->name() << "':" << std::endl;
      ss << ::wjson::strerror::message_trace( e, str.begin(), str.end() ) ;
      throw std::domain_error(ss.str());
    }
  }
  
  void create_(fas::true_)
  {
    _instances = std::make_shared<instance_type>();
    instance_config opt;
    opt.name = this->name();
    opt.enabled = true;
    _global->registry.set("instance", this->name(), _instances);
    _instances->create(_global);
    _instances->reconfigure(opt);
    _instances->initialize();
  }

  void create_(fas::false_) { }

  void configure_(const std::string& stropt, fas::true_)
  {
    component_config opt;
    this->unserialize_(opt, stropt );
    opt.name = this->name();
    
    config_pair op;
    this->serialize_base_instance_(opt, op.instance );
    this->serialize_domain_(opt, op.domain );
    
    if ( _config.instance.empty() )
    {
      _instances->configure(opt);
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' is initial configured ")
    }
    else if ( _config.domain != op.domain )        
    {
      _instances->reconfigure(opt);
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (fully)")
    }
    else if ( _config.instance != op.instance ) 
    {
      _instances->reconfigure_basic(opt);
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (basic)")
    }
    else
    {
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' without reconfiguration")
    }
    
    _config = op;
  }


  void configure_(const std::string& stropt, fas::false_)
  {
    std::set<std::string> stop_list;
    for (const auto& item : _instances )
      stop_list.insert(item.first);
    
    component_config optlist;
    this->unserialize_( optlist, stropt );

    for (const auto& opt : optlist )
    {
      stop_list.erase(opt.name);
      config_pair op;
      this->serialize_domain_(opt, op.domain );
      this->serialize_base_instance_(opt, op.instance );
      
      auto itr = _instances.find(opt.name);
      if ( itr == _instances.end() )
      {
        auto inst =  std::make_shared<instance_type>();
        itr = _instances.insert( std::make_pair( opt.name, inst) ).first;
        if ( _global ) _global->registry.set("instance", opt.name, inst);
        inst->create( _global );
        itr->second->configure(opt);
        CONFIG_LOG_MESSAGE("Instance '" << opt.name << "' is initial configured")
        _config.insert(std::make_pair(opt.name, op) );
      }
      else
      {
        auto oitr = _config.find(opt.name);
        if ( oitr == _config.end() ) abort();
      
        if ( oitr->second.domain != op.domain )
        {
          itr->second->reconfigure(opt);
          CONFIG_LOG_MESSAGE("Instance '" << opt.name << "' is reconfigured (fully)")
        }
        else if ( oitr->second.instance != op.instance )
        {
          itr->second->reconfigure_basic(opt);
          CONFIG_LOG_MESSAGE("Instance '" << opt.name << "' is reconfigured (basic)")
        }
        else
        {
          CONFIG_LOG_MESSAGE("Instance '" << opt.name << "' without reconfiguration")
        }
        oitr->second = op;
      }
    } // for optlist

    for ( const auto& name: stop_list )
    {
      auto itr = _instances.find(name);
      if ( itr == _instances.end() )
      {
        if ( _global ) _global->registry.erase("instance", name);
        itr->second->stop("");
        _instances.erase(itr);
        CONFIG_LOG_MESSAGE("Instance '" << name << "' is deleted")
      }
    }
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
  
  void generate_config_(component_config& opt, const std::string& type, fas::true_) 
  {
    instance_type().generate( opt, type);
  }

  void generate_config_(component_config& opt, const std::string& type, fas::false_) 
  {
    instance_config inst;
    inst.name = this->name() + "1";
    instance_type().generate( inst, type);
    opt.push_back(inst);
  }

private:
  
  global_ptr   _global;
  instance_map _instances;
  config_map  _config;
};

}
