//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/wfcglobal.hpp>
#include <iow/owner/owner.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/json.hpp>
#include <wfc/statistics/statistics.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/workflow.hpp>
#include <wfc/module/domain_config.hpp>
#include <wfc/module/basic_domain.hpp>
#include <wfc/module/instance_handler_.hpp>
#include <wfc/wfc_exit.hpp>

#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>


namespace wfc{
  
/**
 * @brief Базовый класс для реализации прикладного объекта WFC
 * @tparam Itf тип интерфейса объекта ( на базе wfc::iinterface )
 * @tparam Opt произвольная структура с пользовательскими опциями 
 * @tparam StatOpt [=nostat] произвольная структура с пользовательскими опциями статистики
 * @details Прикладной объект в контексте WFC - это объект реализующий прикладную логику приложения.
 * Для облегчения интеграции в качестве базового используется этот класс (domain_object), который облегчает 
 * разработку и обеспечивает доступ к основным ресурсам WFC. Далее рассмотрим основные концепции прикладных объектов и
 * возможности domain_object. 
 *
 * Интерфейсы
 * ----------------
 * Любой прикладной объект должен реализовать API который описан в его интерфейсе - структура с набором чистых виртуальных 
 * методов на базе iinterface, которая позволяет регистрировать его в глобальном реестре объектов и получать к нему доступ. 
 * Эта структура передается в качестве первого шаблонного аргумента в domain_object. В общем случае других ограничений на интерфейсы,
 * кроме необходимости наследоваться от iinterface, нет. Однако, если предполагается удаленный вызов методов интерфейса прикладного 
 * объекта, а это то, для чего собственно и разрабатывался данный фреймворк, то на него накладываются ограничения, описанные в следующем
 * разделе. 
 *
 * API и асинхронные методы
 * ----------------
 * Рассматриваемые в данном разделе ограничения, следует воспринимать как требования, если вы хотите использовать реализованную во
 * фреймворке JSONRPC инфраструктуру. Или как набор рекомендаций, если вы захотите реализовать свой протокол взаимодействия, 
 * без потери совместимости.
 * - Все входящие параметры должны быть объедены в одну структуру(struct). Т.е. на входе метода всегда один параметр типа struct, 
 * в которой описываются все входящие переменные, даже если переменная одна или они отсутствуют (пустая структура). 
 * Это позволяет:
 *    - Добавлять или удалять параметры вызова без необходимости внесение изменения в интерфейс и реализацию
 *    - Добавлять параметры и реализовать логику без необходимости внесения изменения в декларации протокола взаимодействия 
 *      (например JSON-RPC). Т.е. сначала  реализовали, убедились что работает со значением по умолчанию, потом вносим 
 *      изменения в протокол
 *    - Добавлять параметры в структуру и декларацию протокола взаимодействия (например JSON-RPC) без реализации (игнорируем). 
 *      Т.е. объявляем клиенту, что появились новые параметры, убеждаемся что он правильно их использует, потом реализуем
 *      соответствующую логику.  
 *    - Кроме того это требование JSON-RPC инфраструктуры WFC (на самом деле нет - эти ограничения можно обойти, но я вам не 
 *      скажу как, так как это не имеет особого смысла и требует написания большего объема кода) 
 * - Все исходящие параметры также должны быть объедены в одну структуру по тем же причинам что и в предыдущем пункте
 * - Структура с входящими параметрами передается как std::unique_ptr
 *    - Это гарантирует перемещаемость объекта и запрещает случайное копирование 
 *    - Избавляет от необходимости накладывать ограничения на подобные структуры и/или использовать классы-обертки 
 *    - 
 * - Все методы должны быть асинхронными, т.е. возврат результата осуществляется через callback-функцию которая передается при вызове.
 *    - Однако это не означает, что реализация метода не может быть синхронный (вызов callback-функции сразу же после обработки 
 *      запроса )  
 *    
 * Опции
 * ----------------
 * Конфигурация и запуск
 * ----------------
 * Взаимодействие прикладных объектов (цели)
 * ----------------
 * get_id() callback() рекурсивные мутексы
 * 
 * Потоки и очереди (workflow)
 * ----------------
 * wrap()
 * 
 * Обработка параметров командной строки
 * ----------------
 * wfcglobal
 * ----------------
 * Статистика 
 * ----------------
 */
template<
  typename Itf,
  typename Opt,
  typename StatOpt = nostat
>
class domain_object
  : instance_handler_<Opt, StatOpt>
  , public basic_domain
  , public Itf
  
{
public:
  /** @brief Пользовательские опции (синоним options_type) */
  typedef Opt custom_options;
  
  /** @brief Пользовательские опции (синоним custom_options) */
  typedef custom_options options_type;
  
  /** @brief Пользовательские опции для сбора статистики */
  typedef StatOpt customstat_options;
  
  /** @brief Конфигурация прикладного объекта (пользовательские опции, опции статистики и опции wfc::instance) */
  typedef domain_config_t<Opt, StatOpt> domain_config;
  
  /** @brief Только опции wfc::instance, без пользовательских */
  typedef typename domain_config::domain_options domain_options;
  
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
  const custom_options& options() const 
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
  const customstat_options& statistics_options() const
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
  const domain_config& config() const 
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
   * @return обертка над обработчиком обратного вызова 
   * @details Если обработчик обратного взаимодействует с объектом, то может быть ситуация, что он будет вызван после уничтожения объекта.
   * Эта обертка распознает такую ситуацию и вызовет альтернативный обработчик, если задан, в противном случае просто не вызовет исходный.
   * Может использоваться для отправки сообщений в wfc::workflow::post
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
   * @return обертка над обработчиком обратного вызова RPC
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

  /**
   * @brief Возвращает ссылку на объект owner
   * @return ссылка на объект owner
   * @details вы можете сбросить ```owner().reset()```, тогда запросы обернутые ```wrap(...)``` решат, что объект уничтожен, 
   * либо повторно инициализировать обработчики для ```callback(...)``` обертки 
   */
  owner_type& owner()
  {
    return _owner;
  }

  
  /**
   * @brief Возвращает объект по имени в глобальном реестре
   * @tparam I интерфейс объекта (должен быть на базе iinerface)
   * @param target имя объекта
   * @param disabort [=false] отключить аварийное завершение, если объект не найден
   * @return указатель на объект из глобального реестра или nullptr
   * @details Осуществляет поиск в реестре без префикса - это как правило объекты прикладного уровня заданные 
   * в конфигурационном файле системы. Имена объектов и целей определяются пользователем в конфигурационном файле.
   * Двух объектов в системе с одинаковыми именами быть не может. 
   */
  template<typename I>
  std::shared_ptr<I> get_target(const std::string& target, bool disabort = false) const
  {
    if (!this->is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get<I>(target, disabort);
    return nullptr;
  }

  /**
   * @brief Возвращает объект по имени и префиксу в глобальном реестре
   * @tparam I интерфейс объекта (должен быть на базе iinerface)
   * @param prefix префикс объекта (aka пространство имен)
   * @param target имя объекта
   * @param disabort [=false] отключить аварийное завершение, если объект не найден
   * @return указатель на объект из глобального реестра или nullptr
   * @details Осуществляет поиск в реестре по префиксу и имени - это все прочие объекты, которые зарегистрированы 
   * объектами прикладного уровня. Такие объекты размещаются с определенным префиксом. Двух объектов в системе 
   * с одинаковыми именами под одним префиксом быть не может.
   */
  template<typename I>
  std::shared_ptr<I> get_target(const std::string& prefix, const std::string& target, bool disabort = false) const
  {
    if (!is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get<I>(prefix, target, disabort);
    return nullptr;
  }
  
  /**
   * @brief регистрирует объект в глобальном реестре
   * @param prefix префикс объекта (aka пространство имен)
   * @param tg_name имя объекта
   * @param pobj указатель на объект
   * @param nomark [=false] не отмечать в реестре, что были внесены изменения 
   * @details прикладные объекты могут регистрировать произвольное число объектов под разными префиксами для собственных нужд. Пустой префикс 
   * зарезервирован для прикладных объектов. Если объект является наследником этого класса и включен в список модулей пакета WFC то 
   * регистрировать его не нужно. Если зарегистрированные объекты используются только в контексте данного прикладного объекта, 
   * то можно задать ```cpp nomark=true``` чтобы избежать повторной инициализации всей системы
   * @see idomain
   */
  void set_target(const std::string& prefix, const std::string& tg_name, std::shared_ptr<iinterface> pobj, bool nomark = false)
  {
    if ( auto g = this->global() )
    {
      g->registry.set(prefix, tg_name, pobj, nomark);
    }
  }

  /**
   * @brief возвращает указатели на все объектов заданного префикса
   * @param prefix префикс объекта (aka пространство имен)
   * @return указатели на все объектов заданного префикса
   */
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
    
    
  /**
   * @brief Возвращает указатель на [workflow](https://mambaru.github.io/wflow/)
   * @return указатель на [workflow](https://mambaru.github.io/wflow/) или nullptr если система еще не сконфигурирована 
   * @details если задан [workflow](https://mambaru.github.io/wflow/) в конфигурации то этот указатель на этот workflow и возвращается, 
   * в противном случае возвращается общий workflow по умолчанию. Гарантированно результат не будет nullptr, после этапа конфигурации
   */
  std::shared_ptr<workflow_type> get_workflow() const 
  {
    if (!is_configured_())
      return nullptr;
    return _workflow;
  }

  /**
   * @brief Возвращает указатель на [workflow](https://mambaru.github.io/wflow/)
   * @param target имя объекта [workflow](https://mambaru.github.io/wflow/)
   * @param disabort [=false] отключить аварийное завершение, если объект не найден
   * @return указатель на [workflow](https://mambaru.github.io/wflow/) или nullptr
   * @details в базовой конфигурации для прикладного объекта всегда доступен [workflow](https://mambaru.github.io/wflow/), 
   * используйте этот метод если вам нужно более одного [workflow](https://mambaru.github.io/wflow/). Задайте disabort=true
   * если допустимо отсутствие объекта в реестре 
   */
  std::shared_ptr<workflow_type> get_workflow(const std::string& target, bool disabort = false) const 
  {
    if (!is_configured_())
      return nullptr;
    if ( target.empty() )
      return _workflow;
    return this->global()->registry.template get<workflow_type>("workflow", target, disabort);
  }

  /**
   * @brief Возвращает указатель на общий [workflow](https://mambaru.github.io/wflow/) системы
   * @return указатель на [workflow](https://mambaru.github.io/wflow/) или nullptr если система еще не сконфигурирована 
   * @details возвращается общий workflow системы. Гарантированно результат не будет nullptr, после этапа конфигурации.
   * Если в конфигурации объекта не задан workflow, то результат тот же что и get_workflow()
   */
  std::shared_ptr<workflow_type> get_common_workflow() const 
  {
    if (!is_configured_())
      return nullptr;
    if (auto g = this->global() )
      return g->common_workflow;
    return nullptr;
  }
  
  /**
   * @brief Добавляет фоновый обработчик с периодичностью один раз в секунду 
   * @param fun обработчик
   * @details Может вызываться только в методе start и работает до завершения работы приложения. 
   */
  void idle(std::function<void()> fun)
  {
    this->idle(std::chrono::seconds(1), fun);
  }

  /**
   * @brief Добавляет фоновый обработчик с заданной периодичностью 
   * @param duration интервал запуска
   * @param fun обработчик
   * @details Может вызываться только в методе start и работает до завершения работы приложения. 
   */
  void idle(workflow_type::duration_t duration, std::function<void()> fun)
  {
    if ( _idle_flag )
    {
      this->common_workflow()->create_timer(duration, fun);
    }
    else
    {
      SYSTEM_LOG_FATAL("Idle timer can be set only in 'start' method")
    }
  }

  /**
   * @brief Определяет наличие дополнительного аргумента переданного этому объекту через командную строку
   * @param arg имя аргумент
   * @return true если есть аргумент с именем arg
   * @details
   */
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

  
  /**
   * @brief Возвращает значение дополнительного аргумента переданного этому объекту через командную строку
   * @param arg имя аргумент
   * @return значение аргумента в виде строки
   * @details если аргумент arg отсутствует или не имеет значения, то возвращается пустая строка. Для определения 
   * наличия агумента без значений используете has_arg()
   */
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

  /**
   * @brief Возращает значение дополнительного аргумента переданного этому объекту через командную строку
   * @tparam T тип значения
   * @param arg имя аргумент
   * @param err сообщение об ошибке
   * @return значение аргумента или T()
   * @details если аргумент arg отсутствует или не имеет значения, то возвращается T().
   * Для определения наличия аргумента без значений используете has_arg()
   */
  template<typename T>
  T get_arg_t(const std::string& arg, std::string* err = nullptr) const
  {
    return this->get_arg_t_<T>(arg, err, fas::bool_<std::is_integral<T>::value>());
  }

  
  /**
   * @brief Регистрация пользовательского потока (thread)
   * @details Регистрация пользовательского потока нужна для того чтобы система могла более гибко распределять их по ядрам CPU.
   * Для того чтобы управлять распределением потоков для этого объекта нужно влючить wfc::component_features::EnableCPU. Вызывать
   * этот метод нужно непосредственно из запущенного потока
   */
  void reg_thread()
  {
    if (auto g = this->global() )
      g->cpu.set_current_thread(_name);
  }

  /**
   * @brief Снять регистрацию пользовательского потока (thread)
   * @details Вызыватьэтот метод нужно непосредственно из запущенного потока перед его завершением 
   */
  void unreg_thread()
  {
    if (auto g = this->global() )
      g->cpu.del_current_thread();
  }

  /**
   * @brief Возвращает true если объект в режиме suspend
   * @return true если объект в режиме suspend
   * @details В режиме suspend объект не обрабатывает запросы, а просто отвечает значением по умолчанию. Это помогает
   * при поисках проблемных мест с точки зрения производительности. 
   */
  bool suspended() const 
  {
    return _config.suspend;
  }

  /**
   * @brief Если объект в режиме suspend, вызывает callback со значением по умолчанию и возвращает true 
   * @tparam Res тип результата (параметр callback-функции)
   * @param cb callback-функция будет вызвана только если объект в режиме suspend (может быть nullptr)
   * @return true если объект в режиме suspend
   * @details callback-функция должна быть вызвана всегда, поэтому вы не можете просто проигнорировать входящий запрос. Этот метод позволяет 
   * упростить проверку на режим suspend и nullptr. На самом деле вам скорее понадобятся методы bad_request() и notify_ban() которые делают 
   * больше проверок
   */
  template<typename Res>
  bool suspended(const std::function< void(std::unique_ptr<Res>) > & cb) const
  {
    if ( !this->_config.suspend ) 
      return false;

    this->default_response(cb);
    return true;
  }

  /**
   * @brief Создает и отправляет ответ по умолчанию если функция cb!=nullptr
   * @tparam Res тип результата (параметр callback-функции)
   * @param cb callback-функция (может быть nullptr)
   */
  template<typename Res>
  void default_response(const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    if ( cb != nullptr )
      cb( std::make_unique<Res>() );
  }

  /**
   * @brief Проверяет на валидность входящий запрос, а также на режим suspend
   * @tparam Req тип запроса 
   * @tparam Res тип результата (параметр callback-функции)
   * @param req входящий запрос
   * @param cb callback-функция (может быть nullptr)
   * @return true если дальнейшая обработка не требуется, false можно обрабатывать запрос
   * @details Реализует большинство необходимых проверок при обработке запроса:
   * - на режим suspend, с отправкой ответ 
   * - на останов системы
   * - на req==nullptr c отправкой cb(nullptr)
   * Если это метод вернул true это значит, что вызов cb уже произведен
   * ```cpp
   *  void foo_domain::bar(request::bar::ptr req, response::bar::handler cb )
   *  {
   *    if ( this->bad_request(req, cb) )
   *      return;
   *    ... обработка запроса или уведомления ...
   *  }
   * ```
   * 
   */
  template<typename Req, typename Res>
  bool bad_request(const std::unique_ptr<Req>& req, const std::function< void(std::unique_ptr<Res>) >& cb) const
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
  
  /**
   * @brief Запрещает уведомления, проверяет на валидность входящий запрос, а также на режим suspend
   * @tparam Req тип запроса 
   * @tparam Res тип результата (параметр callback-функции)
   * @param req входящий запрос
   * @param cb callback-функция (может быть nullptr)
   * @return true если дальнейшая обработка не требуется, false можно обрабатывать запрос
   * @details Все тоже самое, что и bad_request(), но запрещает также уведомления (это когда cb==nullptr). Это значит, 
   * что после вызова этой функции, если она вернула false, cb на nullptr можно не проверять. 
   */
  template<typename Req, typename Res>
  bool notify_ban(const std::unique_ptr<Req>& req, const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    return cb==nullptr || this->bad_request(req, cb);
  }
  
  /**
   * @brief Разрешает только уведомления, проверяет на валидность входящий запрос, а также на режим suspend
   * @tparam Req тип запроса 
   * @tparam Res тип результата (параметр callback-функции)
   * @param req входящий запрос
   * @param cb callback-функция (может быть nullptr)
   * @return true если дальнейшая обработка не требуется, false можно обрабатывать запрос
   * @details Все тоже самое, что и bad_request(), но разрешает только уведомления (это когда cb==nullptr). Это значит, 
   * что после вызова этой функции, если она вернула false, cb == nullptr. На запрос вызывает cb(nullptr), что означает 
   * сервис недоступен.
   */
  template<typename Req, typename Res>
  bool request_ban(const std::unique_ptr<Req>& req, const std::function< void(std::unique_ptr<Res>) >& cb) const
  {
    if ( this->bad_request(req, cb) )
      return true;

    if ( cb==nullptr )
      return false;

    cb(nullptr);
 
    return true;
  }
  
  /**
   * @brief вспомогательная функция для обработчиков, которые можут обрабатывать как запросы так и уведомления
   * @tparam Res тип результата (параметр callback-функции)
   * @param cb callback-функция (может быть nullptr)
   * @return nullptr если это уведомление (cb==nullptr) и объект запроса в противном случае  
   * ```cpp
   *  void foo_domain::bar(request::bar::ptr req, response::bar::handler cb )
   *  {
   *    if ( this->bad_request(req, cb) )
   *      return;
   * 
   *    ... обработка запроса или уведомления ...
   * 
   *    if (auto res = this->create_response(cb) )
   *    {
   *       ... инициализация res ...   
   *       cb( std::move(res) )
   *    }
   *  }
   * ```
   */
  template<typename Res>
  static std::unique_ptr<Res> create_response( const std::function<void(std::unique_ptr<Res>)>& cb)
  {
    if ( cb != nullptr )
      return std::make_unique<Res>();
    return nullptr;
  }

  /**
   * @brief вспомогательная функция для обработчиков, которые можут обрабатывать как запросы так и уведомления
   * @tparam Res тип результата (параметр callback-функции)
   * @param res объект результата для отправки (может быть nullptr)
   * @param cb callback-функция (может быть nullptr)
   * @details С помощью create_response() создается объект ответа для запроса и nullptr для уведомлений,
   * чтобы избавится от лишних проверок cb, можно воспользоваться этой функцией. Удобно использовать если 
   * для ответов не требующих инициализации (пустые структуры исключительно для подтверждения )
   * ```cpp
   *  void foo_domain::set(request::set::ptr req, response::set::handler cb )
   *  {
   *    if ( this->bad_request(req, cb) )
   *      return;
   *
   *    ... обработка ...
   * 
   *    auto res = this->create_response(cb);
   *    this->send_response( std::move(res), cb)
   *  }
   * ```
   */
  template<typename Res>
  static void send_response(std::unique_ptr<Res>&& res, const std::function< void(std::unique_ptr<Res>) >& cb)
  {
    if (res!=nullptr && cb!=nullptr)
      cb( std::move(res) );
  }

  /**
   * @brief Возвращает объект для сбора, агрегации и отправки статистики
   * @return объект для сбора, агрегации и отправки статистики или nullptr если статистика отключена или не поддерживается
   * @details На данный момент поддержка сбора статистики по умолчанию отключена. Для ее включения требуется перекомпиляция с 
   * флагом -DWFC_ENABLE_STAT=ON
   */
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

  /**
   * @brief Проверка системы на сигнал останова
   * @return true если получен сигнал прекращения работы
   * @details Может быть использования при длительных процедурах инициализации (например загрузка БД), 
   * чтобы завершить работу, а не дожидаться завершения инициализации
   */
  bool system_is_stopped() const
  {
    if (auto g = this->global() )
      return g->stop_signal_flag;
    return false;
  }
  
  /**
   * @brief Генератор конфигурации
   * @param arg параметр конфигурации 
   * @return объект для сериализации config_type()
   * @details Вы можете переопределить этот метод для генерации демонстрационных конфигураций для различных arg, но для arg=="" 
   * ответ должен быть всегда config_type(). В текущей реализации для arg=="example" заполняется массив cpu. Если arg=="" это значит
   * что метод используется для генерации конфигурации всей системы, которая получается сериализацией объектов, которые возвращает 
   * этот метод. В этом случае допустимы только значения по умолчанию в конфигурационных структурах, чтобы пользователь мог удалить 
   * поля из json-конфигурации если его устраивают значения по умолчанию, для упрощения 
   */
  virtual domain_config generate(const std::string& arg) 
  {
    domain_config conf = domain_config();
    if ( arg=="example" )
    {
      conf.cpu.insert(0);
      conf.cpu.insert(1);
      conf.cpu.insert(2);
    }
    return conf;
  }

  typedef instance_handler_<Opt, StatOpt> instance_handler_t;
  instance_handler_t* inst_handler_() 
  { return static_cast<instance_handler_<Opt, StatOpt>*>(this); }
  
private:
  virtual void domain_generate(domain_config& conf, const std::string& type) final;
  virtual void create_domain(const std::string& objname, global_ptr g ) final;
  virtual void configure_domain(const domain_config& opt) final;
  virtual void initialize_domain() final;
  virtual void reconfigure_domain_basic(const domain_options& opt) final;
  virtual void reconfigure_domain(const domain_config& conf) final;
  virtual void restart_domain() final;
  virtual void start_domain() final;
  virtual void stop_domain() final;

private:
  
  template<typename T>
  T get_arg_t_(const std::string& arg, std::string* err, fas::true_) const
  {
    T val = T();
    std::string str = this->get_arg(arg);
    if ( str.empty() )
      return val;
    
    typedef typename json::value<T>::serializer serializer;
    json_error e;
    serializer()(val, str.begin(), str.end(), &e );
    if ( e && err!=nullptr )
      *err = json::strerror::message(e);
    return val;
  }

  template<typename T>
  T get_arg_t_(const std::string& arg, std::string* err, fas::false_) const
  {
    static_assert( std::is_same<T, std::string>::value, "Only for integral types or std::string." );
    std::string val;
    std::string str = this->get_arg(arg);
    if ( str.empty() )
      return val;
    
    if ( str[0]!='\'' && str[0]!='\"' )
      return str;
    
    std::replace(str.begin(), str.end(), '\'', '\"');
    
    typedef typename json::value< std::string >::serializer serializer;
    json_error e;
    serializer()(val, str.begin(), str.end(), &e );
    if ( e && err!=nullptr )
      *err = json::strerror::message(e);
    return val;
  }

  bool is_configured_() const
  {
    if ( _workflow!=nullptr )
      return true;
    DOMAIN_LOG_FATAL("Attempting to use is not initialized object '" << _name <<"'")
    return false;
  }
  
private:
  bool _conf_flag = false;
  bool _idle_flag = false;
  const io_id_t  _io_id;
  std::string    _name;
  global_ptr     _global;
  domain_config  _config;
  owner_type     _owner;
  workflow_ptr   _workflow;
  statistics_ptr _statistics;
};
  


template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::domain_generate(domain_config& conf, const std::string& type) 
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
void domain_object<Itf, Opt, StatOpt>::configure_domain(const domain_config& opt)
{
  _config = opt;
  _workflow = nullptr; // Ибо нефиг. До инициализации ничем пользоваться нельзя
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
void domain_object<Itf, Opt, StatOpt>::reconfigure_domain_basic(const domain_options& opt)
{
  _conf_flag = false;
  static_cast<domain_options&>(_config) = opt;
  if (auto g = this->global() )
    g->cpu.set_cpu(_name, opt.cpu);
  this->reconfigure_basic();
}

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::reconfigure_domain(const domain_config& conf)
{
  _conf_flag = true;
  _config = conf;
  if (auto g = this->global() )
    g->cpu.set_cpu(_name, static_cast<domain_options&>(_config).cpu);
  this->reconfigure();
}
  
template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::restart_domain() 
{
  if ( _conf_flag )
    this->restart();
  _conf_flag = false;
}

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::start_domain()
{
  _idle_flag = true;
  this->start();
  _idle_flag = false;
}
  
template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::stop_domain()
{
  this->stop();
}

}

