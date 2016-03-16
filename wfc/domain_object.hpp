#pragma once

#include <wfc/core/global.hpp>
#include <iow/owner/owner.hpp>
#include <iow/io/io_id.hpp>

#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>


namespace wfc{

template<
  typename Itf,
  typename Opt
>
class domain_object
  : public Itf
{
  // compile debug - убрать
  //typedef void interface_type;
public:
  typedef Opt options_type;
  typedef Itf domain_interface;
  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef ::iow::owner owner_type;
  
  typedef typename domain_interface::data_type data_type;
  typedef typename domain_interface::data_ptr  data_ptr;
  typedef typename domain_interface::io_id_t   io_id_t;

  typedef typename domain_interface::outgoing_handler_t outgoing_handler_t;
  typedef typename domain_interface::incoming_handler_t incoming_handler_t;
  typedef typename domain_interface::startup_handler_t  startup_handler_t;
  typedef typename domain_interface::shutdown_handler_t shutdown_handler_t;

  virtual ~domain_object(){}
  
  domain_object()
    : _started(false)
    , _io_id( ::iow::io::create_id<io_id_t>() )
  {}
  
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

  owner_type& owner()
  {
    return _owner;
  }

  template<typename H>
  auto wrap(H&& h) 
    -> ::iow::owner_handler<typename std::remove_reference<H>::type>
  {
    return _owner.wrap( std::forward<H>(h));
  }

  template<typename H, typename H2>
  auto wrap(H&& h, H2&& h2) 
    -> ::iow::owner_handler2<typename std::remove_reference<H>::type, typename std::remove_reference<H2>::type>
  {
    return _owner.wrap( std::forward<H>(h), std::forward<H2>(h2));
  }

  // TODO: callback, с проверкой одного обязательного вызова, сейчас синоним wrap
  template<typename H>
  auto callback(H&& h) 
    -> ::iow::owner_handler<typename std::remove_reference<H>::type>
  {
    return _owner.wrap( std::forward<H>(h));
  }

  template<typename H, typename H2>
  auto callback(H&& h, H2&& h2) 
    -> ::iow::owner_handler2<typename std::remove_reference<H>::type, typename std::remove_reference<H2>::type>
  {
    return _owner.wrap( std::forward<H>(h), std::forward<H2>(h2));
  }

  static void generate(options_type& opt, const std::string& /*type*/)
  {
    opt = options_type();
  }

  virtual void initialize(const std::string& name, global_ptr g, const options_type& opt) final
  {
    _name = name;
    _global = g;
    _options = opt;
    _owner.enable_callback_check(g->enable_callback_check);
    if ( !_started )
    {
      this->configure();
      _started = true;
    }
    else
    {
      this->reconfigure();
    }
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

  virtual void configure()
  {
    this->reconfigure();
  }
  
  virtual void reconfigure()
  {
    // Переконфигурация запущенного объекта!!
    // TODO: LOG default (empty) initialize
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

  constexpr io_id_t get_id() const { return _io_id;}
  
  std::string get_arg(const std::string& arg) const
  {
    if ( !_global->args.has(_name) )
      return std::string();
    
    auto args = _global->args.get(_name);
    if ( !args.has(arg) )
      return std::string();
    
    return args.get(arg);
  }

  int get_arg_int(const std::string& arg) const
  {
    if ( !_global->args.has(_name) )
      return 0;
    
    auto args = _global->args.get(_name);
    if ( !args.has(arg) )
      return 0;
    
    std::string sval = args.get(arg);
    if ( sval.empty() ) return 0;
    return atoi( sval.c_str() );
  }

  bool has_arg(const std::string& arg) const
  {
    if ( !_global->args.has(_name) )
      return false;
    
    auto args = _global->args.get(_name);
    return args.has(arg);
  }

private:
  bool _started;
  const io_id_t _io_id;
  std::string _name;
  global_ptr _global;
  options_type _options;
  owner_type _owner;
};
  
}
