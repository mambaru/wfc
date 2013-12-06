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

  const char *name() {return "test_method";}

  template<typename T>
  void request(T& t, std::unique_ptr<test_request> req, int id, std::function<void(std::unique_ptr<int>)> callback)
  {
    callback(std::unique_ptr<int>(new int( req->a + req->b )));
  }

  template<typename T>
  void notify(T& t, std::unique_ptr<test_request> req)
  {
    std::cout << "test_notify" << std::endl;
    t.get_aspect().template get<_test_method_>().notify(t,  std::unique_ptr<int>(new int( req->a + req->b )));
    t.get_aspect().template get<_test_method_>().request(t, std::unique_ptr<test_request>(new test_request{2, 3}) );
  }

  template<typename T>
  void response(T& t, std::unique_ptr<int> resp,  int id)
  {
    std::cout << "test_notify response " << *resp <<  "id = " <<  id <<  std::endl;
  }
};

template<typename Tg, typename M>
struct method:
  fas::type_list_n<
    fas::advice< Tg, inet::jsonrpc::method<M> >,
    fas::group<inet::jsonrpc::_method_, Tg>
  >::type
{};



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

typedef inet::tcp_server<
    inet::echo_connection<
      fas::aspect< fas::type_list_n<
        fas::group<inet::_start_, _start_>,
        fas::advice<_start_, ad_start>,
        fas::alias< inet::basic::_incoming_, inet::rn::_input_>,
        fas::alias< inet::rn::_incoming_,    inet::jsonrpc::_input_>,
        fas::alias< inet::jsonrpc::_output_, inet::rn::_outgoing_>,
        fas::alias< inet::jsonrpc::_not_jsonrpc_, inet::rn::_outgoing_>,
        fas::alias< inet::rn::_output_, inet::basic::_outgoing_>,
        method<_test_method_, test_method>,
        inet::jsonrpc::aspect,
        inet::rn::aspect
        /*fas::alias< inet::rn::_incomin >
        
        */
        
      >::type>
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
