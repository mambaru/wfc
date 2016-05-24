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
  typedef typename instance_type::options_type instance_options;
  typedef typename instance_type::domain_interface domain_interface;
  
  typedef typename std::conditional<
    is_singleton,
    instance_options,
    std::vector<instance_options>
  >::type component_options;

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

  struct options_pair { std::string instance, domain;};
  typedef typename std::conditional<
    is_singleton,
    options_pair,
    std::map< std::string, options_pair>
  >::type options_map;

  typedef typename std::conditional<
    is_singleton,
    std::string,
    std::list< std::string>
  >::type start_list;
    
  virtual ~basic_component()
  {
    if ( _global )
    {
      _global->registry.erase( "component", this->name() );
    }
  }

  virtual std::string name() const 
  {
    return component_name()();
  }

  virtual std::string description() const
  {
    return "no description";
  }

  virtual std::string interface_name() const 
  {
    return typeid( domain_interface ).name();
  }

  // TODO: generate json
  virtual std::string generate(const std::string& type) const 
  {
    component_options opt;
    this->generate_options(opt, type);
    typename component_json::serializer serializer;
    std::string result;
    serializer( opt, std::back_inserter( result ) );
    return std::move(result);
  }

  virtual bool parse(const std::string& stropt)
  {
    component_options opt;
    this->unserialize_(opt, stropt );
    return true;
  }

  virtual void create( std::shared_ptr<wfcglobal> g) 
  {
    _global = g;
    this->create_(fas::bool_<is_singleton>());
  }
  
  virtual void configure(const std::string& stropt, const std::string& /*arg*/)
  {
    this->configure_(stropt, fas::bool_<is_singleton>() );
  }

  virtual void generate_options(component_options& opt, const std::string& type) const 
  {
    this->generate_options_(opt, type, fas::bool_<is_singleton>() );
  }
  
  // only for external control
  virtual void start(const std::string& arg) 
  {
    this->start_(arg, fas::bool_<is_singleton>() );
  }

  virtual void stop(const std::string& arg) 
  {
    this->stop_(arg, fas::bool_<is_singleton>() );
  }

