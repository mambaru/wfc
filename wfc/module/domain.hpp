#pragma once

#include <wfc/core/global.hpp>

#include <memory>
#include <string>
#include <stdexcept>
#include <boost/concept_check.hpp>


namespace wfc{

template<
  typename Itf,
  typename Opt
>
class domain
  : public Itf
{
  
public:
  typedef Opt options_type;
  typedef Itf interface_type;
  typedef std::shared_ptr<wfcglobal> global_ptr;

  const std::string& name() const
  {
    return _name;
  }
  
  global_ptr global() const 
  {
    return _global;
  }

  const options_type& options() const 
  {
    return _options;
  }
  
  
  static void generate(options_type& opt, const std::string& /*type*/)
  {
    opt = options_type();
  }
  
  virtual void initialize(const std::string& name, global_ptr g, const options_type& opt)
  {
    _name = name;
    _global = g;
    _options = opt;
    this->reconfigure();
  }

  virtual void start(const std::string&)
  {
    // TODO: LOG default (empty) start
  }

  virtual void stop(const std::string&)
  {
    // TODO: LOG default (empty) stop
  }
  
  virtual void perform(const std::string&) 
  {
    
  }

  virtual void reconfigure()
  {
    // Переконфигурация запущенного объекта!!
    // TODO: LOG default (empty) initialize
  }

private:  
  
  std::string _name;
  global_ptr _global;
  options_type _options;
};
  
}
