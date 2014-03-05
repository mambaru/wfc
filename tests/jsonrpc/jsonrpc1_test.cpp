#include <iostream>

#include <wfc/io/posix/rn/reader.hpp>
#include <wfc/jsonrpc/service.hpp>

#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>

typedef std::vector<int> test1_params;
typedef wfc::json::array< std::vector< wfc::json::value<int> > > test1_json;


struct itest
{
  virtual ~itest() {}
  virtual void test1(std::unique_ptr<test1_params> req, std::function< wfc::callback_status(std::unique_ptr<test1_params>) > callback) = 0;
};

class test: public itest
{
public:
  
  virtual void test1(std::unique_ptr<test1_params> req, std::function< wfc::callback_status(std::unique_ptr<test1_params>) > callback)
  {
    std::cout << "test::test" << std::endl;
    std::reverse(req->begin(), req->end());
    callback(std::move(req));
  }
  
};

FAS_STRING(_test1_, "test1")
FAS_STRING(_test2_, "test2")


struct test1_handler
{
  template<typename T>
  void operator()(T& t, std::unique_ptr<test1_params> req, std::function< wfc::callback_status(std::unique_ptr<test1_params>, std::unique_ptr<wfc::jsonrpc::error>) > callback)
  {
    callback(std::move(req), nullptr);
  }
};

struct method_test1: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test1_>,
  wfc::jsonrpc::invoke<test1_json, test1_json, wfc::jsonrpc::mem_fun_handler<test1_params, test1_params, itest, &itest::test1> >
>{};

struct method_test2: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test2_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >
>{};


struct method_list: wfc::jsonrpc::method_list< fas::aspect<
  wfc::jsonrpc::target<itest>,
  wfc::jsonrpc::add_method<method_test1>,
  wfc::jsonrpc::add_method<method_test2>
> >{};



int main()
{
  int dd[2];
  ::pipe(dd);

  typedef wfc::jsonrpc::handler<method_list> instance_wrapper;
  
  wfc::io_service io_service;
  wfc::io_service::work wrk(io_service);
  wfc::jsonrpc::options options;
  auto t = std::make_shared<test>();
  options.instance = std::make_shared<instance_wrapper>( t );
  wfc::jsonrpc::service<> jsonrpc(io_service, options);
  
  typedef wfc::io::posix::rn::reader reader_type;
  reader_type::options_type reader_options;
  
  reader_options.handler = jsonrpc.handler;
  
  reader_type::descriptor_type desc(io_service, dd[0]);
  reader_type reader = reader_type( std::move(desc), reader_options );
  /*
  wfc::io::rn::reader::options init;
  init.input_buffer_size = 8096;
  init.handler = jsonrpc.handler;
  
  typedef wfc::io::reader::reader< reader_aspect > reader_type;
  boost::asio::posix::stream_descriptor sd(io_service, dd[0]);
  reader_type reader( std::move(sd), init);    
  */
  
  //jsonrpc.start(jsonrpc);
  
  std::thread th([&dd, &io_service]()
  {
    const char *req1="\t {\"method\":\"test1\",\"params\":[1,2,3,4,5],\"id\":12345} \t\t {\"method\":\"test2\",\"params\":[1,2,3,4,5],\"id\":12345} \r\n";
    const char *req2="{-}\r\n";
    const char *req3="--\r\n";
    const char *req4="{\"id\":[\"66!\"]}\r\n";
    
    write(dd[1], req1, std::strlen(req1) );
    write(dd[1], req2, std::strlen(req2) );
    write(dd[1], req3, std::strlen(req3) );
    write(dd[1], req4, std::strlen(req4) );
    /*
    write(dd[1], "test1", 5);
    usleep(10000);
    write(dd[1], "\r\n", 2);
    usleep(10000);
    write(dd[1], "te", 2);
    usleep(10000);
    write(dd[1], "st2\r\n", 5);
    usleep(10000);
    write(dd[1], "test3\r\ntest4\r\n", 17);
    */
    // RUN jsonrpc
    io_service.run();
  });
    
  std::vector<std::string> result;
  while( reader.status() ) 
  {
    std::cout << "WHILE { " << std::endl;
    if ( auto d = reader.read() )
    {
      std::cout << "READ READY " << std::string(d->begin(), d->end()) << std::endl;
      result.push_back( std::string(d->begin(), d->end()) );
    }
    else
    {
      std::cout << "READ NOT READY " << std::endl;
    }
    
    if ( result.size() == 5)
      break;
    
    std::cout << "} WHILE" << std::endl;
  }
  
  
  /*
  reader.start();
  for (int i=0; i < 50; ++i)
  {
    io_service.poll_one();
  }
  reader.stop();
  */
  /*
  for (int i=0; i < 5; ++i)
  {
    reader.async_read([&result]( wfc::io::data_ptr d){
      std::cout << "----> " << std::string(d->begin(), d->end()) <<std::endl;
      result.push_back( std::string(d->begin(), d->end()) );
      return wfc::callback_status::ready;
    });
  }
  */
  
  /*
  while ( result.size() != 4 )
  {
    std::cout << "result.size() " << result.size() << std::endl;
    io_service.poll_one();
    usleep(100000);
  }
  */
  std::cout << "result.size() " << result.size() << std::endl;
  
  io_service.stop();
  th.join();
    
  if (result.size() != 4)
    abort();

  if (result[0] != "test1")
    abort();

  if (result[1] != "test2")
    abort();
    
  if (result[2] != "test3")
    abort();
    
  if (result[3] != "test4")
    abort();    
}

