#pragma once

#include <wfc/module/imodule.hpp>
#include <wfc/module/ipackage.hpp>

#include <fas/type_list.hpp>
#include <memory>
#include <string>
#include <map>

namespace wfc{

template<typename BuildInfo, typename... Args>  
class package
  : public ipackage
{
  typedef typename fas::type_list_n< Args... >::type module_list;
  typedef std::shared_ptr<imodule> module_ptr;
  typedef std::map<std::string, module_ptr > module_map;
public:
  virtual std::string build_info() const 
  {
    return BuildInfo()();
  }
  
  virtual std::string description() const 
  {
    return "no description";
  }
  
  virtual void create( std::shared_ptr<wfcglobal> g)
  {
    _global = g;
    this->create_( module_list() );
  }
  
  // virtual std::vector<std::string> objects() 
  virtual std::vector< std::shared_ptr<imodule> > modules() const
  {
    std::vector< std::shared_ptr<imodule> > result;
    for (auto& p : _modules)
    {
      result.push_back(p.second);
    }
    return result;
  }

  // only for external control
  virtual void start(const std::string& arg) 
  {
    for (auto& p : _modules)
    {
      p.second->start(arg);
    }
  }

  virtual void stop(const std::string& arg)
  {
    for (auto& p : _modules)
    {
      p.second->stop(arg);
    }
  }

private:
  
  void create_(fas::empty_list) {}
  
  template<typename H, typename L>
  void create_( fas::type_list< H, L > ) 
  {
    //typename H::object_name name;
    auto obj = std::make_shared<H>();
    _modules[ obj->name() ] = obj;
    if ( _global )
    {
      _global->registry.set("module", obj->name(), obj);
    }
    obj->create(_global);
    create_( L() );
  }

private:
  std::shared_ptr<wfcglobal> _global;
  module_map _modules;
  
};

}
