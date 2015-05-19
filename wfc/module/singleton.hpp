#pragma once

#include <wfc/core/iobject.hpp>
#include <wfc/core/global.hpp>

#include <memory>
#include <string>
#include <stdexcept>


namespace wfc{
  
// base for singletion or multiton
// для sington массив из одного объекта
// multiton частый случай singletion
template< typename Name, typename Instance, typename OptJson>
class multiton
  : public iobject
{
  // Автоопределялка для Obj: object_base или object_impl
public:
  typedef Instance instance_type;
  typedef OptJson  instance_json;
  typedef typename instance_type::options_type options_type;
  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef std::shared_ptr<instance_type> instance_ptr;
  typedef std::map< std::string, instance_ptr> instance_map;


  virtual std::string name() const 
  {
    return Name()();
  }

  virtual std::string description() const
  {
    return "no description";
  }

  virtual std::string generate(const std::string& type) const 
  {
    options_type opt;
    this->generate(opt, type);
    typename options_json::serializer serializer;
    std::string result;
    serializer( opt, std::back_inserter(result) );
    return result;
  }

  virtual bool parse(const std::string& stropt)
  {
    try
    {
      auto beg = stropt.begin();
      auto end = stropt.end();
      beg = json::parser::parse_space(beg, end);
      
      options_type opt;
      typename options_json::serializer serializer;
      serializer()(opt, beg, end);
    }
    catch(const json::json_error& e)
    {
      std::stringstream ss;
      ss << "Invalid json configuration sington module '"<< this->name() << "':" << std::endl;
      ss << e.message( jsonbeg, jsonend );
      throw std::domain_error(ss.str());
    }
  }

  virtual void create( std::shared_ptr<global> g) 
  {
    _global = g;
  }

  virtual void configure(const std::string& conf, const std::string& arg)
  {
 
  }

// only for external control
  virtual void start(const std::string& arg) 
  {

  }

  virtual void stop(const std::string& arg) 
  {
    
  }

private:
  global_ptr   _global;
  instance_map _instances;

};

template<
  typename Name,
  typename Obj,
  typename OptJson
>
class singletion:
  public iobject
  //: public instance_holder<Name, Obj, OptJson>
{
public:
  typedef object_impl<Obj> object_type;
  typedef Itf inteface_type;
  typedef OptJson options_json;
  typedef typename options_json::target options_type;
  
  typedef std::shared_ptr<object_type> object_ptr;

  
  virtual void configure(const std::string& conf, const std::string& arg)  
  {
    _options.name = this->name();
    
  }
  
  virtual void initialize(const std::string& arg) 
  {
    
  }
  
  virtual void start(const std::string& arg) 
  {
    
  }
  
  virtual void stop(const std::string& arg)
  {
    
  }
  
  virtual void shutdown(const std::string& arg) 
  {
    
  }

// -------------------------------------------------

  virtual void generate(options_type& opt, const std::string& /*type*/ )
  {
    opt = options_type();
  }
  
  
  object_ptr object() const 
  {
    return _object;
  }

  object_ptr global() const 
  {
    return _global;
  }

private:
  
  options_type _options;
  object_ptr   _object;
  
};
  
}
