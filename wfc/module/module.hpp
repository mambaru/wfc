#pragma once

#include <wfc/core/imodule.hpp>
#include <wfc/core/iobject.hpp>

namespace wfc{

template<typename Name, typename... Args>  
class module
  : public imodule
{
  typedef typename fas::type_list_n< Args... >::type object_list;
  typedef std::shared_ptr<iobject> object_ptr;
  typedef std::map<std::string, object_ptr > object_map;
public:
  virtual std::string name() const 
  {
    return Name()();
  }
  
  virtual std::string description() const 
  {
    return "no description";
  }
  
  virtual void create( std::shared_ptr<wfcglobal> g)
  {
    _global = g;
    this->create_( object_list() );
  }
  
  // virtual std::vector<std::string> objects() 
  virtual std::vector< std::shared_ptr<iobject> > objects() const
  {
    std::vector< std::shared_ptr<iobject> > result;
    for (auto& p : _objects)
    {
      result.push_back(p.second);
    }
    return result;
    /*
    std::vector<std::string> result;
    this->objects_(result, object_list() );
    return std::move(result);
    */
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
