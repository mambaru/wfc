#pragma once

#include <wfc/module/imodule.hpp>
#include <wfc/module/icomponent.hpp>
#include <wfc/core/wfcglobal.hpp>

#include <fas/type_list.hpp>
#include <memory>
#include <string>
#include <list>

namespace wfc{

template<typename Name, typename... Args>  
class component_list
  : public imodule
{
  typedef typename fas::type_list_n< Args... >::type component_types;
  typedef std::shared_ptr<icomponent> component_ptr;
  typedef std::list< component_ptr > component_map;
public:
  virtual std::string name() const override
  {
    return Name()();
  }
                      
  virtual std::string description() const override
  {
    return "no description";
  }
  
  virtual void create( std::shared_ptr<wfcglobal> g) override
  {
    _global = g;
    this->create_( component_types() );
  }
  
  virtual std::vector< std::shared_ptr<icomponent> > components() const override
  {
    std::vector< std::shared_ptr<icomponent> > result;
    for (auto& p : _components)
    {
      result.push_back(p );
    }
    return result;
  }

  // only for external control
  virtual void start(const std::string& arg) override
  {
    for (auto& p : _components)
    {
      p->start(arg);
    }
  }

  virtual void stop(const std::string& arg) override
  {
    for (auto& p : _components)
    {
      p->stop(arg);
    }
  }

private:

  static void create_(fas::empty_list) {}
  
  template<typename H, typename L>
  void create_( fas::type_list< H, L > ) 
  {
    auto obj = std::make_shared<H>();
    //_components[ obj->name() ] = obj;
    _components.push_back(obj);
    if ( _global )
    {
      _global->registry.set("component", obj->name(), obj);
    }
    obj->create(_global);
    create_( L() );
  }

private:
  std::shared_ptr<wfcglobal> _global;
  component_map _components;
  
};

}
