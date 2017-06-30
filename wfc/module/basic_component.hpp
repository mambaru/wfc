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

  std::vector<std::string> unknown_names_( const instance_config_map& lead, const instance_config_map& inst )
  {
    std::vector<std::string> unk;
    for (auto& ii: inst)
    {
      std::cout << std::endl << ii.first << std::endl;
      if ( lead.find(ii.first) == lead.end() )
        unk.push_back( ii.first );
    }
    return unk;
  }

  std::vector<std::string> 
    check_instance_names_( const instance_config_map& lead, std::string instjson )
  {
    instance_config_map inst;
    instance_map_json::serializer()( inst, instjson.begin(), instjson.end(), nullptr );
    return this->unknown_names_( lead, inst);
  }

  // Позиция неизвестного имени поля, 0 если все ок
  size_t check_names_( std::string instjson )
  {
    instance_config opt;
    std::string leadjson;
    
    typename instance_json::serializer()( opt, std::back_inserter(leadjson) );
    instance_config_map lead;
    instance_map_json::serializer()( lead, leadjson.begin(), leadjson.end(), nullptr );
    
    auto unk = this->check_instance_names_( lead, instjson);
    if ( unk.empty() )
      return 0;
    std::string rawname = std::string("\"") + unk.front() + std::string("\"");
    size_t pos = instjson.find( rawname );
    if ( pos == std::string::npos )
      return 0;
    return pos;
  }
  
  bool check_names_( std::string compjson, fas::true_, json::json_error& err )
  {
    if ( auto pos = this->check_names_(compjson) )
    {
      err = json::json_error(json::error_code::InvalidMember, compjson.size() - pos);
      return false;
    }
    return true;
  }
  
  bool check_names_( std::string /*compjson*/, fas::false_, json::json_error& /*err*/ )
  {
    /*
    if ( auto pos = this->check_names_(compjson) )
    {
      err = json::json_error(json::json_code::InvalidMember, compjson.size() - pos);
    }
    */
    return true;
  }

  bool unserialize_( component_config& opt,  const std::string& str, json::json_error* err )
  {
    typename component_json::serializer serializer;
    auto beg = str.begin();
    beg = json::parser::parse_space(beg, str.end(), err);
    serializer( opt, beg, str.end(), err );
    if ( err == nullptr )
      return true;
    
    if ( !*err )
    {
      // поиск неизвестных полей (чтобы проще было находть опечатки )
      return this->check_names_(str, fas::bool_<is_singleton>(), *err);
    }
      
    return false;
    /*if ( e )
    {
      std::stringstream ss;
      ss << "Json unserialize error for component '"<< this->name() << "':" << std::endl;
      ss << ::wjson::strerror::message_trace( e, str.begin(), str.end() ) ;
      throw std::domain_error(ss.str());
    }
    */
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
  
  void generate_config_(component_config& opt, const std::string& type, fas::true_) 
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
