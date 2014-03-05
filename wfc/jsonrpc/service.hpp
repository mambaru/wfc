#include <iostream>

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/io_service.hpp>
#include <wfc/json/json.hpp>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>



namespace wfc{ namespace jsonrpc{
  

typedef wfc::io::data_type          raw_id_type;
typedef std::unique_ptr<raw_id_type>  raw_id_ptr;


struct incoming
{
  typedef wfc::io::data_type data_type;
  typedef data_type::iterator iterator;
  typedef std::pair<iterator, iterator> pair_type;
  pair_type method;
  pair_type params;
  pair_type result;
  pair_type error;
  pair_type id;
};

struct incoming_json
{
  typedef incoming::pair_type pair_type;
  typedef json::raw_pair<pair_type> pair_json;
  
  FAS_NAME(id)
  FAS_NAME(method)
  FAS_NAME(params)
  FAS_NAME(result)
  FAS_NAME(error)
  
  typedef json::object<
    incoming, 
    fas::type_list_n<
      json::member<n_method, incoming, pair_type, &incoming::method, pair_json>,
      json::member<n_params, incoming, pair_type, &incoming::params, pair_json>,
      json::member<n_result, incoming, pair_type, &incoming::result, pair_json>,
      json::member<n_error,  incoming, pair_type, &incoming::error,  pair_json>,
      json::member<n_id,     incoming, pair_type, &incoming::id,     pair_json>
    >::type
  > type;

  typedef type::serializer serializer;
};

class incoming_holder
{
  typedef wfc::io::data_type data_type;
  typedef wfc::io::data_ptr  data_ptr;
  typedef data_type::iterator iterator;
public:
  incoming_holder(data_ptr d, std::weak_ptr< wfc::io::iio> iio, wfc::io::callback clb)
    : iio(iio)
    , handler( clb )
    , _data( std::move(d) )
  {
    _begin = wfc::json::parser::parse_space(_data->begin(), _data->end());
    _end = incoming_json::serializer()( _incoming, _begin, _data->end());
  }
  
  data_ptr detach()
  {
    _incoming = incoming();
    return std::move(_data);
  }
  
  data_ptr tail() const
  {
    if ( _data == nullptr )
      return nullptr;
    
    iterator beg = wfc::json::parser::parse_space( _end, _data->end());
    if ( beg == _data->end() )
      return nullptr;
    
    return std::make_unique<data_type>(beg, _data->end());
  }
  
  bool empty() const { return !$();}
  bool $() const{ return _data != nullptr;}
  
  bool has_method()   const { return $() && _incoming.method.first!=_incoming.method.second;}
  bool has_result() const   { return $() && _incoming.result.first!=_incoming.result.second;}
  bool has_error() const    { return $() && _incoming.error.first!=_incoming.error.second;}
  bool has_id() const       { return $() && _incoming.id.first!=_incoming.id.second;}
  bool has_params() const   { return $() && _incoming.params.first!=_incoming.params.second;}
  
  bool is_request() const     { return this->has_method() && this->has_id();   }
  bool is_response() const    { return this->has_result() && this->has_id();   }
  bool is_notify() const      { return this->has_method() && !this->has_id();  }
  bool is_error() const       { return this->has_error(); }
  bool is_other_error() const   { return this->has_error() && !this->has_id();   }
  bool is_request_error() const { return this->has_error() && this->has_id();   }
  
  bool is_valid() const 
  {
    return this->is_request()
      || this->is_response()
      || this->is_notify()
      || this->is_error();
  }
  
    
  bool method(const char* ch)
  {
    incoming::iterator beg = _incoming.method.first;
    incoming::iterator end = _incoming.method.second;
    if (beg++==end) return false;
    if (beg==end--) return false;
    for (; beg != end && *beg==*ch; ++beg, ++ch);
    return beg==end && *ch=='\0';
    
    /*static const char *name = N()();
    const char *ch = name;
    for (; beg != end && *beg==*ch; ++beg, ++ch);
    return beg==end && *ch='\0';
    */
  }

  
  template<typename V, typename J = json::value<V> >
  V get_id() const
  {
    V id = V();
    if ( $() )
      typename J::serializer()( id, _incoming.id.first, _incoming.id.second );
    return id;
  }
  
  data_ptr raw_id() const
  {
    if ( this->has_id() )
      return std::make_unique<data_type>(_incoming.id.first, _incoming.id.second);
    else
      return nullptr;
  }
  
