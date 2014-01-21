#include <wfc/jsonrpc/server_tcp.hpp>
#include <wfc/jsonrpc/client/client.hpp>
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