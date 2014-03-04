#include <iostream>

/*
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/rn/reader/aspect.hpp>
*/

#include <wfc/io/posix/rn/reader.hpp>

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/io_service.hpp>
#include <wfc/json/json.hpp>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>


/*
struct reader_aspect: 
  fas::aspect<
    wfc::io::rn::reader::aspect2<wfc::io::reader::_incoming_>,
    wfc::io::reader::stream< boost::asio::posix::stream_descriptor, wfc::io::rn::reader::_incoming_>,
    wfc::io::reader::error_log
  >
{};    
*/

namespace wfc{ namespace jsonrpc{
  
typedef wfc::io::data_type data_type;
typedef wfc::io::data_ptr  data_ptr;
typedef wfc::io::handler   handler;
typedef wfc::io::callback  callback;

typedef data_type                   raw_id_type;
typedef std::unique_ptr<data_type>  raw_id_ptr;


struct incoming
{
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
  typedef data_type::iterator iterator;
public:
  incoming_holder(data_ptr d, callback clb)
    : handler( clb )
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
  std::unique_ptr<typename J::value_type> get_params() const
  {
    if ( !this->has_params() )
      return nullptr;
    if ( 'n'==*_incoming.params.first)
      return nullptr; // is null 
    auto result = std::make_unique<typename J::value_type>();
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
  
  callback handler;
  
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
  std::unique_ptr<data_type> id;
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
  typedef json::pointer<std::unique_ptr<data_type>, json::raw_value<data_type> > id_json;
  
  FAS_NAME(error)
  FAS_NAME(id)
  
  typedef json::object<
    error_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_error, error_type, std::unique_ptr<target>,    &error_type::error, error_json >,
      json::member<n_id,    error_type, std::unique_ptr<data_type>, &error_type::id,    id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};


template<typename T>
struct outgoing_result
  : outgoing
{
  std::unique_ptr<T> result;
  std::unique_ptr<data_type> id;
};

template<typename T>
struct outgoing_result_json
{
  typedef typename T::target target;
  typedef outgoing_result<target> result_type;
  typedef typename result_type::version_type version_type;

  typedef json::pointer<std::unique_ptr<target>, T> result_json;
  typedef json::pointer<std::unique_ptr<data_type>, json::raw_value<data_type> > id_json;
  
  FAS_NAME(id)
  FAS_NAME(result)
  
  typedef json::object<
    result_type,
    typename fas::type_list_n<
      version_member::type,
      json::member<n_result, result_type, std::unique_ptr<target>,  &result_type::result, result_json >,
      json::member<n_id,     result_type, std::unique_ptr<data_type>, &result_type::id,   id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};



/// end OUTGOING JSON

/// INSTANCE
/// Объект обрабатывающий запросы (один объект на запрос, если нет захвата)

class instance_base
{
public:
  virtual ~instance_base() {}
  virtual std::shared_ptr<instance_base> clone() = 0;
  virtual bool process_method(incoming_holder holder) = 0;
  virtual bool process_result(incoming_holder holder) = 0;
  virtual bool process_error(incoming_holder holder) = 0;
  