  template<typename J>
  std::unique_ptr<typename J::target> get_params() const
  {
    if ( !this->has_params() )
      return nullptr;
    if ( 'n'==*_incoming.params.first)
      return nullptr; // is null 
    auto result = std::make_unique<typename J::target>();
    typename J::serializer()(*result, _incoming.params.first, _incoming.params.second);
    return std::move(result);
  }
  
  template<typename J>
  std::unique_ptr<typename J::value_type> get_error() const
  {
    if ( !this->has_error() )
      return nullptr;
    auto result = std::make_unique<typename J::value_type>();
    typename J::serializer()(*result, _incoming.error.first, _incoming.error.second);
    return std::move(result);
  }
  
  std::weak_ptr<wfc::io::iio> iio;
  wfc::io::callback handler;
  
  const incoming& get()  const 
  {
    return _incoming;
  }
  
private:
  
  data_ptr _data;
  incoming _incoming;
  iterator _begin;
  iterator _end;
};

/// ERROR

struct error
{
  int code;
  std::string message;
  
  error()
    : code()
    , message()
  {}
  
  error(int code, std::string message)
    : code(code)
    , message(message)
  {}
  
};

// -32602, "Invalid params"


struct parse_error: error
{
  parse_error()
    : error(-32700, "Parse error")
  {}
};

struct invalid_request: error
{
  invalid_request()
    : error(-32600, "Invalid Request")
  {}
};

struct method_not_found: error
{
  method_not_found()
    : error(-32601, "Method not found")
  {}
};

struct invalid_params: error
{
  invalid_params()
    : error(-32602, "Invalid params")
  {}
};

struct internal_error: error
{
  internal_error()
    : error(-32603, "Internal error")
  {}
};

struct method_not_impl: error
{
  method_not_impl()
    : error(-32001, "Method not implementation")
  {}
};

struct server_error: error
{
  server_error()
    : error(-32000, "Server error")
  {}

  server_error( const std::string& what)
    : error(-32002, std::string("Server error: ") + what )
  {}
};


struct error_json
{
  FAS_NAME(code)
  FAS_NAME(message)
  
  typedef json::object<
    error, 
    fas::type_list_n<
      json::member<n_code,    error, int,         &error::code>,
      json::member<n_message, error, std::string, &error::message>
    >::type
  > type;

  typedef type::serializer serializer;
};

/// END ERROR

/// OUTGOING JSON

struct outgoing
{
  typedef char version_type[4];
  version_type version;
  outgoing()
    : version{'2','.','0','\0'}
  {}
};

struct version_member
{
  FAS_NAME(jsonrpc)
  typedef json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  


template<typename T>
struct outgoing_error: outgoing
{
  std::unique_ptr<T> error;
  std::unique_ptr< wfc::io::data_type> id;
  outgoing_error()
    : outgoing()
    , id(nullptr)
  {}
};

template<typename T>
struct outgoing_error_json
{
  typedef typename T::target target;
  typedef outgoing_error<target> error_type;
  typedef typename error_type::version_type version_type;
  typedef json::pointer<std::unique_ptr<target>, T> error_json;
  typedef json::pointer<std::unique_ptr<wfc::io::data_type>, json::raw_value< wfc::io::data_type> > id_json;
  
  FAS_NAME(error)
  FAS_NAME(id)
  
  typedef json::object<
    error_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_error, error_type, std::unique_ptr<target>,    &error_type::error, error_json >,
      json::member<n_id,    error_type, wfc::io::data_ptr, &error_type::id,    id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};


template<typename T>
struct outgoing_result
  : outgoing
{
  std::unique_ptr<T> result;
  wfc::io::data_ptr id;
};

template<typename T>
struct outgoing_result_json
{
  typedef typename T::target target;
  typedef outgoing_result<target> result_type;
  typedef typename result_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> result_json;
  typedef json::pointer<wfc::io::data_ptr, json::raw_value< wfc::io::data_type> > id_json;
  
  FAS_NAME(id)
  FAS_NAME(result)
  
  typedef json::object<
    result_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_result, result_type, std::unique_ptr<target>,  &result_type::result, result_json >,
      json::member<n_id,     result_type, wfc::io::data_ptr, &result_type::id,   id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};



/// end OUTGOING JSON

/// INSTANCE
/// Объект обрабатывающий запросы (один объект на запрос, если нет захвата)

class handler_base
{
public:
  virtual ~handler_base() {}
  virtual std::shared_ptr<handler_base> clone() = 0;
  
  virtual void process(incoming_holder holder) = 0;
  /*
  virtual bool process_method(incoming_holder holder) = 0;
  virtual bool process_result(incoming_holder holder) = 0;
  virtual bool process_error(incoming_holder holder) = 0;
  */
  