private:

  void serialize_base_instance_( const instance_options& opt,  std::string& str )
  {
    typename base_instance_json::serializer serializer;
    str.clear();
    serializer( opt, std::back_inserter(str) );
  }

  void serialize_domain_( const instance_options& opt,  std::string& str )
  {
    typename domain_json::serializer serializer;
    str.clear();
    serializer( opt, std::back_inserter(str) );
  }

  void unserialize_( component_options& opt,  const std::string& str )
  {
    try
    {
      typename component_json::serializer serializer;
      serializer( opt, str.begin(), str.end() );
    }
    catch(const ::iow::json::json_error& e)
    {
      std::stringstream ss;
      ss << "Json unserialize error for component '"<< this->name() << "':" << std::endl;
      ss << e.message( str.begin(), str.end() );
      throw std::domain_error(ss.str());
    }
  }
  
  void create_(fas::true_)
  {
    _instances = std::make_shared<instance_type>();
    instance_options opt;
    opt.name = this->name();
    opt.enabled = true;
    _global->registry.set("instance", this->name(), _instances);
    _instances->create(_global);
    _instances->reconfigure(opt);
    _instances->initialize();
    // CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' configured with defaults params")
  }

  void create_(fas::false_) { }

  void configure_(const std::string& stropt, fas::true_)
  {
    component_options opt;
    this->unserialize_(opt, stropt );
    opt.name = this->name();
    
    options_pair op;
    this->serialize_base_instance_(opt, op.instance );
    this->serialize_domain_(opt, op.domain );
    
    if ( _options.domain != op.domain )        
    {
      _instances->reconfigure(opt);
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (fully)")
    }
    else if ( _options.instance != op.instance ) 
    {
      _instances->reconfigure_instance(opt);
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' is reconfigured (basic)")
    }
    else
    {
      CONFIG_LOG_MESSAGE("Singleton '" << opt.name << "' without reconfiguration")
    }
    
    _options = op;
  }


  void configure_(const std::string& stropt, fas::false_)
  {
    std::set<std::string> stop_list;
    for (const auto& item : _instances )
      stop_list.insert(item.first);
    
    component_options optlist;
    this->unserialize_( optlist, stropt );

    for (const auto& opt : optlist )
    {
      stop_list.erase(opt.name);
      options_pair op;
      this->serialize_domain_(opt, op.domain );
      this->serialize_base_instance_(opt, op.instance );
      
      auto itr = _instances.find(opt.name);
      if ( itr == _instances.end() )
      {
        auto inst =  std::make_shared<instance_type>();
        itr = _instances.insert( std::make_pair( opt.name, inst) ).first;
        if ( _global ) _global->registry.set("instance", opt.name, inst);
        inst->create( _global );
        itr->second->reconfigure(opt);
        CONFIG_LOG_MESSAGE("Instance '" << opt.name << "' is configured")
        _options.insert(std::make_pair(opt.name, op) );
      }
      else
      {
        auto oitr = _options.find(opt.name);
        if ( oitr == _options.end() ) abort();
      
        if ( oitr->second.domain != op.domain )
        {
          itr->second->reconfigure(opt);
          CONFIG_LOG_MESSAGE("Instance '" << opt.name << "' is reconfigured (fully)")
        }
        else if ( oitr->second.instance != op.instance )
        {
          itr->second->reconfigure_instance(opt);
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

  /*
  void configure_(const std::string& stropt, fas::false_)
  {
    std::set<std::string> stop_list;
    for (const auto& item : _instances )
      stop_list.insert(item.first);

    component_options optlist;
    this->unserialize_( optlist, stropt );

    
    for (const auto& opt : optlist )
    {
      bool suspend_only = false;
      { // проверям на фактическое изменения опций объекта
        // для этого сериализуем его и сравнимваем строки
        // после сериализации пробелов и коментариев в строке не будет
        
        std::string dom_json;
        std::string ins_json;
        
        this->serialize_domain_(opt, dom_json );
        this->serialize_base_instance_(opt, ins_json );
        
        auto itr = _options.find(opt.name);
        if ( itr!=_options.end() )
        {
          
          if ( strins == itr->second )
          {
            continue;
          }
          
          {
            auto test_opt = opt;
            test_opt.suspend = !opt.suspend;
            std::string test_strins;
            this->serialize_(test_opt, test_strins );
            suspend_only = ( test_strins == itr->second );
          }
          
          itr->second = strins;
          
        }
        else
        {
          _options.insert(itr, std::make_pair(opt.name, strins) );
        }
      }
      
      auto itr = _instances.find(opt.name);
      if ( itr == _instances.end() )
      {
        auto inst =  std::make_shared<instance_type>();
        itr = _instances.insert( std::make_pair( opt.name, inst) ).first;
        if ( _global )
        {
          _global->registry.set("instance", opt.name, inst);
        }
        inst->create( _global );
      }
      
      if ( suspend_only )
      {
        DEBUG_LOG_MESSAGE("component: change only suspend " << opt.name)
        itr->second->suspend(opt.suspend);
      }
      else
      {
        itr->second->configure(opt);
        itr->second->suspend(opt.suspend);
        DEBUG_LOG_MESSAGE("component: configured instance " << opt.name)
      }
      stop_list.erase(opt.name);
    } // for optlist
    
    for ( const auto& name: stop_list )
    {
      auto itr = _instances.find(name);
      if ( itr == _instances.end() )
      {
        if ( _global )
        {
          _global->registry.erase("instance", name);
        }
        DEBUG_LOG_MESSAGE("component: stop instance " << itr->first)
        itr->second->stop("");
        _instances.erase(itr);
      }
    }
  }
  
  */
  
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
  
  void generate_options_(component_options& opt, const std::string& type, fas::true_) const 
  {
    instance_type().generate( opt, type);
  }

  void generate_options_(component_options& opt, const std::string& type, fas::false_) const 
  {
    instance_options inst;
    inst.name = this->name() + "1";
    instance_type().generate( inst, type);
    opt.push_back(inst);
  }

private:
  
  global_ptr   _global;
  instance_map _instances;
  options_map  _options;
};

}
