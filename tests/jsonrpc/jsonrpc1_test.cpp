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
  
  int dd[2];
  ::pipe(dd);

  /*
  typedef wfc::jsonrpc::handler<method_list> instance_wrapper;
  
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

