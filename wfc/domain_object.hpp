//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/wfcglobal.hpp>
#include <wfc/json.hpp>
#include <wfc/statistics/statistics.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/workflow.hpp>
#include <wfc/module/domain_config.hpp>
#include <wfc/module/basic_domain.hpp>
#include <wfc/module/instance_handler_.hpp>
#include <wfc/wfc_exit.hpp>

#include <wflow/owner.hpp>
#include <iow/io/io_id.hpp>

#include <wjson/schema.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

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
  typedef wflow::owner owner_type;

  /** @brief Ошибка JSON-десериализации  */
  typedef wjson::json_error json_error;

  /** @brief Уникальный идентификатор объекта */
  typedef typename domain_interface::io_id_t   io_id_t;

  typedef typename domain_interface::data_type data_type; 
  typedef typename domain_interface::data_ptr data_ptr; 
  typedef typename domain_interface::output_handler_t output_handler_t; 
  
  /** @brief Очередь задач*/
  typedef wflow::workflow workflow_type;

  typedef workflow_type::timer_id_t timer_id_t;

  /** @brief std::shared_ptr<workflow_type> */
  typedef std::shared_ptr<workflow_type> workflow_ptr;

  /** @brief Для сбора статистики */
  typedef ::wfc::statistics::statistics statistics_type;

  /** @brief std::shared_ptr<statistics_type> */
  typedef std::shared_ptr<statistics_type> statistics_ptr;

  /** @brief Деструктор */
  virtual ~domain_object()
  {
    _global = nullptr;
  }

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
  virtual io_id_t get_id() const override final
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
   * @details Если обработчик взаимодействует с этим объектом, то может быть ситуация, что он будет вызван после уничтожения объекта.
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
   * @brief Создает обертку над обработчиком для постановки в очередь
   * @tparam H1 основной обработчик
   * @tparam H2 альтернативный обработчика
   * @param io_id идентификатор трека (можно создать с помощю iow::create_id)
   * @param h1 исходный обработчик
   * @param h2 альтернативный обработчик (м.б. nullptr)
   * @details Данная обертка позволяет сбрасывать все сообщения трека с помощью release_tracking
   */
  template<typename H1, typename H2>
  auto tracking(io_id_t io_id, H1&& h1, H2&& h2)
    -> typename wrap_result_<H1, H2>::type
  {
    return _owner.tracking(io_id, std::forward<H1>(h1), std::forward<H2>(h2));
  }

  /**
   * @brief Сбрасывает все обработчики трека
   * @param io_id идентификатор трека
   * @details для всех обработчиков для которых был использован tracking с заданым io_id, будет применен альтернативный метод при обращении к нему.
   * Может быть использован, чтобы выкинуть группу сообщений из очереди
   */
  void release_tracking(io_id_t io_id)
  {
    _owner.release_tracking(io_id);
  }

  /**
   * @brief Создает обертку над обработчиком обратного вызова RPC
   * @tparam H тип обработчика
   * @param h исходный обработчик
   * @return обертка над обработчиком обратного вызова RPC
   * @details Обработчик обратного вызова RPC, должны быть гарантировано вызван только один раз. Если обработчик не вызывается,
   * то клиентская сторона может ждать вечно ответа, также не должно быть повторных вызовов - это может привести к неопределенному
   * поведению клиентского кода. Эта обертка детектирует оба случая и вызывает соответствующий обработчик, который настраиваются
   * глобально в модуле ядра.
   */
  template<typename H>
  auto callback(H&& h) const
    -> typename callback_result_<H>::type
  {
    return _owner.callback( std::forward<H>(h) );
  }

  /**
   * @details При включенной опции component_features::EnableTracking вызов этой функции обязателен
   * в порожденном классе при ее переопределении, чтобы трекер корректно работал.
   */
  virtual void unreg_io(io_id_t io_id) override
  {
    _owner.release_tracking(io_id);
  }

  bool perform_status(data_ptr& d, output_handler_t& handler)
  {
    if ( handler == nullptr )
      return false;
    
    if ( d == nullptr )
    {
      handler(nullptr);
      return true;
    }
    
    if ( this->suspended() )
    {
      handler( iow::io::make("SUSPENDED") );
      return true;
    }
    
    if ( d->size() < 7 )
      return false;

    std::istringstream iss( std::string( d->begin(), d->end()) );

    std::string command;
    iss >> command;

    if ( command != "@STATUS" )
      return false;

    size_t errors = 0, warnings = 0;

    if ( iss >> errors )
      iss >> warnings;

    //  Модуль ядра всегда есть, отключаем abort  для тестирования
    if ( auto pcore = this->get_target<icore>("core", true) )
    {
      std::string statustext = pcore->get_status_text(errors, warnings);
      handler( iow::io::make(statustext) );
    }
    else
      handler( iow::io::make("OK") );
    return true;
  }

  virtual void perform_io(data_ptr d, io_id_t id, output_handler_t handler) override
  {
    if ( this->perform_status(d, handler) )
      return;
   
    domain_interface::perform_io(std::move(d), id, handler);
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
      return g->registry.template get_target<I>(target, disabort);
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
  std::shared_ptr<I> get_object(const std::string& prefix, const std::string& target, bool disabort = false) const
  {
    if (!is_configured_())
      return nullptr;

    if ( auto g = this->global() )
      return g->registry.template get_object<I>(prefix, target, disabort);
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
  void reg_object(const std::string& prefix, const std::string& tg_name, std::shared_ptr<iinterface> pobj, bool nomark = false)
  {
    if ( auto g = this->global() )
    {
      g->registry.set_object(prefix, tg_name, pobj, nomark);
    }
  }

  /**
   * @brief возвращает указатели на все объектов заданного префикса из глобального реестра
   * @param prefix префикс объекта (aka пространство имен)
   * @return указатели на все объектов заданного префикса
   */
  template<typename I>
  std::map<std::string, std::shared_ptr<I> > select_objects(const std::string& prefix) const
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
   * @see wflow::workflow
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
   * @warning последнее обновление. В таргет можно передавать JSON-объект с конфигурацией workflow, тогда будет создан
   * незарегистрированный workflow, который можно удалить после использования. Может использоваться для многопоточной инициализации
   * или начальной загрузки из БД или файла. Для этого в json-конфигурации объекта для имени вместо string следует использовать raw_value.
   * После этого в конфиге можно использовать как имя так и сразу сконфигурировать workflow. Для основного workflow все по прежнему,
   * можно использовать только имя.
   * @warning незарегистрированный workflow не собирает статистику, на него нельзя назначить ядра CPU,
   * и фреймворк не отслеживает зависания потоков таких workflow
   * @see wflow::workflow
   */
  std::shared_ptr<workflow_type> get_workflow(const std::string& target, bool disabort = false, bool* created = nullptr) const
  {
    if (!is_configured_())
      return nullptr;

    if ( target.empty() )
      return _workflow;

    return this->get_workflow_(target, disabort, created);
  }


  /**
   * @brief Возвращает указатель на общий [workflow](https://mambaru.github.io/wflow/) системы
   * @return указатель на [workflow](https://mambaru.github.io/wflow/) или nullptr если система еще не сконфигурирована
   * @details возвращается общий workflow системы. Гарантированно результат не будет nullptr, после этапа конфигурации.
   * Если в конфигурации объекта не задан workflow, то результат тот же что и get_workflow()
   * @see wflow::workflow
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
   * @brief Создает и запускает новый [workflow](https://mambaru.github.io/wflow/) с задаными опциями
   * @param opt опции для workflow
   * @return новый объект workflow
   * @see wflow::workflow
   */
  std::shared_ptr<workflow_type> create_workflow(const wflow::workflow_options& opt) const
  {
    wflow::workflow_handlers wrkh;
    if (auto g = this->global() )
    {
      pid_t pid = 0;
      wrkh.status_handler = [g, pid](std::thread::id) mutable
      {
        pid = g->cpu.thread_active(pid);
      };
    }

    auto wflow = std::make_shared<workflow_type>(this->global()->io_context, opt, wrkh);
    wflow->start();
    return wflow;
  }

  /**
   * @brief Создает и запускает новый workflow на основе конфигурации существующего объекта или json-конфигурации
   * @param newname Имя для отображения в логах и пр
   * @param target Имя существующего workflow (в конфигурации) или json-строка с новой конфигурацией
   * @details Основное назначение - это создание временных workflow, например для начальной загрузки из БД
   * @see wflow::workflow, wflow::workflow_options_json
   */
  std::shared_ptr<workflow_type> create_workflow(const std::string& newname, const std::string& target) const
  {
    auto opt = this->create_workflow_options_(target);

    std::string newid = this->name() + ":workflow";
    if ( !opt.id.empty() ) newid += "." + opt.id;
    if ( !newname.empty() ) newid += "." + newname;
    opt.id = newid;
    return this->create_workflow(opt);
  }


  /**
   * @brief Создает и запускает новый workflow на основе конфигурации workflow по умолчанию ( workflow::workflow )
   * @param newname Имя для отображения в логах и пр
   * @details Основное назначение - это создание временных workflow, например для начальной загрузки из БД
   * @see wflow::workflow
   */
  std::shared_ptr<workflow_type> create_workflow(const std::string& newname)
  {
    return this->create_workflow(newname, "");
  }

  /**
   * @brief Создает и запускает новый workflow на основе конфигурации workflow по умолчанию ( workflow::workflow )
   * @details Основное назначение - это создание временных workflow, например для начальной загрузки из БД.
   * Идентификатор для отображения будет в формате `имя_объекта:workflow`
   * @see wflow::workflow
   */
  std::shared_ptr<workflow_type> create_workflow()
  {
    return this->create_workflow("", "");
  }

  /**
   * @brief Добавляет фоновый обработчик с периодичностью один раз в секунду
   * @param fun обработчик
   * @details Может вызываться только в методе start и работает до завершения работы приложения.
   */
  void idle(std::function<bool()> fun)
  {
    this->idle(std::chrono::seconds(1), fun);
  }

  /**
   * @brief Добавляет фоновый обработчик с заданной периодичностью
   * @param duration интервал запуска
   * @param fun обработчик
   * @details Может вызываться только в методе start и работает до завершения работы приложения.
   */
  void idle(workflow_type::duration_t duration, std::function<bool()> fun)
  {
    if ( _idle_flag )
    {
      this->get_common_workflow()->create_timer(duration, fun);
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

    if ( this->global_stop_flag() )
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
   * @brief Возвращает ссылку на глобальный флаг прекращения работы
   * @return ссылка на глобальный флаг прекращения работы
   * @details Может быть использования при длительных процедурах инициализации (например загрузка БД),
   * чтобы завершить работу, а не дожидаться завершения инициализации. Так же можно передавать в качестве параметра
   * другим длительным процедурам, чтобы ускорить завершение работы
   */
  const std::atomic_bool& global_stop_flag() const
  {
    if (auto g = this->global() )
      return g->stop_signal_flag;

    static std::atomic_bool _fake_flag;
    return _fake_flag;
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
    std::vector<std::string> schema_list;
    boost::split( schema_list, arg, boost::is_any_of(",") );
    domain_config cnfg;
    wjson::schema::create_schema<domain_config>(cnfg, schema_list);
    return cnfg;
  }

  bool is_configured() const
  {
     return _workflow!=nullptr;
  }


  typedef instance_handler_<Opt, StatOpt> instance_handler_t;
  instance_handler_t* inst_handler_()
  { return static_cast<instance_handler_<Opt, StatOpt>*>(this); }

private:
  virtual void domain_generate(domain_config& conf, const std::string& type) override final;
  virtual void create_domain(const std::string& objname, global_ptr g ) override final;
  virtual void configure_domain(const domain_config& opt) override final;
  virtual void initialize_domain() override final;
  virtual void reconfigure_domain_basic(const domain_options& opt) override final;
  virtual void reconfigure_domain(const domain_config& conf) override final;
  virtual void restart_domain() override final;
  virtual void start_domain() override final;
  virtual void stop_domain() override final;

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
    SYSTEM_LOG_FATAL("Attempting to use is not initialized object '" << _name <<"'")
    return false;
  }


  /* 0-строка, 1=опции */
  int dual_target_deserialize_(const std::string& target, std::string* str, wflow::workflow_options* opt) const
  {
    if ( target.empty() )
      return 0;

    wjson::json_error er;
    int ret = wjson::multi_deserializer<
      wjson::string<>,
      wflow::workflow_options_json
    >( target.begin(), target.end(), *str, *opt, &er );

    if ( ret > 1 )
    {
      ret = 0;

      if ( er )
      {
        *str = target;
        er.reset();
      }
      else if ( !er )
      {
        wflow::workflow_options tmp;
        wflow::workflow_options_json::serializer()(tmp, target.begin(), target.end(), &er);
        str->clear();
      }

      if ( er )
      {
        SYSTEM_LOG_ERROR("Workflow configuration error. Default configuration will be used. Trace:"
          << wjson::strerror::message_trace(er, target.begin(), target.end()));
      }
    }
    return ret;
  }

  wflow::workflow_options create_workflow_options_(const std::string& target) const
  {
    wflow::workflow_options opt;
    std::string target_name;
    if ( 0 == this->dual_target_deserialize_(target, &target_name, &opt) )
    {
      if ( auto wflow1 = this->get_workflow(target_name, true) )
      {
        opt = wflow1->get_options();
        opt.id = target_name;
      }
      else if ( auto wflow2 = this->get_common_workflow() )
      {
        opt = wflow2->get_options();
      }
    }

    return opt;
  }

  std::shared_ptr<workflow_type> get_workflow_(const std::string& target, bool disabort, bool* created) const
  {
    if ( created!=nullptr ) 
      *created = false;
    
    if ( target.empty() )
      return nullptr;

    auto beg = target.begin();
    auto end = target.end();
    
    if ( wjson::parser::is_null(beg, end) )
      return nullptr;

    if ( wjson::parser::is_object(beg, end) )
    {
      if ( created!=nullptr ) *created = true;
      return this->create_workflow("", target);
    }

    std::string starget;

    if ( wjson::parser::is_string(beg, end) )
      wjson::string<>::serializer()(starget, beg, end, nullptr);
    else
      starget=target;
    
    if ( starget.empty() )
      return nullptr;
    
    return this->global()->registry.template get_object<workflow_type>("workflow", starget, disabort);
  }

private:
  bool _conf_flag = false;
  bool _idle_flag = false;
  bool _my_workflow = false;
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
  _owner.enable_tracking(opt.tracking);
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

    _workflow = this->get_workflow_(_config.workflow, false, &_my_workflow);
    if ( _workflow == nullptr )
      _workflow = this->global()->common_workflow;

    _statistics = ! _config.statistics.disabled
                  ? this->global()->registry.template get_object<statistics::statistics>("statistics", _config.statistics.target, false)
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
  _owner.enable_tracking(opt.tracking);
  this->reconfigure_basic();
}

template<typename Itf, typename Opt, typename StatOpt>
void domain_object<Itf, Opt, StatOpt>::reconfigure_domain(const domain_config& conf)
{
  _conf_flag = true;
  _config = conf;
  if (auto g = this->global() )
    g->cpu.set_cpu(_name, static_cast<domain_options&>(_config).cpu);
  _owner.enable_tracking(conf.tracking);
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
  if ( _my_workflow )
    _workflow->stop();
  this->stop();
}

}

