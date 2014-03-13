/*
#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/jsonrpc/handler.hpp>
*/
#include <wfc/service/rn/jsonrpc/service.hpp>
#include <wfc/io_service.hpp>
#include <wfc/io/types.hpp>
#include <boost/asio.hpp>
#include <wfc/jsonrpc/handler.hpp>
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

JSONRPC_TAG(test1)
JSONRPC_TAG(test2)

/*
struct method_test1: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test1_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
>{};*/

struct method_test2: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test2_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
>{};


struct method_list: wfc::jsonrpc::method_list<
  wfc::jsonrpc::target<itest>,
  wfc::jsonrpc::invoke_method<_test1_, test1_json, test1_json, itest, &itest::test1>,
  method_test2
>{};




void test_loop(wfc::io_service& io_service)
{
  io_service.stop();
}

//typedef wfc::io::ip::tcp::rn::jsonrpc::server server;

/*
 {\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}\r\n
 
 {"method":"test1","params":[1,2,3,4,5],"id":1}
 {"method":"test2","params":[1,2,3,4,5],"id":1}
 */

int main(int argc, char* [])
{
  using namespace std::placeholders;
  wfc::io_service io_service;

  /*server::options_type conf;
  conf.acceptors = 10;
  conf.threads = 2;
  */
  wfc::io_service::work wrk(io_service);
  
  auto ptest = std::make_shared<test>();
  
  wfc::service::rn::jsonrpc::configuration config;
  
  config.jsonrpc.services[0].threads = 4;
  config.jsonrpc.services[0].queues[0].count = 16;
  
  config.tcp[0].acceptors = 4;
  auto factory = wfc::service::rn::jsonrpc::make_factory<method_list>(ptest);
  wfc::service::rn::jsonrpc::service service( io_service, config, factory);

  /*
  wfc::jsonrpc::options jsonrpc_options;
  jsonrpc_options.services[0].threads = 1;
  jsonrpc_options.services[0].queues[0].count = 16;
  */
  
  /*
  auto jsonrpc = std::make_shared<wfc::jsonrpc::service>(io_service, jsonrpc_options, wfc::jsonrpc::handler<method_list>(ptest) ) ;
  auto srv = std::make_shared<server>(io_service, conf, jsonrpc );
  */
  
  /*
  jsonrpc->start();
  srv->start();
  */
  
  service.start();
  
  std::thread th;
  
  if (argc == 1)
    th = std::thread( std::bind(test_loop, std::ref(io_service) ) );
  
  std::cout << "run" << std::endl;
  io_service.run();
  std::cout << "done" << std::endl;
  
  service.stop();
  //!!! jsonrpc.stop();
  
  if (argc == 1)
    th.join();
}
