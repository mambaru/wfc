#pragma once

#include <wfc/logger.hpp>
#include <wfc/core/except.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/module/multi_instance_config.hpp>
#include <wfc/module/multi_instance_config_json.hpp>

#include <algorithm>
#include <vector>
#include <string>

namespace wfc{

template<
  typename BuildInfo,
  typename ConfigJson,
  typename Inst, 
  imodule::priority StartupPriority = imodule::priority::normal,
  imodule::priority ShutdownPriority = imodule::priority::normal 
>
class multi_instance
  : public imodule
{
public:
  typedef ConfigJson configuration_json;
  typedef configuration_json instance_config_json;
  typedef typename configuration_json::target instance_config;
  typedef Inst instance_type;
  typedef std::shared_ptr<instance_type> instance_ptr;
  typedef std::unordered_map<std::string, instance_ptr> instance_map;
  typedef multi_instance_config module_config;
  typedef multi_instance_config_json module_config_json;
  
  virtual priority startup_priority() const
  {
    return StartupPriority;
  }
  
  virtual priority shutdown_priority() const
  {
    return ShutdownPriority;
  }
  
  virtual std::string version() const
  {
    return BuildInfo()();
  }
  
  virtual std::string description() const
  {
    return std::string();
  }
  
  virtual std::string generate(const std::string& type) const
  {
    std::string strconf;  
    instance_config conf = instance_type::create_config(type);
    typename instance_config_json::serializer()(conf, std::back_inserter(strconf));

    module_config module_conf;
    module_conf[_name+"1"] = strconf;
    std::string module_strconf;
    module_config_json::serializer()(module_conf, std::back_inserter(module_strconf));
    return module_strconf;
  }

  virtual bool parse_config(const std::string& confstr)
  {
    module_config conf;
    module_config_json::serializer()(conf, confstr.begin(), confstr.end());
    for(auto s : conf)
    {
      auto jsonbeg = s.second.begin();
      auto jsonend = s.second.end();
      try
      {
        jsonbeg = json::parser::parse_space(jsonbeg, jsonend);
        instance_config cong;
        
        typename instance_config_json::serializer()(cong, jsonbeg, jsonend);
        
      }
      catch(const json::json_error& e)
      {
        std::stringstream ss;
        ss << "Invalid json configuration module '"<< _name << "' for instance '" << s.first << "':" << std::endl;
        ss << e.message( jsonbeg, jsonend );
        
        throw config_error(ss.str());
      }
    }
    return true;
  }
  
  virtual void create( const std::string& name, std::shared_ptr<global> g )
  {
    _name = name;
    _global = g;
  }
  
  virtual void configure(const std::string& conf, const std::string& )
  {
    _config.clear();
    module_config_json::serializer()(_config, conf.begin(), conf.end());

    std::vector<std::string> new_list;
    std::vector<std::string> old_list;
    std::vector<std::string> stop_list;
    
    for (const auto &c : _config)
      new_list.push_back(c.first);
    for (const auto &i : _instance_map)
      old_list.push_back(i.first);

    std::sort(new_list.begin(), new_list.end());
    std::sort(old_list.begin(), old_list.end());

    std::set_difference(
      old_list.begin(), old_list.end(),
      new_list.begin(), new_list.end(),
      std::back_inserter(stop_list)
    );

    for (const auto &n : stop_list)
    {
      CONFIG_LOG_MESSAGE("module '" << _name << "': stop instance '" << n << "'...")
      _instance_map[n]->stop();
    }

    for (const auto &n : stop_list)
      _instance_map.erase(n);

    
    for (const auto &n : _config)
    {
      instance_config conf;
      typename instance_config_json::serializer()( conf, n.second.begin(), n.second.end() );
      auto itr = _instance_map.find(n.first);
      if (itr == _instance_map.end() )
      {
        CONFIG_LOG_MESSAGE("module '" << _name << "': create instance '" << n.first << "'...")
        _instance_map[n.first] = std::make_shared<instance_type>(n.first, _global, conf);
      }
      else
      {
        CONFIG_LOG_MESSAGE("module '" << _name << "': reconfigure instance '" << n.first << "'...")
        itr->second->reconfigure( conf );
      }
    }
  }
  
  virtual void initialize(const std::string&)
  {
    for (const auto &n : _instance_map)
    {
      CONFIG_LOG_MESSAGE("module '" << _name << "': initialize instance '" << n.first << "'...")
      n.second->initialize();
    }
  }
  
  virtual void start(const std::string&)
  {
    for (const auto &n : _instance_map)
    {
      CONFIG_LOG_MESSAGE("module '" << _name << "': start instance '" << n.first << "'...")
      n.second->start();
    }
  }
  
  virtual void stop(const std::string&)
  {
    for (auto &n : _instance_map)
    {
      CONFIG_LOG_MESSAGE("module '" << _name << "': stop instance '" << n.first << "'...")
      n.second->stop();
      n.second.reset();
    }
    _instance_map.clear();
  }
  
  virtual void shutdown(const std::string&)
  {
    for (auto &n : _instance_map)
    {
      CONFIG_LOG_MESSAGE("module '" << _name << "': shutdown instance '" << n.first << "'...")
      n.second->shutdown();
      n.second.reset();
    }
    _instance_map.clear();
  }

private:
  std::string _name;
  module_config _config;
  instance_map _instance_map;
  std::shared_ptr<global> _global;
};

}