  // Инициализируеться jsonrpc
  // Обработчик исходящего запроса
  std::function< void( data_ptr /*d*/, data_ptr /*id*/, std::shared_ptr<instance_base>) > outgoing = nullptr;
};

template<typename N>
struct name
{
  typedef fas::metalist::advice metatype;
  typedef N tag;
  typedef name<N> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  template<typename Itr>
  bool operator()(Itr beg, Itr end)
  {
    static const char *name = N()();
    const char *ch = name;
    for (; beg != end && *beg==*ch; ++beg, ++ch);
    return beg==end && *ch='\0';
  }
};

struct _request_;
template<typename JReq, typename JResp, typename Handler>
struct request: Handler
{
  typedef fas::metalist::advice metatype;
  typedef _request_ tag;
  typedef request<JReq, JResp, Handler> advice_class;

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
          [ph]( std::unique_ptr<response_type> params, std::unique_ptr<error> err )
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
              ph->handler( std::move(d) );
            }
            else
            {
              outgoing_result<response_type> result;
              result.result = std::move(params);
              result.id = ph->raw_id();
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

template< typename A = fas::aspect<> >
class method
  : public fas::aspect_class<A>
{
public:
  
  typedef method<A> self;
  typedef fas::aspect_class<A> super;

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
  
  /*
  template<typename T>
  void request(T& t, void params, callback callback)
  {
  }*/
  
  
};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class instance
  : public instance_base
  , public AspectClass<A>
  , public std::enable_shared_from_this< instance<A, AspectClass> >
{
public:
  virtual std::shared_ptr<instance_base> clone()
  {
    
  }
  
  virtual bool process_method(incoming_holder holder)
  {
    
  }
  
  virtual bool process_result(incoming_holder holder)
  {
  }
  
  virtual bool process_error(incoming_holder holder)
  {
    
  }
  
};


/// END INSTANCE



struct _incoming_;

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class jsonrpc_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< jsonrpc_base<A, AspectClass> >
{
public:
  typedef jsonrpc_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;

  jsonrpc_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
    , handler (std::bind( &self::operator(), this, std::placeholders::_1, std::placeholders::_2) )
  {
    super::create(*this);
  }

  /*
  void doit( data_ptr d, wfc::io::callback handler)
  {
    std::cout << "JSON RPC {" <<  std::string(d->begin(), d->end())<< std::endl;
    handler( std::move(d) );
    std::cout << "}JSON RPC" << std::endl;
  }
  */

  
  void operator()( data_ptr d, wfc::io::callback&& handler)
  {
    this->get_aspect().template get<_incoming_>()( *this, std::move(d), std::move(handler) );
    /*
    std::cout << "JSON RPC operator(1) " <<  (d != nullptr) << std::endl;

    auto dd = std::make_shared<data_ptr>(std::move(d));
    this->dispatch( [this, dd,  handler]() {
      std::cout << "JSON RPC operator(2) " <<  ( *dd != nullptr) << std::endl;
      this->doit( std::move(*dd), handler);
    });
    */
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
  void operator()( T&, J, O obj, callback handler)
  {
    auto d = std::make_unique<data_type>();
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
  void operator()( T& t, const error& err, callback handler)
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
  void operator()( T&, incoming_holder)
  {
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
  void operator()( T& t, typename T::data_ptr d, wfc::io::callback&& handler)
  {
    std::cout << "ad_incoming { " << std::string( d->begin(), d->end()) << std::endl;
    try
    {
      while (d != nullptr)
      {
        std::cout << "ad_incoming next: " << std::string( d->begin(), d->end()) << std::endl;
        incoming_holder hold(std::move(d), handler );
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

struct options
{
  std::function<void()> not_alive = nullptr;
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
class jsonrpc:
  public jsonrpc_base< typename fas::merge_aspect< A, aspect>::type, AspectClass>
{
public:
  typedef jsonrpc<A, AspectClass> self;
  typedef jsonrpc_base< typename fas::merge_aspect< A, aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  jsonrpc(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }

};

}} // wfc

int main()
{
  int dd[2];
  ::pipe(dd);

  wfc::io_service io_service;
  wfc::jsonrpc::options options;
  wfc::jsonrpc::jsonrpc<> jsonrpc(io_service, options);
  
  typedef wfc::io::posix::rn::reader reader_type;
  reader_type::options_type reader_options;
  
  reader_options.handler = jsonrpc.handler;
  
  reader_type::descriptor_type desc(io_service, dd[0]);
  reader_type reader( std::move(desc), reader_options );
  /*
  wfc::io::rn::reader::options init;
  init.input_buffer_size = 8096;
  init.handler = jsonrpc.handler;
  
  typedef wfc::io::reader::reader< reader_aspect > reader_type;
  boost::asio::posix::stream_descriptor sd(io_service, dd[0]);
  reader_type reader( std::move(sd), init);    
  */
  
  //jsonrpc.start(jsonrpc);
  
  std::thread th([&dd, &io_service]()
  {
    const char *req1="\t {\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":12345} \t\t {\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":12345} \r\n";
    const char *req2="{-}\r\n";
    const char *req3="--\r\n";
    const char *req4="{\"id\":[\"66!\"]}\r\n";
    
    write(dd[1], req1, std::strlen(req1) );
    write(dd[1], req2, std::strlen(req2) );
    write(dd[1], req3, std::strlen(req3) );
    write(dd[1], req4, std::strlen(req4) );
    /*
    write(dd[1], "test1", 5);
    usleep(10000);
    write(dd[1], "\r\n", 2);
    usleep(10000);
    write(dd[1], "te", 2);
    usleep(10000);
    write(dd[1], "st2\r\n", 5);
    usleep(10000);
    write(dd[1], "test3\r\ntest4\r\n", 17);
    */
    // RUN jsonrpc
    io_service.run();
  });
    
  std::vector<std::string> result;
    
  for (int i=0; i < 5; ++i)
  {
    reader.async_read([&result]( wfc::io::data_ptr d){
      std::cout << "----> " << std::string(d->begin(), d->end()) <<std::endl;
      result.push_back( std::string(d->begin(), d->end()) );
    });
  }
  
  while ( result.size() != 4 )
  {
    std::cout << "result.size() " << result.size() << std::endl;
    io_service.poll_one();
    usleep(100000);
  }
  std::cout << "result.size() " << result.size() << std::endl;
  
  io_service.stop();
  th.join();
    
  if (result.size() != 4)
    abort();

  if (result[0] != "test1")
    abort();

  if (result[1] != "test2")
    abort();
    
  if (result[2] != "test3")
    abort();
    
  if (result[3] != "test4")
    abort();    
}