  // Инициализируеться jsonrpc
  // Обработчик исходящего запроса
  std::function< void( wfc::io::data_ptr /*d*/, wfc::io::data_ptr /*id*/, std::shared_ptr<handler_base>) > outgoing = nullptr;
};

struct _name_;

template<typename N>
struct name
{
  typedef fas::metalist::advice metatype;
  typedef _name_ tag;
  typedef name<N> advice_class;
  typedef N name_type;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  const char* operator()() const
  {
    return name_type()();
  }
};

struct _target_;

template<typename I>
struct target
  : fas::value<_target_, std::weak_ptr<I> >
{
};


//float (SomeClass::*my_memfunc_ptr)(int, char *);

template<
  typename Req, 
  typename Resp, 
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<Req>, std::function< callback_status(std::unique_ptr<Req>) > ) >
struct mem_fun_handler
{
  
  template<typename T>
  void operator()(T& t, std::unique_ptr<Req> req, std::function< wfc::callback_status(std::unique_ptr<Resp>, std::unique_ptr<wfc::jsonrpc::error>) > callback)
  {
    if ( auto i = t.target().lock() )
    {
      if ( callback==nullptr)
      {
        (i.get()->*mem_ptr)( std::move(req), nullptr );
      }
      else
      {
        (i.get()->*mem_ptr)( std::move(req), [callback](std::unique_ptr<Resp> resp)
        {
          return callback( std::move(resp), nullptr);
        });
      }
    }
  }
};

struct _invoke_;
template<typename JReq, typename JResp, typename Handler>
struct invoke: Handler
{
  typedef fas::metalist::advice metatype;
  typedef _invoke_ tag;
  typedef invoke<JReq, JResp, Handler> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JReq  request_json;
  typedef JResp response_json;
  typedef typename request_json::target  request_type;
  typedef typename response_json::target response_type;
  
  
  template<typename T>
  void operator()(T& t, incoming_holder holder)
  {
    try
    {
      auto req = holder.get_params<request_json>();
      if (holder.is_notify())
      {
        Handler::operator()( t, std::move(req), nullptr);
      }
      else
      {
        auto ph = std::make_shared<incoming_holder>( std::move(holder) );
        Handler::operator()( t, std::move(req), 
          [ph]( std::unique_ptr<response_type> params, std::unique_ptr<error> err ) -> callback_status
          {
            if (err != nullptr )
            {
              typedef outgoing_error_json< error_json::type >::type json_type;
              outgoing_error<error> error_message;
              error_message.error = std::move(err);
              error_message.id = std::move( ph->raw_id() );
              
              auto d = ph->detach();
              d->clear();
              typename json_type::serializer()(error_message, std::inserter( *d, d->end() ));
              return ph->handler( std::move(d) );
            }
            else
            {
              outgoing_result<response_type> result;
              result.result = std::move(params);
              result.id = ph->raw_id();
              auto d = ph->detach();
              d->clear();
              typedef outgoing_result_json<response_json> result_json;
              typename result_json::serializer()(result, std::inserter( *d, d->end() ));
              std::cout << "method SEND: " << std::string(d->begin(), d->end()) << std::endl;
              return ph->handler( std::move(d) );
              /*typename error_json::serializer()(error_message, std::inserter( *d, d->end() ));
              return ph->handler( std::move(d) );
              */
            }
          } // callback 
        );
      }
    }
    catch(const json::json_error& e)
    {
      // SEND INVALID Params и перенести выше
      //t.get_aspect().template get<_invalid_params_>()(t, id);
    }
  }
};

template<
  typename JReq, 
  typename JResp, 
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<typename JReq::target>, std::function< callback_status(std::unique_ptr<typename JResp::target>) > ) >
struct invoke_mem_fun
  : invoke<JReq, JResp, mem_fun_handler<typename JReq::target, typename JResp::target, I, mem_ptr> >
{};

struct method_aspect: fas::aspect<> {};

template< typename... Args >
class method
  : public fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, method_aspect>::type >
{
public:
  
  typedef method<Args...> self;
  typedef fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, method_aspect>::type > super;
  
  typedef fas::metalist::advice metatype;
  typedef typename super::aspect::template advice_cast<_name_>::type::name_type tag;
  typedef self advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  const char* name() const
  {
    return this->get_aspect().template get<_name_>()();
  }

  /*
  template<typename Itr>
  bool name(Itr beg, Itr end) const
  {
    this->get_aspect().template get<_name_>()( beg, end);
  }
  */

  template<typename T>
  void operator()(T& t, incoming_holder holder)
  {
    std::cout << "METHOD!" << std::endl;
    this->get_aspect().template get<_invoke_>()(t, std::move(holder));
  }

  /*
  template<typename T>
  void process_method(T& t, incoming_holder holder)
  {
    
  }
  
  template<typename T>
  void process_result(T& t, incoming_holder holder)
  {
    
  }
  
  template<typename T>
  void process_error(T& t, incoming_holder holder)
  {
  }
  */
  
  /*
  template<typename T>
  void request(T& t, void params, callback callback)
  {
  }*/
};

struct _method_;

template< typename Method>
struct add_method: fas::type_list_n<
  Method,
  fas::group<_method_, typename Method::tag>
>::type {};

struct instance_aspect: fas::aspect<
  target<fas::empty_type>
>{};

struct f_invoke
{
  incoming_holder& holder;
  
