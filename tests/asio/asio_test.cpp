#include <comet/asio/server.hpp>
#include <comet/asio/jsonrpc/aspect.hpp>
#include <comet/asio/jsonrpc/method.hpp>
#include <comet/asio/rn/aspect.hpp>
#include <comet/asio/rn/aspect.hpp>
#include <boost/asio.hpp>
#include <signal.h>
using namespace ::mamba::comet;

struct test_request
{
  int a;
  int b;
};

struct test_request_json
{
  NAME(a)
  NAME(b)
  typedef json::object<
    test_request,
    fas::type_list_n<
      json::member<n_a, test_request, int, &test_request::a>,
      json::member<n_b, test_request, int, &test_request::b>
    >::type
  > type;
  typedef type::serializer serializer;
};

struct _test_method_;

struct test_method
{
  typedef test_request_json::type invoke_request;
  typedef test_request_json::type invoke_notify;
  //typedef test_request_json::type invoke_response;
  typedef json::value<int> invoke_response;

  typedef json::value<int> call_notify;
  typedef test_request_json::type call_request;
  typedef json::value<int> call_response;

  std::function<void()> _callback;
  
  test_method()
    : _callback(nullptr)
  {}
  
  const char *name() const {return "test_method";}
  
  

  template<typename T, typename Callback>
  void request(T& t, std::unique_ptr<test_request> req, int id, Callback callback)
  {
    if ( _callback != nullptr )
      _callback();
    test_request r = *req;
    _callback=[r, callback]()
    {
      callback(std::make_unique<int>(r.a + r.b), nullptr);
    };
  }
 
  /*
  template<typename T>
  void request(T& t, std::unique_ptr<test_request> req, int id, std::function<callback_status(std::unique_ptr<int>, std::unique_ptr<inet::jsonrpc::error>)> callback)
  {
    callback(std::make_unique<int>(req->a + req->b), nullptr);
  }

  template<typename T, typename Cal>
  void request(T& t, std::unique_ptr<test_request> req, int id, Callback callback)
  {
    callback(std::make_unique<int>(req->a + req->b), nullptr);
  }
  
  std::function<callback_status(std::unique_ptr<test_response>, std::unique_ptr<inet::jsonrpc::error>)> callback
*/
  

  template<typename T>
  void notify(T& t, std::unique_ptr<test_request> req)
  {
    std::cout << "test_notify" << std::endl;
    t.get_aspect().template get<_test_method_>().notify(t,  std::unique_ptr<int>(new int( req->a + req->b )));
    t.get_aspect().template get<_test_method_>().request(
      t, 
      std::unique_ptr<test_request>(new test_request{2, 3}), 
      [](std::unique_ptr<int> resp, std::unique_ptr<inet::jsonrpc::error> err)
      {
        if ( !err )
        {
          if ( resp )
            std::cout << "RESULT " << *resp << std::endl;
          else
            std::cout << "RESULT NULL" << std::endl;
        }
        else
        {
          std::cout << "ERROR " << err->message << std::endl;
        }
        return callback_status::ready;
      }
    );
  }

  template<typename T>
  void response(T& t, std::unique_ptr<int> resp,  int id)
  {
    std::cout << "test_notify response " << *resp <<  "id = " <<  id <<  std::endl;
  }
};




struct _start_;
struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "START!" << std::endl;
    std::string str;
    for (int i =0; i < 100; ++i)
    {
      str="[[[";
      t.write(str.c_str(), str.size());
      std::stringstream ss;
      for (int k =0; k < 100; ++k)
        ss << "Привет Чувак "<< i<<"-" << k <<",";
      ss << "]]]\r\n";
      str = ss.str();
      t.write(str.c_str(), str.size());
    }
    std::cout << "ALL WRITE!" << std::endl;
  }
};

struct test_aspect:
  fas::aspect< fas::type_list_n<
        fas::group<inet::_start_, _start_>,
        fas::advice<_start_, ad_start>,
        fas::alias< inet::basic::_incoming_, inet::rn::_input_>,
        fas::alias< inet::rn::_incoming_,    inet::jsonrpc::_input_>,
        fas::alias< inet::jsonrpc::_output_, inet::rn::_outgoing_>,
        // fas::alias< inet::jsonrpc::_not_jsonrpc_, inet::rn::_outgoing_>,
        fas::alias< inet::rn::_output_, inet::basic::_outgoing_>,
        inet::jsonrpc::method<_test_method_, test_method>,
        inet::jsonrpc::aspect,
        inet::rn::aspect
        /*fas::alias< inet::rn::_incomin >
        
        */
        
      >::type>
{};

typedef inet::tcp_server<
    inet::echo_connection<
      test_aspect
    >
  > server_type;
typedef server_type::connection_type connection_type;

std::shared_ptr<connection_type> conn;


int main(int argc, char* argv[])
{
    
  inet::server_config conf;
  conf.enabled = true;
  conf.host = "0.0.0.0";
  conf.port = "12345";

  
  ::boost::asio::io_service io_service;
  ::boost::asio::signal_set ss(io_service);
  server_type srv(io_service);

  ss.add(SIGINT);
  ss.add(SIGTERM);

  ss.async_wait(
      [&srv](boost::system::error_code /*ec*/, int /*signo*/)
      {
        std::cout << "stop..." << std::endl;
        srv.stop();
        std::cout << "stop! Done!" << std::endl;
      });

  
  srv.context();
  srv.start(conf);
  io_service.run();
  std::cout << "STOP" << std::endl;
  std::cin.get();
  // conn.write("final!", 6);
  return 0;
}
