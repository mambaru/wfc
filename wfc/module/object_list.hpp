#pragma once

#include <wfc/module/imodule.hpp>
#include <wfc/module/iobject.hpp>
#include <wfc/core/global.hpp>

#include <fas/type_list.hpp>
#include <memory>
#include <string>
#include <map>

namespace wfc{

template<typename Name, typename... Args>  
class object_list
  : public imodule
{
  typedef typename fas::type_list_n< Args... >::type object_types;
  typedef std::shared_ptr<iobject> object_ptr;
  typedef std::map<std::string, object_ptr > object_map;
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
    this->create_( object_types() );
  }
  
  // virtual std::vector<std::string> objects() 
  virtual std::vector< std::shared_ptr<iobject> > objects() const override
  {
    std::vector< std::shared_ptr<iobject> > result;
    for (auto& p : _objects)
    {
      result.push_back(p.second);
    }
    return result;
  }

  // only for external control
  virtual void start(const std::string& arg) override
  {
    for (auto& p : _objects)
    {
      p.second->start(arg);
    }
  }

  virtual void stop(const std::string& arg) override
  {
    for (auto& p : _objects)
    {
      p.second->stop(arg);
    }
  }


  virtual void startup_io(io_id_t , outgoing_handler_t ) override
  {
  }

  virtual void perform_io(data_ptr , io_id_t, outgoing_handler_t handler) override
  {
    if ( handler!=nullptr )
      handler(nullptr);
  }
  
  virtual void shutdown_io(io_id_t ) override
  {
    
  }

private:
  
  /*
  void objects_( std::vector<std::string>& , fas::empty_list) {}
  
  template<typename H, typename L>
  void objects_( std::vector<std::string>& obj, fas::type_list< H, L > ) 
  {
    typename H::object_name name;
    obj.push_back( name() );
  }*/

  void create_(fas::empty_list) {}
  
  template<typename H, typename L>
  void create_( fas::type_list< H, L > ) 
  {
    //typename H::object_name name;
    auto obj = std::make_shared<H>();
    _objects[ obj->name() ] = obj;
    if ( _global )
    {
      _global->registry.set("object", obj->name(), obj);
    }
    obj->create(_global);
    create_( L() );
  }

private:
  std::shared_ptr<wfcglobal> _global;
  object_map _objects;
  
};

}
