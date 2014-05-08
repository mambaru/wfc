#include <iostream>

#include <wfc/io/posix/rn/reader.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/method.hpp>
#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_json.hpp>

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

/*
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
*/

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

/*
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
        if ( error==nullptr){\
          if ( resp != nullptr) \
            callback( std::move(resp) );\
          else\
            callback( nullptr );\
        };\
      });\
    }\
  }
  */

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

  typedef wfc::jsonrpc::handler<method_list> handler_type;
  //handler_type handler;
  auto handler = std::make_shared<handler_type>();
  /*
  handler.outgoing_request_handler=[]( handler::incoming_handler_t, handler::request_serializer_t ser) {
    auto d = ser(333);
    std::cout << "call JSON READY " << std::string( d->begin(), d->end() ) << std::endl;
  };
  */
  
  wfc::io_service io_service;
  wfc::io_service::work wrk(io_service);
  wfc::jsonrpc::options options;
  //options.handler = phndl;
  wfc::jsonrpc::service jsonrpc(io_service, options, *handler);

  /*
  jsonrpc.attach_handler( 1, handler, [&jsonrpc](wfc::io::data_ptr d)
  {
    wfc::jsonrpc::incoming_holder holder( std::move(d) );
    wfc::jsonrpc::outgoing_result<test1_params> result;
    result.id = std::move(holder.raw_id());
    result.result = std::make_unique<test1_params>(test1_params({5,4,3,2,1}));
    auto dd = std::make_unique<wfc::io::data_type>();
    wfc::jsonrpc::outgoing_result_json<test1_json>::serializer() ( result, std::back_inserter(*dd) );
    jsonrpc.operator()( std::move(dd), 1, []( wfc::io::data_ptr){} );
  });
  */
  

  jsonrpc.start();
  
  
  std::thread th([&](){
    io_service.run();
  });

  handler->test1( 
    std::make_unique<test1_params>( test1_params({1,2,3,4,5}) ), 
    []( std::unique_ptr<test1_params> res)
    {
      std::cout << "Result ready" << std::endl;
      if ( res == nullptr)
        abort();

      if ( res->size() != 5)
        abort();
      for (int i =0; i < 5;++i)
      {
        if (res->at(i) != (5-i) )
          abort();
      }
      std::cout << "Done!" << std::endl;
      exit(0);
    }
  );
  
  while (1){
    sleep(1);
  }

  /*
  for (int i=0;i<10;++i)
    io_service.poll_one();
  
  phndl->test1( 
    std::make_unique<test1_params>( test1_params({1,2,3,4,5}) ), 
    []( std::unique_ptr<test1_params>)
    {
      exit(0);
    }
  );
  shh(4);
  
  for (int i=0;i<10;++i)
    io_service.poll_one();
  */
  
}

