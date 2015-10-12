#pragma once

#include <wfc/module/imodule.hpp>
#include <wfc/module/icomponent.hpp>
#include <wfc/core/global.hpp>

#include <fas/type_list.hpp>
#include <memory>
#include <string>
#include <map>

namespace wfc{

template<typename Name, typename... Args>  
class component_list
  : public imodule
{
  typedef typename fas::type_list_n< Args... >::type component_types;
  typedef std::shared_ptr<icomponent> component_ptr;
  typedef std::map<std::string, component_ptr > component_map;
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
      result.push_back(p.second);
    }
    return result;
  }

  // only for external control
  virtual void start(const std::string& arg) override
  {
    for (auto& p : _components)
    {
      p.second->start(arg);
    }
  }

  virtual void stop(const std::string& arg) override
  {
    for (auto& p : _components)
    {
      p.second->stop(arg);
    }
  }


  virtual void reg_io(io_id_t , std::weak_ptr<iinterface> ) override
  {
  }

  virtual void perform_io(data_ptr , io_id_t, outgoing_handler_t handler) override
  {
    if ( handler!=nullptr )
      handler(nullptr);
  }
  
  virtual void unreg_io(io_id_t ) override
  {
    
  }

private:

  void create_(fas::empty_list) {}
  
  template<typename H, typename L>
  void create_( fas::type_list< H, L > ) 
  {
    auto obj = std::make_shared<H>();
    _components[ obj->name() ] = obj;
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