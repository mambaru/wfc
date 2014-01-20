#include <wfc/jsonrpc/server_tcp.hpp>
#include <wfc/json/json.hpp>
#include <boost/asio.hpp>

struct _reverse_;

/*
 * {"method":"reverse","params":[1,2,3,4,5], "id":1}
 */

struct reverse_method
{
  const char* name() const { return "reverse";}
  typedef wfc::json::array< std::vector< wfc::json::value<int> > > invoke_request;
  typedef wfc::json::array< std::vector< wfc::json::value<int> > > invoke_response;
  typedef wfc::json::array< std::vector< wfc::json::value<int> > > call_notify;

  typedef wfc::json::array< std::vector< wfc::json::value<int> > > call_request;
  typedef wfc::json::array< std::vector< wfc::json::value<int> > > call_response;
  
  template<typename T, typename F>
  void request(T& t, std::unique_ptr< std::vector< int > > req, int /*id*/, F callback)
  {
    std::reverse(req->begin(), req->end());
    callback( std::move(req), nullptr );
    
    std::cout << "reverse_method notify..." << std::endl;
    t.get_aspect().template get<_reverse_>().notify(t, std::make_unique< std::vector< int > >());
    std::cout << "reverse_method notify... done!" << std::endl;
    /*
    t.get_aspect().template get<_reverse_>().request(t, std::make_unique< std::vector< int > >(), 
    [](std::unique_ptr< std::vector< int > >, std::unique_ptr< wfc::inet::jsonrpc::error > ){
      std::cout << "READY!!!" << std::endl;
      return ::wfc::callback_status::ready;
    }); 
    */
  }
  
  
  
  template<typename T>
  void response(T& t, std::unique_ptr< std::vector< int > > resp,  int id)
  {
    std::cout << "READY" << std::endl;
  }
  
  

};

struct aspect_methods: wfc::jsonrpc::jsonrpc<
  wfc::jsonrpc::method<_reverse_, reverse_method>
>{};


struct method_list: fas::type_list_n<
  wfc::jsonrpc::method<_reverse_, reverse_method>
>::type{};


#include <wfc/inet/client/client.hpp>
#include <wfc/inet/client/client_tcp_config.hpp>
#include <wfc/inet/client/client_tcp_configurator.hpp>

namespace wfc{ namespace jsonrpc{
  
struct client_tcp_config
  : inet::client_tcp_config
{
  client_tcp_config()
  {}
};


struct client_tcp_configurator: 
  inet::client_tcp_configurator
{
  typedef client_tcp_config config_type;

  template<typename T>
  void operator()(T& t, const client_tcp_config& conf)
  {
    inet::client_tcp_configurator::operator()(t, conf);
    auto cntx = t.client_context();
    t.client_context(cntx);
  }
};


namespace connection{


struct aspect: fas::aspect< fas::type_list_n<
  aspect_error, 
  fas::advice<_send_json_, ad_send_json>,
  fas::advice<_invoke_, ad_invoke>,
  fas::alias<_input_, _invoke_>,
  fas::group<inet::_startup_, _invoke_>, 
  fas::stub<_method_stat_>
>::type >
{};

template<typename BaseContext = inet::empty_context>
struct context_stream: inet::stream::stream_context<BaseContext>
{
  bool close_after_response;
  context_stream()
    : inet::stream::stream_context<BaseContext>()
    , close_after_response(false)
  {};
};


struct aspect_stream_rn
  : fas::aspect<
      fas::advice< inet::_configurator_, client_tcp_configurator>, 
      inet::basic_context_t< context_stream >, 
      inet::stream::aspect,
      inet::rn::aspect,
      aspect,
      fas::alias< inet::stream::_incoming_, inet::rn::_input_        >,
      fas::alias< inet::rn::_output_,       inet::stream::_outgoing_ >,
      fas::alias< inet::rn::_incoming_,     /*jsonrpc::*/_input_   >,
      fas::advice< _write_ , ad_stream_write>,
      fas::alias< _output_, inet::rn::_outgoing_ >
  >
{};


// namespace rn{
  
template<typename ...Args>
struct tcp  
  : inet::connection_aspect<
      typename fas::merge_aspect< 
        fas::aspect<Args...>, 
        aspect_stream_rn
      >::type 
    >
{
};

}

namespace client {

/*
struct aspect_client_tcp: fas::merge_aspect<
  fas::aspect<
    //fas::advice<inet::_configuration_, wfc::inet::client_tcp_configurator>
  >,
  
>::type
{};*/


template<typename ...Args>
struct tcp: inet::client_aspect< 
  fas::aspect<Args..., inet::aspect_client_tcp>
>{};


template<typename ...Args>
struct client_tcp_helper
{
  // TODO: Убрать!!! указать явно!!
  typedef inet::client< Args... > client_base;
};
  
template<typename A = connection::tcp<>, typename B = tcp<>, typename ...Args>
class client
  : public inet::client<A, B, Args...>
{
public:
  typedef inet::client<A, B, Args...> super;
  
  
  client( ::wfc::io_service& io_service, const client_tcp_config& conf )
    : super(io_service, conf)
  {
  }

  client( std::weak_ptr< wfc::global > g, const client_tcp_config& conf )
    : super(g, conf)
  {
  }
};

}
  
}}

