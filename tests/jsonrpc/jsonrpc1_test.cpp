#include <iostream>
#include <wfc/jsonrpc.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/types.hpp>
std::string requests[][2] = {
  { 
    "{\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":1}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":1}"
  },
  { 
    "{\"method\":\"test2\",\"params\":[1,2,3,4,5]}",
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found.\"},\"id\":null}"
  },
  {
    "{\"method\":\"test3\",\"params\":{1,2,3,4,5},\"id\":1}",
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

struct nomethod:
  wfc::jsonrpc::method_list<>
{};

typedef wfc::jsonrpc::handler<nomethod> nohandler;

int test_count = 0;

int main()
{
  wfc::io_service ios;
  wfc::jsonrpc::service::options_type opt = wfc::jsonrpc::service::options_type::create();
  
  wfc::jsonrpc::service service(ios, opt, nohandler());
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
  
  if ( test_count!=7 )
  {
    std::cout << "ERROR. invalid test count " << test_count << std::endl;
    exit(-1);
  }
  return 0;
}
