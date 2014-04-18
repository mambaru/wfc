#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/jsonrpc/handler.hpp>
#include <wfc/io_service.hpp>
#include <wfc/io/types.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <algorithm>

typedef std::vector<int> test1_params;
typedef wfc::json::array< std::vector< wfc::json::value<int> > > test1_json;


struct itest
{
  virtual ~itest() {}
  virtual void test1(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback) = 0;
};

class test: public itest
{
public:
  virtual void test1(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback) 
  {
    std::reverse(req->begin(), req->end());
    callback( std::move(req) );
  }
};

FAS_STRING(_test1_, "test1")
FAS_STRING(_test2_, "test2")

struct method_test1: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test1_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
>{};

struct method_test2: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test2_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
>{};


struct method_list: wfc::jsonrpc::method_list<
  wfc::jsonrpc::target<itest>,
  method_test1,
  method_test2
>{};




void test_loop(wfc::io_service& io_service)
{
  io_service.run();
}

typedef wfc::io::ip::tcp::rn::jsonrpc::server server;

/*
 {\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}\r\n
 
 {"method":"test1","params":[1,2,3,4,5],"id":1}
 {"method":"test2","params":[1,2,3,4,5],"id":1}
 */

int main(int argc, char* [])
{
  using namespace std::placeholders;
  wfc::io_service io_service;

  server::options_type conf;
  // conf.acceptors = 10;
  conf.threads = 1;
  conf.host = "0.0.0.0";
  conf.port = "12345";
  wfc::io_service::work wrk(io_service);
  
  auto ptest = std::make_shared<test>();

  wfc::jsonrpc::options jsonrpc_options;
  jsonrpc_options.workers.resize(1);
  jsonrpc_options.workers[0].strands.resize(1);
  jsonrpc_options.workers[0].threads = 1;
  jsonrpc_options.workers[0].strands[0].count = 1;
  
  auto jsonrpc = std::make_shared<wfc::jsonrpc::service>(io_service, jsonrpc_options, wfc::jsonrpc::handler<method_list>(ptest) ) ;
  auto srv = std::make_shared<server>(io_service, conf, jsonrpc );
  
  jsonrpc->start();
  srv->start();
  
  
  std::thread th;
  
  if (argc == 1)
    th = std::thread( std::bind(test_loop, std::ref(io_service) ) );

  sleep(1);
  
  std::cout << "server stop..." << std::endl;
  srv->stop();
  std::cout << "jsonrpc stop..." << std::endl;
  jsonrpc->stop();
  std::cout << "io_service stop..." << std::endl;
  io_service.stop();
  if (argc == 1)
  {
    std::cout << "thread join..." << std::endl;
    th.join();
  }
  std::cout << "done" << std::endl;
}
