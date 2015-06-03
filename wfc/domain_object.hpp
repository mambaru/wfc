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
class domain_object
  : public Itf
{
public:
  typedef Opt options_type;
  typedef Itf interface_type;
  typedef std::shared_ptr<wfcglobal> global_ptr;
  
  
  typedef typename interface_type::data_type data_type;
  typedef typename interface_type::data_ptr  data_ptr;
  typedef typename interface_type::io_id_t   io_id_t;

  typedef typename interface_type::outgoing_handler_t outgoing_handler_t;
  typedef typename interface_type::incoming_handler_t incoming_handler_t;
  typedef typename interface_type::startup_handler_t  startup_handler_t;
  typedef typename interface_type::shutdown_handler_t shutdown_handler_t;


  virtual ~domain_object(){}
  
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
    CONFIG_LOG_MESSAGE("instance " << _name << " default START!!!")
    // TODO: LOG default (empty) start
  }

  virtual void stop(const std::string&)
  {
    // TODO: LOG default (empty) stop
  }

  virtual void reconfigure()
  {
    // Переконфигурация запущенного объекта!!
    // TODO: LOG default (empty) initialize
  }

  /*
  virtual void perform(const std::string&) 
  {
    
  }
  */
  
  virtual void startup_io(io_id_t , outgoing_handler_t )
  {
  }

  virtual void perform_io(data_ptr , io_id_t, outgoing_handler_t handler)
  {
    if ( handler!=nullptr )
      handler(nullptr);
  }
  
  virtual void shutdown_io(io_id_t )
  {
    
  }



private:  
  
  std::string _name;
  global_ptr _global;
  options_type _options;
};
  
}
