#pragma once

#include <wfc/core/wfcglobal.hpp>
#include <iow/owner/owner.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/json.hpp>
#include <wfc/statistics/statistics.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/workflow.hpp>
#include <wfc/module/instance_options.hpp>
#include <wfc/module/idomain.hpp>
#include <wfc/module/instance_handler_.hpp>
#include <wfc/wfc_exit.hpp>

#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>


namespace wfc{

/**
 * @brief Класс для реализации прикладного объекта WFC
 * @tparam Itf тип интерфейса объекта ( на базе wfc::iinterface )
 * @tparam Opt произвольная структура с пользовательскими опциями 
 * @tparam StatOpt[=nostat]
 */
template<
  typename Itf,
  typename Opt,
  typename StatOpt = nostat
>
class domain_object
  : instance_handler_<Opt, StatOpt>
  , public idomain
  , public Itf
  
{
public:
  /** @brief Пользовательские опции */
  typedef Opt options_type;
  
  /** @brief Пользовательские опции для сбора статистики */
  typedef StatOpt statoptions_type;
  
  /** @brief Конфигурация инстанса прикладного объекта (пользовательские опции, опции статистики и опции wfc::instance) */
  typedef instance_options<Opt, StatOpt> config_type;
  
  /** @brief Только опции wfc::instance, без пользовательских */
  typedef typename config_type::basic_options basic_options;
  
  /** @brief Тип интерфейса объекта ( на базе wfc::iinterface ) */
  typedef Itf domain_interface;
  
  /** @brief Указатель на wfc::wfcglobal */
  typedef std::shared_ptr<wfcglobal> global_ptr;
  
  /** @brief Создает обертки для callback обработчиков */
  typedef ::iow::owner owner_type;
  
  /** @brief Ошибка JSON-десериализации  */
  typedef ::wfc::json::json_error json_error;
  
  /** @brief Уникальный идентификатор объекта */
  typedef typename domain_interface::io_id_t   io_id_t;
  
  /** @brief Очередь задач*/
  typedef ::wfc::workflow workflow_type;
  
  /** @brief std::shared_ptr<workflow_type> */
  typedef std::shared_ptr<workflow_type> workflow_ptr;
  
  /** @brief Для сбора статистики */
  typedef ::wfc::statistics::statistics statistics_type;
  
  /** @brief std::shared_ptr<statistics_type> */
  typedef std::shared_ptr<statistics_type> statistics_ptr;

  /** @brief Деструктор */
  virtual ~domain_object(){}
  
  /** @brief Конструктор */
  domain_object()
    : _io_id( ::iow::io::create_id<io_id_t>() )
  {}

  /**
   * @brief Возвращает имя объекта
   * @return имя объекта
   * @details Имя объекта задается при создании объекта из конфигурации. Изменить имя после создания объекта нельзя.
   */
  const std::string& name() const
  {
    return _name;
  }

  /**
   * @brief Возвращает указатель на wfc::wfcglobal
   * @return указатель на wfc::wfcglobal
   * @details по возможности следует использовать методы этого класса, а не обращаться к wfc::wfcglobal напрямую
   */
  global_ptr global() const 
  {
    if ( _global!=nullptr)
      return _global;
    else
      return wfcglobal::static_global;
  }
  
  /**
   * @brief Пользовательские настройки
   * @return ссылку на пользовательские настройки
   * @details Если доступна динамическая реконфигурация, то безопасно получить доступ 
   * к настройкам можно только в методах wfc::idomain, которые всегда вызываются фреймворком 
   * в основном потоке. В реализациях методах wfc::idomain::configure или wfc::idomain::reconfigure можно 
   * скопировать необходимые данные 
   */
  const options_type& options() const 
  {
    return _config;
  }
  
  /**
   * @brief Пользовательские настройки статистики
   * @return ссылку на опции статистики
   * @details Если доступна динамическая реконфигурация, то безопасно получить доступ 
   * к настройкам можно только в методах wfc::idomain, которые всегда вызываются фреймворком 
   * в основном потоке. В реализациях методах wfc::idomain::configure или wfc::idomain::reconfigure можно 
   * скопировать необходимые данные 
   */
  const statoptions_type& statistics_options() const
  {
    return _config.statistics;
  }

  /**
   * @brief Общая конфигурация объекта
   * @return ссылку на конфигурация объекта
   * @details Если доступна динамическая реконфигурация, то безопасно получить доступ 
   * к настройкам можно только в методах wfc::idomain, которые всегда вызываются фреймворком 
   * в основном потоке. В реализациях методах wfc::idomain::configure или wfc::idomain::reconfigure можно 
   * скопировать необходимые данные 
   */
  const config_type& config() const 
  {
    return _config;
  }

  
  /**
   * @brief Возвращает уникальный идентификатор объекта
   * @return уникальный идентификатор объекта
   * @details уникален для любого объекта в рамках процесса
   */
  constexpr io_id_t get_id() const
  {
    return _io_id;
  }
  
  
  /**
   * @brief Создает обертку над обработчиком обратного вызова 
   * @tparam H1 основной обработчик
   * @tparam H2 альтернативный обработчика
   * @param h1 исходный обработчик
   * @param h2 альтернативный обработчик (м.б. nullptr)
   * @details Если обработчик обратного взаимодействует с объектом, то может быть ситуация, что он будет вызван после уничтожения объекта.
   * Эта обертка распознает такую ситуацию и вызовет альтернативный обработчик, если задан, в противном случае просто не вызовет исходный.
   * Может использоватся для отправки сообщений в wfc::workflow::post
   */
  template<typename H1, typename H2>
  auto wrap(H1&& h1, H2&& h2) const
    -> typename wrap_result_<H1, H2>::type
  {
    return _owner.wrap( std::forward<H1>(h1), std::forward<H2>(h2));
  }

  /**
   * @brief Создает обертку над обработчиком обратного вызова RPC
   * @tparam H тип обработчика
   * @param h исходный обработчик
   * @details Обработчик обратного вызова RPC, должны быть гарантировано вызван только один раз. Если обработчик не вызывается, 
   * то клиентская сторона может ждать вечно ответе, также не должно быть повторных вызовов - это может привести к неопределенному 
   * поведению клиентского кода. Эта обертка детектирует оба случая и вызывает соответствующий обработчик, которые настраиваются  
   * глобально в модуле ядра.
   */
  template<typename H>
  auto callback(H&& h) const
    -> typename callback_result_<H>::type
  {
    return _owner.callback( std::forward<H>(h) );
  }

  
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


  
  bool system_is_stopped() const
  {
    if (auto g = this->global() )
      return g->stop_signal_flag;
    return false;
  }
  

  owner_type& owner()
  {
    return _owner;
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
  
  typedef instance_handler_<Opt, StatOpt> instance_handler_t;
  instance_handler_t* inst_handler_() 
  { return static_cast<instance_handler_<Opt, StatOpt>*>(this); }
private:
  virtual void domain_generate(config_type& conf, const std::string& type) final;
  virtual void create_domain(const std::string& objname, global_ptr g ) final;
  virtual void configure_domain(const config_type& opt) final;
  virtual void initialize_domain() final;
  virtual void reconfigure_domain_basic(const basic_options& opt) final;
  virtual void reconfigure_domain(const config_type& conf) final;
  virtual void ready_domain() final;
  virtual void start_domain() final;
  virtual void stop_domain() final;

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
  


template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::domain_generate(config_type& conf, const std::string& type) 
  {
    conf = this->generate(type);
  }

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::create_domain(const std::string& objname, global_ptr g ) 
  {
    _name = objname;
    _global = g;
    this->create();
  }

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::configure_domain(const config_type& opt)
  {
    _config = opt;
    _workflow = nullptr; // Ибо нефиг. До инициализации ничем пользоватся нельзя
    _conf_flag = false;
    if (auto g = this->global() )
      g->cpu.set_cpu(_name, opt.cpu);
    this->configure();
  }

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::initialize_domain() 
{
  if ( auto g = this->global() )
  {
    auto pname = std::make_shared<std::string>( this->name() );
    
    _owner.set_double_call_handler(std::bind(g->doublecall_handler, this->name()));
    _owner.set_no_call_handler(std::bind(g->nocall_handler, this->name()));
    /*
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
    */

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

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::reconfigure_domain_basic(const basic_options& opt)
  {
    _conf_flag = false;
    static_cast<basic_options&>(_config) = opt;
    if (auto g = this->global() )
      g->cpu.set_cpu(_name, opt.cpu);
    this->reconfigure_basic();
  }

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::reconfigure_domain(const config_type& conf)
  {
    _conf_flag = true;
    _config = conf;
    if (auto g = this->global() )
      g->cpu.set_cpu(_name, static_cast<basic_options&>(_config).cpu);
    this->reconfigure();
  }
  
template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::ready_domain() 
  {
    if ( _conf_flag )
      this->ready();
    _conf_flag = false;
  }


template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::start_domain()
  {
    this->start();
  }
  

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::stop_domain()
  {
    this->stop();
  }


}

