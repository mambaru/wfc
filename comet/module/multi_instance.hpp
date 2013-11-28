#pragma once

#include <comet/module/imodule.hpp>
#include <comet/module/multi_instance_config.hpp>
#include <comet/module/multi_instance_config_json.hpp>

namespace mamba{ namespace comet{

template<typename BuildInfo, typename ConfigJson, typename Inst>
class multi_instance
  : public imodule
{
public:
  typedef ConfigJson configuration_json;
  typedef typename configuration_json::serializer configuration_serializer;
  typedef typename configuration_json::target configuration_type;
  typedef Inst instance_type;
  typedef std::shared_ptr<instance_type> instance_ptr;
  typedef std::unordered_map<std::string, instance_ptr> instance_map;
  typedef multi_instance_config module_config;
  typedef multi_instance_config_json module_config_json;
  
  virtual priority startup_priority() const
  {
    
  }
  
  virtual priority shutdown_priority() const
  {
    
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
    std::string result;  
    configuration_type conf;
    configuration_serializer()(conf, std::back_inserter(result));
    return result;
  }
  
  virtual bool parse_config(const std::string& confstr)
  {
    module_config conf;
    module_config_json::serializer()(conf, confstr.begin(), confstr.end());
    return true;
  }
  
  virtual void create( const std::string& name, std::weak_ptr<global> g )
  {
    _global = g;
  }
  
  virtual void configure(const std::string& conf)
  {
    /*configuration_type conf;
    configuration_serializer()(conf, confstr.begin(), confstr.end());
    */
  }
  
  virtual void initialize()
  {
    
  }
  
  virtual void start()
  {
    
  }
  
  virtual void stop()
  {
    
  }

private:
  instance_map _instance_map;
  std::weak_ptr<global> _global;
};

}}