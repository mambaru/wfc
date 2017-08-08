#pragma once

#include <wfc/core/global.hpp>
#include <iow/owner/owner.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/json.hpp>
#include <wfc/statistics/statistics.hpp>
#include <wfc/workflow.hpp>
#include <wfc/module/instance_options.hpp>
#include <wfc/wfc_exit.hpp>

#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>


namespace wfc{

template<
  typename Itf,
  typename Opt,
  typename StatOpt = nostat
>
class domain_object
  : public Itf
{
public:
  typedef Opt options_type;
  typedef StatOpt statistics_options_type;
  typedef instance_options<Opt, StatOpt> config_type;
  typedef typename config_type::basic_options basic_options;

  typedef Itf domain_interface;
  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef ::iow::owner owner_type;
  typedef ::wfc::json::json_error json_error;
  
  typedef typename domain_interface::data_type data_type;
  typedef typename domain_interface::data_ptr  data_ptr;
  typedef typename domain_interface::io_id_t   io_id_t;

  typedef typename domain_interface::outgoing_handler_t outgoing_handler_t;
  typedef typename domain_interface::incoming_handler_t incoming_handler_t;
  typedef typename domain_interface::startup_handler_t  startup_handler_t;
  typedef typename domain_interface::shutdown_handler_t shutdown_handler_t;
  
  typedef ::wfc::workflow workflow_type;
  typedef std::shared_ptr<workflow_type> workflow_ptr;
  typedef workflow_type::timer_id_t timer_id_t;

  typedef ::wfc::statistics statistics_type;
  typedef std::shared_ptr<statistics_type> statistics_ptr;

  virtual ~domain_object(){}
  
  domain_object()
    : _io_id( ::iow::io::create_id<io_id_t>() )
  {}

  virtual config_type generate(const std::string&) 
  {
    return config_type();
  }
  
  virtual void create() {}

  virtual void configure() { this->reconfigure(); }

  virtual void initialize() {}

  // Только при первом запуске
  virtual void start() { this->ready(); }

  // Каждый раз при реконфигурации или переинициализации
  virtual void ready() {};
  
  virtual void stop( )  {}

  virtual void reconfigure() { }

  virtual void reconfigure_basic() {}

  ///
  /// -----------------------------------------------------------------
  /// 
  
  virtual void domain_generate(config_type& conf, const std::string& type) final
  {
    conf = this->generate(type);
  }

  virtual void create_domain(const std::string& name, global_ptr g ) final
  {
    _name = name;
    _global = g;
    this->create();
  }

  // вызываеться один раз между create_domain и initialize_domain
  virtual void configure_domain(const config_type& opt) final
  {
    _config = opt;
    _workflow = nullptr; // Ибо нефиг. До инициализации ничем пользоватся нельзя
    _conf_flag = false;
    this->configure();
  }

  // первый раз после configure_domain, последующие после reconfigure_domain или reconfigure_basic
  virtual void initialize_domain() final
  {
    _owner.enable_callback_check(_global->enable_callback_check);
    _workflow = _config.workflow.empty()
                ? this->global()->workflow
                : this->global()->registry.template get<workflow >("workflow", _config.workflow)
                ;

    _statistics = ! _config.statistics.disabled 
                  ? this->global()->registry.template get<statistics>("statistics", _config.statistics.target)
                  : nullptr
                  ;

    basic_options bopt = static_cast<basic_options>(_config);
    _global->cpu.set_cpu(_name, bopt.cpu);
    _conf_flag = true;
    this->initialize();
  }

  virtual void reconfigure_domain_basic(const basic_options& opt) final
  {
    _conf_flag = false;
    static_cast<basic_options&>(_config) = opt;
    this->reconfigure_basic();
  }

  virtual void reconfigure_domain(const config_type& conf) final
  {
    _conf_flag = true;
    _config = conf;
    this->reconfigure();
  }
  
  // только после реконфигурации и инициализации
  virtual void ready_domain() final
  {
    if ( _conf_flag )
      this->ready();
    _conf_flag = false;
  }

  // только один раз после конфигурации и инициализации
  virtual void start_domain() final
  {
    this->start();
  }
  

  virtual void stop_domain() final
  {
    this->stop();
  }

  const std::string& name() const
  {
    return _name;
  }

  global_ptr global() const 
  {
    return _global;
  }
  
  bool system_is_stopped() const
  {
    if ( auto g = _global )
      return g->stop_signal_flag;
    return false;
  }
  
  const options_type& options() const 
  {
    return _config;
  }

  const config_type& config() const 
  {
    return _config;
  }
  
  const statistics_options_type& statistics_options() const
  {
    return _config.statistics;
  }

  owner_type& owner()
  {
    return _owner;
  }

  template<typename H, typename H2>
  auto wrap(H&& h, H2&& h2) const
    -> ::iow::owner_handler<typename std::remove_reference<H>::type, typename std::remove_reference<H2>::type>
  {
    return _owner.wrap( std::forward<H>(h), std::forward<H2>(h2));
  }


  template<typename H, typename H2>
  auto callback(H&& h, H2&& h2) const
    -> ::iow::owner_handler<typename std::remove_reference<H>::type, typename std::remove_reference<H2>::type>
  {
    return _owner.wrap( std::forward<H>(h), std::forward<H2>(h2));
  }

  template<typename I>
  std::shared_ptr<I> get_target(const std::string& prefix, const std::string& name, bool disabort = false) const
  {
    if (!is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get<I>(prefix, name, disabort);
    return nullptr;
  }
  
  template<typename I>
  std::shared_ptr<I> get_target(const std::string& name, bool disabort = false) const
  {
    if (!is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get<I>(name, disabort);
    return nullptr;
  }

  statistics_ptr get_statistics() const 
  {
    if (!is_configured_())
      return nullptr;

    return _statistics;
  }

  std::shared_ptr<workflow_type> get_workflow() const 
  {
    if (!is_configured_())
      return nullptr;
    return _workflow;
  }

  std::shared_ptr<workflow_type> get_common_workflow() const 
  {
    if (!is_configured_())
      return nullptr;
    return this->global()->workflow;
  }

  std::shared_ptr<workflow_type> get_workflow(const std::string& name, bool disabort = false) const 
  {
    if (!is_configured_())
      return nullptr;
    return this->global()->registry.template get<workflow_type>("workflow", name, disabort);
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

  constexpr io_id_t get_id() const
  {
    return _io_id;
  }
  
  std::string get_arg(const std::string& arg) const
  {
    if ( !_global->args.has(_name) )
      return std::string();
    
    auto args = _global->args.get(_name);
    if ( !args.has(arg) )
      return std::string();
    
    return args.get(arg);
  }

  template<typename T>
  T get_arg_t(const std::string& arg, json_error* e = nullptr) const
  {
    T val = T();
    auto str = this->get_arg(arg);
    if ( str.empty() )
      return val;
    
    typedef typename ::wfc::json::value<T>::serializer serializer;
    serializer()(val, str.begin(), str.end(), e );
    
    return val;
  }

  bool has_arg(const std::string& arg) const
  {
    if ( !_global->args.has(_name) )
      return false;
    
    auto args = _global->args.get(_name);
    return args.has(arg);
  }
  
  void reg_thread()
  {
    _global->cpu.set_current_thread(_name);
  }

  void unreg_thread()
  {
    _global->cpu.del_current_thread();
  }

  bool suspended() const 
  {
    return _config.suspend;
  }

  template<typename Res, typename Req, typename Callback>
  bool suspended(const Req& /*req*/, const Callback& cb) const
  {
    if ( !this->_config.suspend ) 
      return false;

    this->default_response<Res>(cb);
    return true;
  }

  template<typename Res, typename Callback>
  void default_response(const Callback& cb) const
  {
    if ( cb != nullptr )
      cb( std::make_unique<Res>() );
  }

  template<typename Res, typename Req, typename Callback>
  bool bad_request(const Req& req, const Callback& cb) const
  {
    if ( this->suspended<Res>(req, cb) ) 
      return true;
    
    if ( req!=nullptr )
      return false;
    
    if ( cb!=nullptr )
      cb(nullptr);
    
    return true;
  }
  
  template<typename Res, typename Req, typename Callback>
  bool notify_ban(const Req& req, const Callback& cb) const
  {
    return cb==nullptr || this->bad_request<Res>(req, cb);
  }
  
  template<typename Res, typename Req, typename Callback>
  bool request_ban(const Req& req, const Callback& cb) const
  {
    if ( this->bad_request<Res>(req, cb) )
      return true;

    if ( cb==nullptr )
      return false;

    cb(nullptr);
 
    return true;
  }
private:
  bool is_configured_() const
  {
    if ( _workflow!=nullptr )
      return true;
    DOMAIN_LOG_FATAL("Attempting to use is not initialized object '" << _name <<"'")
    return false;
  }
private:
  bool _conf_flag = false;
  const io_id_t  _io_id;
  std::string    _name;
  global_ptr     _global;
  config_type    _config;
  owner_type     _owner;
  workflow_ptr   _workflow;
  statistics_ptr _statistics;
};
  
}