void test_client()
{
  // typedef int $;
  std::thread([]()
  {
    sleep(1);
    std::cout << "test_client()" << std::endl;
    wfc::jsonrpc::client_tcp_config config;
    config.host="0.0.0.0";
    config.port="12345";
    ::boost::asio::io_service io_service;
    wfc::jsonrpc::client::client< 
      wfc::jsonrpc::connection::tcp< method_list >,
      wfc::jsonrpc::client::tcp<>
      //aspect_methods 
    > client(io_service, config);
    std::cout << "test_client() client start" << std::endl;
    client.start();
    io_service.post([&client](){
      std::vector<int> vect={1,2,3,4,5};
      
      auto conn = client.connection().lock();
      
      conn->get_aspect().get<_reverse_>().request(*conn, std::make_unique< std::vector<int> >(vect), 
      [](std::unique_ptr< std::vector< int > > resp,  std::unique_ptr< wfc::jsonrpc::error > err)
      {
        if (!err)
        {
          std::cout << "JSON READY!!!! " << resp->size() << std::endl;
        }
        else
        {
          std::cout << "JSON ERROR!!!!" << std::endl;
        }
        
        return wfc::callback_status::ready;
        
      });
      
    });
    std::cout << "test_client() client run" << std::endl;
    io_service.run();
    std::cout << "test_client() STOP" << std::endl;
  }).detach();
  
}


int main()
{
  wfc::jsonrpc::server_tcp_config config;
  config.host="0.0.0.0";
  config.port="12345";
  ::boost::asio::io_service io_service;
  wfc::jsonrpc::server_tcp< aspect_methods > srv(io_service, config);
  test_client();
  srv.start();
  io_service.run();
  /*
  ::boost::asio::io_service io_service;
  wfc::jsonrpc::server_tcp< aspect_methods > srv(io_service);
  
  auto config = srv.server_context();
  config.host = "127.0.0.1";
  config.port = "12345";
  config.listen_threads = 0;
  config.worker_threads = 0;
  srv.server_context(config);
  srv.start();

  io_service.run();
  std::cout << "FINAL" << std::endl;
  std::cin.get();
  srv.stop();
  **/

  /*
  auto conn_conf = srv.connection_context();
  conn_conf.enable_stat = true;
  srv.connection_context(conn_conf);
  
  auto config = srv.server_context();
  config.host = "192.168.1.35";
  config.port = "80";
  config.listen_threads = 10;
  config.worker_threads = 10;
  srv.server_context(config);
  srv.start();
  //for (;;)
    io_service.run();
  std::cout << "FINAL" << std::endl;
  std::cin.get();
  srv.stop();
  */
}