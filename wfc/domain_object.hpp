#pragma once

#include <wfc/core/global.hpp>
#include <iow/owner/owner.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/json.hpp>
#include <wfc/statistics/statistics.hpp>
#include <wfc/iinterface.hpp>
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
  typedef StatOpt statoptions_type;
  typedef instance_options<Opt, StatOpt> config_type;
  typedef typename config_type::basic_options basic_options;

  typedef Itf domain_interface;
  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef ::iow::owner owner_type;
  typedef ::wfc::json::json_error json_error;
  
  typedef typename domain_interface::data_type data_type;
  typedef typename domain_interface::data_ptr  data_ptr;
  typedef typename domain_interface::io_id_t   io_id_t;

  typedef typename domain_interface::output_handler_t output_handler_t;
  typedef typename domain_interface::input_handler_t input_handler_t;
  
  typedef ::wfc::workflow workflow_type;
  typedef std::shared_ptr<workflow_type> workflow_ptr;
  typedef workflow_type::timer_id_t timer_id_t;

  typedef ::wfc::statistics::statistics statistics_type;
  typedef std::shared_ptr<statistics_type> statistics_ptr;

  virtual ~domain_object(){}
  
  domain_object()
    : _io_id( ::iow::io::create_id<io_id_t>() )
  {}

  virtual config_type generate(const std::string& arg) 
  {
    config_type conf =config_type();
    if ( arg=="example" )
    {
      conf.cpu.insert(0);
      conf.cpu.insert(1);
      conf.cpu.insert(2);
    }
    return conf;
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

  virtual void create_domain(const std::string& objname, global_ptr g ) final
  {
    _name = objname;
    _global = g;
    this->create();
  }

  // вызываеться один раз между create_domain и initialize_domain
  virtual void configure_domain(const config_type& opt) final
  {
    _config = opt;
    _workflow = nullptr; // Ибо нефиг. До инициализации ничем пользоватся нельзя
    _conf_flag = false;
    if (auto g = this->global() )
      g->cpu.set_cpu(_name, opt.cpu);
    this->configure();
  }

  // первый раз после configure_domain, последующие после reconfigure_domain или reconfigure_basic
  virtual void initialize_domain() final
  {
    if ( auto g = this->global() )
    {
      auto pname = std::make_shared<std::string>( this->name() );
      _owner.set_double_call_handler([g, pname]()
      {
        if ( g->stop_signal_flag )
          return;
        
        if ( g->double_callback_abort )
        {
          DOMAIN_LOG_FATAL("Double call callback functions for '" << *pname << "'")
        }
        else if ( g->double_callback_show )
        {
          DOMAIN_LOG_ERROR("Double call callback functions for '" << *pname << "'")
        }
      });

      _owner.set_no_call_handler([g, pname]()
      {
        if ( g->stop_signal_flag )
          return;

        if ( g->nocall_callback_abort )
        {
          DOMAIN_LOG_FATAL("Lost call callback for '" << *pname << "'")
        }
        else if ( g->nocall_callback_show )
        {
          DOMAIN_LOG_ERROR("Lost callback for '" << *pname << "'")
        }
      });

      _workflow = _config.workflow.empty()
                  ? this->global()->common_workflow
                  : this->global()->registry.template get<workflow >("workflow", _config.workflow)
                  ;

      _statistics = ! _config.statistics.disabled 
                    ? this->global()->registry.template get<statistics::statistics>("statistics", _config.statistics.target)
                    : nullptr
                    ;

      _conf_flag = true;
    }
    this->initialize();
  }

  virtual void reconfigure_domain_basic(const basic_options& opt) final
  {
    _conf_flag = false;
    static_cast<basic_options&>(_config) = opt;
    if (auto g = this->global() )
      g->cpu.set_cpu(_name, opt.cpu);
    this->reconfigure_basic();
  }

  virtual void reconfigure_domain(const config_type& conf) final
  {
    _conf_flag = true;
    _config = conf;
    if (auto g = this->global() )
      g->cpu.set_cpu(_name, static_cast<basic_options&>(_config).cpu);
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
    if ( _global!=nullptr)
      return _global;
    else
      return wfcglobal::static_global;
  }
  
  bool system_is_stopped() const
  {
    if (auto g = this->global() )
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
  
  const statoptions_type& statistics_options() const
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


  template<typename H>
  auto callback(H&& h) const
    -> ::iow::callback_handler<typename std::remove_reference<H>::type>
  {
    return _owner.callback( std::forward<H>(h) );
  }

  template<typename I>
  std::shared_ptr<I> get_target(const std::string& prefix, const std::string& tg_name, bool disabort = false) const
  {
    if (!is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get<I>(prefix, tg_name, disabort);
    return nullptr;
  }
  
  template<typename I>
  std::shared_ptr<I> get_target(const std::string& tg_name, bool disabort = false) const
  {
    if (!this->is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get<I>(tg_name, disabort);
    return nullptr;
  }

  template<typename I>
  std::map<std::string, std::shared_ptr<I> > select_targets(const std::string& prefix) const
  {
    std::map<std::string, std::shared_ptr<I> > result;
    if ( this->is_configured_() )
    {
      if ( auto g = this->global() )
        result = g->registry.template select<I>(prefix);
    }
    return result;
  }
  
  void set_target(const std::string& prefix, const std::string& tg_name, std::shared_ptr<iinterface> p, bool nomark = false)
  {
    if ( auto g = this->global() )
    {
      g->registry.set(prefix, tg_name, p, nomark);
    }
  }
  
  statistics_ptr get_statistics() const 
  {
    if ( auto g = this->global() )
    {
      if ( g->disable_statistics )
        return nullptr;
    }

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

  std::shared_ptr<workflow_type> get_workflow(const std::string& wf_name, bool disabort = false) const 
  {
    if (!is_configured_())
      return nullptr;
    return this->global()->registry.template get<workflow_type>("workflow", wf_name, disabort);
  }

  virtual void reg_io(io_id_t , std::weak_ptr<iinterface> ) override
  {
  }

  virtual void perform_io(data_ptr , io_id_t, output_handler_t handler) override
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
    if (auto g = this->global() )
    {
      if ( !g->args.has(_name) )
        return std::string();
      
      auto args = g->args.get(_name);
      if ( !args.has(arg) )
        return std::string();
      
      return args.get(arg);
    }
    return std::string();
  }

  template<typename T>
  T get_arg_t(const std::string& arg, json_error* e = nullptr) const
  {
    T val = T();
    std::string str = this->get_arg(arg);
    if ( str.empty() )
      return val;
    
    typedef typename json::value<T>::serializer serializer;
    serializer()(val, str.begin(), str.end(), e );
    
    return val;
  }

  bool has_arg(const std::string& arg) const
  {
    if (auto g = this->global() )
    {
      if ( !g->args.has(_name) )
        return false;
      
      auto args = g->args.get(_name);
      return args.has(arg);
    }
    return false;
  }
  
  void reg_thread()
  {
    if (auto g = this->global() )
      g->cpu.set_current_thread(_name);
  }

  void unreg_thread()
  {
    if (auto g = this->global() )
      g->cpu.del_current_thread();
  }

  bool suspended() const 
  {
    return _config.suspend;
  }

  template<typename Res>
  bool suspended(const std::function< void(std::unique_ptr<Res>) > & cb) const
  {
    if ( !this->_config.suspend ) 
      return false;

    this->default_response(cb);
    return true;
  }

  template<typename Res>
  void default_response(const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    if ( cb != nullptr )
      cb( std::make_unique<Res>() );
  }

  template<typename Req, typename Res>
  bool bad_request(const Req& req, const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    if ( this->suspended(cb) ) 
      return true;

    if ( this->system_is_stopped() ) 
      return true;
    
    if ( req!=nullptr )
      return false;
    
    if ( cb!=nullptr )
      cb(nullptr);
    
    return true;
  }
  
  template<typename Req, typename Res>
  bool notify_ban(const Req& req, const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    return cb==nullptr || this->bad_request(req, cb);
  }
  
  template<typename Req, typename Res>
  bool request_ban(const Req& req, const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    if ( this->bad_request(req, cb) )
      return true;

    if ( cb==nullptr )
      return false;

    cb(nullptr);
 
    return true;
  }
  
  template<typename Res>
  static std::unique_ptr<Res> create_response( const std::function<void(std::unique_ptr<Res>)>& cb)
  {
    if ( cb != nullptr )
      return std::make_unique<Res>();
    return nullptr;
  }
  
  template<typename Res>
  static void send_response(std::unique_ptr<Res>&& res, const std::function< void(std::unique_ptr<Res>) >& cb)
  {
    if (res!=nullptr && cb!=nullptr)
      cb( std::move(res) );
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
