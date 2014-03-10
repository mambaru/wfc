#include <iostream>

#include <wfc/io/posix/rn/reader.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/errors.hpp>

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
    std::cout << "test::test" << std::endl;
    std::reverse(req->begin(), req->end());
    callback(std::move(req));
  }
  
};

FAS_STRING(_test1_, "test1")
FAS_STRING(_test2_, "test2")


struct method_test1: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test1_>,
  wfc::jsonrpc::call<test1_json, test1_json>
>{};


/*

  virtual void test1(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback)
  {
    typedef super::aspect::advice_cast<_>
    if ( callback == nullptr )
    {
      this->call<_test1_>( std::move(req), nullptr );
    }
    else
    {
      this->call<_test1_>( std::move(req), [callback](std::unique_ptr<test1_params> resp, std::unique_ptr<wfc::jsonrpc::error> error)
      {
        if ( error==nullptr)
          callback( std::move(resp) );
      });
    }

*/

#define JSONRPC_METHOD_IMPL(Tg, Method)\
  virtual void test1( call_params_ptr<Tg>::type req, std::function< void(call_result_ptr<Tg>::type) > callback)\
  {\
    if ( callback == nullptr )\
    {\
      this->call<Tg>( std::move(req), nullptr );\
    }\
    else\
    {\
      this->call<Tg>( std::move(req), [callback](call_result_ptr<Tg>::type resp, call_error_ptr<Tg>::type error)\
      {\
        if ( error==nullptr)\
          callback( std::move(resp) );\
      });\
    }\
  }

struct method_list: wfc::jsonrpc::method_list<

  wfc::jsonrpc::interface_<itest>,
  
  wfc::jsonrpc::method<
    wfc::jsonrpc::name<_test1_>,
    wfc::jsonrpc::call<test1_json, test1_json>
  >
>
{
  
public:
  
  JSONRPC_METHOD_IMPL( _test1_, test1 );
};



int main()
{
  int dd[2];
  ::pipe(dd);

  typedef wfc::jsonrpc::handler<method_list> handler;
  auto phndl = std::make_shared<handler>();
  phndl->outgoing_request_handler=[]( handler::incoming_handler_t, handler::request_serializer_t ser) {
    auto d = ser(333);
    std::cout << "call JSON READY " << std::string( d->begin(), d->end() ) << std::endl;
  };
  
  wfc::io_service io_service;
  wfc::io_service::work wrk(io_service);
  wfc::jsonrpc::service_options options;
  options.handler = phndl;
  wfc::jsonrpc::service<> jsonrpc(io_service, options);
  
  wfc::io::shutdown_handler_t shh = nullptr;
  jsonrpc.startup_handler(
    4, 
    []( wfc::io::data_ptr d) 
    { 
      std::cout << "call READY " << std::string( d->begin(), d->end() ) << std::endl;
    },
    [&shh]( wfc::io::shutdown_handler_t h)
    {
      std::cout << "shh init shutdown"<< std::endl;
      shh=h; 
    } 
  );
  jsonrpc.start();
  for (int i=0;i<10;++i)
    io_service.poll_one();
  
  phndl->test1( std::make_unique<test1_params>( test1_params({1,2,3,4,5}) ), nullptr );
  shh(4);
  
  for (int i=0;i<10;++i)
    io_service.poll_one();
  
}

