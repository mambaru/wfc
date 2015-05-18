#pragma once

#include <wfc/core/imodule.hpp>
#include <../../../../cdaemon/geoip_converter/types.h>
#include <wfc/core/global.hpp>

#include <memory>
#include <string>
#include <stdexcept>


namespace wfc{
  
template< typename Name, typename Obj, typename OptJson>
class module_base
{
  // Автоопределялка для Obj: object_base или object_impl
};
  
template<
  typename Name,
  typename Obj,
  typename OptJson
>
class singletion
  : public imodule
{
public:
  typedef object_impl<Obj> object_type;
  typedef Itf inteface_type;
  typedef OptJson options_json;
  typedef typename options_json::target options_type;
  
  typedef std::shared_ptr<object_type> object_ptr;
  typedef std::shared_ptr<global> global_ptr;
  
  virtual int startup_priority() const
  {
    return _options.startup_priority;
  }
  
  virtual int shutdown_priority() const
  {
    return _options.shutdown_priority;
  }
  
  virtual std::string name() const 
  {
    return Name()();
  }
  
  virtual std::string description() const
  {
    return "";
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
    _options.name = this->name();
    _global = g;
  }
  
  virtual void configure(const std::string& conf, const std::string& arg)  
  {
    
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
  global_ptr   _global;
  object_ptr   _object;
  
};
  
}
