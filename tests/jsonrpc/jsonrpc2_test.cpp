#include <iostream>
#include <wfc/jsonrpc.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/types.hpp>
#include <algorithm>
std::string requests[][2] = {
  { 
    "{\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"result\":[5,4,3,2,1],\"id\":1}"
  },
  { 
    "{\"method\":\"test2\",\"params\":[1,2,3,4,5],\"id\":2}",
    "{\"jsonrpc\":\"2.0\",\"result\":[5,4,3,2,1],\"id\":2}"
  },
  {
    "{\"method\":\"test1\",\"params\":{1,2,3,4,5},\"id\":3}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error.\"},\"id\":null}"
  },
  {
    "{\"method\":\"test3\",\"params\":{1,2,3,4,5}}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error.\"},\"id\":null}"
  },
  {
    "{\"method\":\"test4\",\"params\":{1,2,3",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error.\"},\"id\":null}"
  },
  {
    "{\"method\":null,\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}"
  },
  {
    "{\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request.\"},\"id\":1}"
  },
  {
    "{\"result\":[1,2,3,4,5],\"id\":1}",
    "- ignored -"
  },
  {
    "{\"result\":[1,2,3,4,5]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request.\"},\"id\":null}"
  }
};

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
    if ( callback )
      callback(std::move(req));
  }
};


JSONRPC_TAG(test1)
JSONRPC_TAG(test2)


struct method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::target<itest>,
  wfc::jsonrpc::invoke_method< _test1_, test1_json,      test1_json,      itest, &itest::test1>,
  wfc::jsonrpc::invoke_method< _test2_, test1_json,      test1_json,      itest, &itest::test1>
>
{};


typedef wfc::jsonrpc::handler<method_list> handler;

int test_count = 0;

int main()
{
  auto tst = std::make_shared<test>();
  wfc::io_service ios;
  wfc::jsonrpc::service::options_type opt = wfc::jsonrpc::service::options_type::create();
  opt.workers[0].threads = 10;
  
  wfc::jsonrpc::service service(ios, opt, handler(tst) );
  service.start();
  for (auto r: requests)
  {
    auto req = std::make_unique<wfc::io::data_type>( r[0].begin(), r[0].end() );
    service( std::move(req), 1, [r](wfc::io::data_ptr res)
    {
      if ( std::string(res->begin(), res->end()) != r[1] )
      {
        std::cout << "ERROR" << std::endl;
        std::cout << "request: "  << r[0] << std::endl;
        std::cout << "response: " << std::string(res->begin(), res->end()) << std::endl;
        std::cout << "should be: " << r[1] << std::endl;
        exit(-1);
      }
      ++test_count;
    });
  }
  ios.poll();
  
  if ( test_count!=8 )
  {
    std::cout << "ERROR. invalid test count " << test_count << std::endl;
    exit(-1);
  }
  return 0;
}

