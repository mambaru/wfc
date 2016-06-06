#pragma once

#include <wfc/core/build_info.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/core/global.hpp>

#include <fas/type_list.hpp>
#include <memory>
#include <string>
#include <map>

namespace wfc{

template<typename BuildInfo, typename... Args>  
class module_list
  : public ipackage
{
  typedef typename fas::type_list_n< Args... >::type module_types;
  typedef std::shared_ptr<imodule> module_ptr;
  typedef std::map<std::string, module_ptr > module_map;
  typedef std::shared_ptr<wfcglobal> global_ptr;

public:

  virtual std::shared_ptr<ibuild_info> build_info()  
  {
    return make_build_info<BuildInfo>();
  }

  virtual std::string name()  
  {
    return BuildInfo().name();
  }

  virtual std::string description()  
  {
    return "no description";
  }

  virtual void create( std::shared_ptr<wfcglobal> g)
  {
    
    this->create_( g, module_types() );
  }

  virtual std::vector< std::shared_ptr<imodule> > modules() 
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

  void create_(global_ptr, fas::empty_list) {}

  template<typename H, typename L>
  void create_(global_ptr g, fas::type_list< H, L > ) 
  {
    auto obj = std::make_shared<H>();
    _modules[ obj->name() ] = obj;
    if ( g!=nullptr )
      g->registry.set("module", obj->name(), obj);
    obj->create(g);
    this->create_( g, L() );
  }
private:
  module_map _modules;
};

}
