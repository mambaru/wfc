#include <iostream>

#include <wfc/io/posix/rn/reader.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/jsonrpc/handler.hpp>

#include <string>
#include <thread>
#include <functional>

#include <boost/asio.hpp>

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
    std::cout << "------------------------------> test::test1 READY " << std::endl;
    std::reverse(req->begin(), req->end());
    callback( std::move(req) );
    //abort();
  }
};

FAS_STRING(_test1_, "test1")
FAS_STRING(_test2_, "test2")

struct method_test1: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test1_>,
  wfc::jsonrpc::invoke<test1_json, test1_json, wfc::jsonrpc::mem_fun_handler<test1_params, test1_params, itest, &itest::test1> >
>{};

struct method_test2: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test2_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
>{};


struct method_list: wfc::jsonrpc::method_list<

  wfc::jsonrpc::target<itest>,
  
  method_test1,
  
  wfc::jsonrpc::method<
    wfc::jsonrpc::name<_test2_>,
    wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
  >
>{};



int main()
{
  using namespace std::placeholders;
  
  int dd[2];
  ::pipe(dd);

  wfc::io_service io_service;
  wfc::io_service::work wrk(io_service);

  auto t = std::make_shared<test>();

  typedef wfc::jsonrpc::handler<method_list> handler_type;
  handler_type test_handler(t);
  
  /*
  
  options.startup_handler = std::bind( &startup_handler::operator(), &jsonrpc, _1, _2, _3);
  */
  
  wfc::jsonrpc::service_options options;
  typedef wfc::jsonrpc::service service_type;
  service_type jsonrpc(io_service, options, test_handler);
  
  typedef wfc::io::posix::rn::reader reader_type;
  reader_type::options_type reader_options;
  reader_options.startup_handler = std::bind( &service_type::startup_handler, &jsonrpc, _1, _2, _3);
  
  auto jsonrpc_handler = std::bind( &service_type::operator(), &jsonrpc, _1, _2, _3);
  boost::asio::posix::stream_descriptor sd(io_service, dd[0]);
  reader_type reader( std::move(sd), reader_options, jsonrpc_handler  );
  
  
  const char *test1 = "{\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}\r\n";
  ::write(dd[1], test1, std::strlen(test1) );
  jsonrpc.start();
  reader.start();
  
  std::thread th([&](){
    io_service.run();
  });
  
  wfc::io::data_ptr d = nullptr;
  
  /*while( d == nullptr ) 
  {
    d = reader.read();
  }
  */
  
  reader.async_read([&](wfc::io::data_ptr d)
  {
    std::cout << "Done! " << std::string(d->begin(), d->end() ) << std::endl;
    io_service.stop();
  });
  th.join();
  
  
  /*wfc::io::data_ptr d = nullptr;
  while ()
    */
  
  //reader_options.handler = std::bind( jsonrpc, jsonrpc::operator(), _1, _2, _3);
  /*
  wfc::io_service io_service;
  wfc::io_service::work wrk(io_service);
  wfc::jsonrpc::service_options options;
  auto t = std::make_shared<test>();
  options.handler = std::make_shared<instance_wrapper>( t );
  wfc::jsonrpc::service<> jsonrpc(io_service, options);
  
  typedef wfc::io::posix::rn::reader reader_type;
  reader_type::options_type reader_options;
  
  reader_options.handler = jsonrpc.handler;
  */
}