  f_invoke(incoming_holder& holder)
    : holder( holder )
  {
  }
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if ( holder.$() )
    {
      if ( holder.method( t.get_aspect().template get<Tg>().name() ) )
      {
        t.get_aspect().template get<Tg>()(t, std::move(holder) );
      }
    }
  }
};


template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class method_list
  : public handler_base
  , public AspectClass< typename fas::merge_aspect<A, instance_aspect>::type >
  , public std::enable_shared_from_this< method_list<A, AspectClass> >
{
public:
  typedef method_list<A, AspectClass> self;
  typedef self magic;
  typedef AspectClass< typename fas::merge_aspect<A, instance_aspect>::type > super;
  
  typedef typename super::aspect::template advice_cast<_target_>::type target_type;
  
  /*
  instance(invoke_interface itf = invoke_interface() )
  {
    this->get_aspect().template get<_invoke_interface_>() = itf;
  }
  */
  
  virtual void process(incoming_holder holder)
  {
    std::cout << "instance process!!!" << std::endl;
    fas::for_each_group<_method_>(*this, f_invoke( holder ) );
  }
  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

};

template<typename Instanse>
class handler
  : public Instanse
{
public:
  typedef handler<Instanse> self;
  typedef Instanse super;
  typedef typename Instanse::target_type target_type;
  
  handler(target_type trg = target_type() )
  {
    this->get_aspect().template get<_target_>() = trg;
  }

  virtual std::shared_ptr<handler_base> clone()
  {
    std::cout << "CLONE!!!" << std::endl;
    return std::make_shared<self>(*this);
  }
  
};


/// END INSTANCE

struct worker_options
  : wfc::io::basic::options
{
  std::shared_ptr<handler_base> instance;
};


/// WORKER

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class worker_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< worker_base<A, AspectClass> >
{
public:
  typedef worker_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;
  
  worker_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
    super::create(*this);
  }
  
  void operator()(incoming_holder holder)
  {
    auto ph = std::make_shared<incoming_holder>(std::move(holder) );
    this->dispatch([this, ph](){
      if ( auto p1 = ph->iio.lock() )
      {
        auto p2 = p1.get();
        auto itr = _instance_map.find(p2);
        if ( itr == _instance_map.end() )
        {
          itr = _instance_map.insert( std::make_pair(p2, this->options().instance->clone() ) ).first;
          p1->add_release_handler(
            [this](std::weak_ptr<wfc::io::iio> wp)
            {
              auto pp = wp.lock().get();
              this->dispatch([this, pp](){
                std::cout << "RELEASE1: " << this->_instance_map.size() << std::endl;
                // Здесь проверок не ставим
                // Если поломато, значит ошибка в логике
                this->_instance_map.erase(pp);
                std::cout << "RELEASE2: " << this->_instance_map.size() << std::endl;
              });
            }
          );
        }
        
        itr->second->process( std::move(*ph) );
      }
    });
  }
private:
  std::map< wfc::io::iio*, std::shared_ptr<handler_base> > _instance_map;
  
};

struct worker_aspect: fas::aspect<
  fas::type<wfc::io::_options_type_, worker_options>
> {};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class worker:
  public worker_base< typename fas::merge_aspect< A, worker_aspect>::type, AspectClass>
{
public:
  typedef worker<A, AspectClass> self;
  typedef worker_base< typename fas::merge_aspect< A, worker_aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  worker(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }

};





/// END WORKER



struct _incoming_;

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class service_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< service_base<A, AspectClass> >
{
public:
  typedef service_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;

  service_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
    , handler (std::bind( &self::operator(), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) )
    , tmp_worker(io_service, opt)
  {
    super::create(*this);
  }

  void operator()( data_ptr d, std::weak_ptr<wfc::io::iio> iio, wfc::io::callback&& handler)
  {
    auto dd = std::make_shared<data_ptr>( std::move(d) );
    this->dispatch([this, dd, iio, handler]()
    {
        std::cout << "jsonrpc_base" << std::endl; 
        

      this->get_aspect().template get<_incoming_>()( *this, std::move(*dd), iio, std::move(handler) );
    });
  }
  
  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }

  wfc::io::handler handler;
  
  worker<> tmp_worker;
};

/// /////////////////////////////////////////

struct _send_json_;

struct ad_send_json
{
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, incoming_holder holder)
  {
    auto d = holder.detach();
    d->clear();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    holder.handler( std::move(d) );
  }
  
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, wfc::io::callback handler)
  {
    auto d = std::make_unique<wfc::io::data_type>();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    handler( std::move(d) );
  }
};


struct _jsonrpc_error_;
struct ad_jsonrpc_error
{
  template<typename T>
  void operator()( T& t, const error& err, incoming_holder holder)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = std::move( holder.raw_id() );
    t.get_aspect().template get<_send_json_>()(t, json_type(), std::move(error_message), std::move(holder) );
  }

  template<typename T>
  void operator()( T& t, const error& err, wfc::io::callback handler)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = nullptr;
    t.get_aspect().template get<_send_json_>()(t, json_type(), std::move(error_message), handler );
  }
};

struct _process_error_;
struct _process_method_;
struct _process_result_;


struct ad_process_method
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    t.tmp_worker( std::move(holder) );
  }
};

struct ad_process_error
{
  template<typename T>
  void operator()( T&, incoming_holder)
  {
    
  }
};

struct ad_process_result
{
  template<typename T>
  void operator()( T&, incoming_holder)
  {
  }
};

struct _process_;
struct ad_process
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    if ( holder.has_method() )
    {
      t.get_aspect().template get<_process_method_>()(t, std::move(holder) );
    }
    else if ( holder.has_result() )
    {
      t.get_aspect().template get<_process_result_>()(t, std::move(holder) );
    }
    else if ( holder.has_error() )
    {
      t.get_aspect().template get<_process_error_>()(t, std::move(holder) );
    }
  }
};


struct _verify_;
struct ad_verify
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    if ( holder.is_valid() )
    {
      t.get_aspect().template get<_process_>()(t, std::move(holder) );
    }
    else
    {
      t.get_aspect().template get<_jsonrpc_error_>()(t, invalid_request(), std::move(holder) );
    }
  }
};

struct ad_incoming
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d, std::weak_ptr<wfc::io::iio> iio, wfc::io::callback handler)
  {
    std::cout << "ad_incoming { " << std::string( d->begin(), d->end()) << std::endl;
    try
    {
      while (d != nullptr)
      {
        std::cout << "ad_incoming next: " << std::string( d->begin(), d->end()) << std::endl;
        incoming_holder hold(std::move(d), iio, handler );
        d = hold.tail();
        t.get_aspect().template get<_verify_>()( t, std::move(hold) );
      }
    }
    catch(const json::json_error& er)
    {
      t.get_aspect().template get<_jsonrpc_error_>()(t, parse_error(), handler );
    }
    catch(const std::exception& ex)
    {
      // TODO; сделать логгирование 
      t.get_aspect().template get<_jsonrpc_error_>()(t, server_error(ex.what() ), handler );
    }
    catch(...)
    {
      // TODO; сделать логгирование 
      t.get_aspect().template get<_jsonrpc_error_>()(t, server_error(), handler );
    }
    std::cout << "} ad_incoming" << std::endl;
  }
};


/// /////////////////////////////////////////

struct options: worker_options
{
  
};

struct aspect: fas::aspect<
  fas::type<wfc::io::_options_type_, options>,
  fas::advice<_incoming_, ad_incoming>,
  fas::advice<_verify_,ad_verify>,
  fas::advice<_process_,ad_process>,
  fas::advice<_send_json_, ad_send_json>,
  fas::advice<_jsonrpc_error_, ad_jsonrpc_error>,
  fas::advice<_process_error_,  ad_process_error>,
  fas::advice<_process_method_, ad_process_method>,
  fas::advice<_process_result_, ad_process_result>

> {};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class service:
  public service_base< typename fas::merge_aspect< A, aspect>::type, AspectClass>
{
public:
  typedef service<A, AspectClass> self;
  typedef service_base< typename fas::merge_aspect< A, aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  service(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }

};

}} // wfc


