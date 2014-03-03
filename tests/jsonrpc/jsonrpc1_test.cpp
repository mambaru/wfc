#include <iostream>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io_service.hpp>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>


struct reader_aspect: 
  fas::aspect<
    wfc::io::rn::reader::aspect2<wfc::io::reader::_incoming_>,
    wfc::io::reader::stream< boost::asio::posix::stream_descriptor, wfc::io::rn::reader::_incoming_>,
    wfc::io::reader::error_log
  >
{};    

namespace wfc{ namespace jsonrpc{

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class jsonrpc_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< jsonrpc_base<A, AspectClass> >
{
public:
  typedef jsonrpc_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;

  jsonrpc_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
    , handler (std::bind( &self::operator(), this, std::placeholders::_1, std::placeholders::_2) )
  {
    super::create(*this);
  }

  void doit( data_ptr d, wfc::io::callback handler)
  {
    std::cout << "JSON RPC {" <<  std::string(d->begin(), d->end())<< std::endl;
    handler( std::move(d) );
    std::cout << "}JSON RPC" << std::endl;
  }

  
  void operator()( data_ptr d, wfc::io::callback handler)
  {
    std::cout << "JSON RPC operator(1) " <<  (d != nullptr) << std::endl;

    auto dd = std::make_shared<data_ptr>(std::move(d));
    this->dispatch( [this, dd,  handler]() {
      std::cout << "JSON RPC operator(2) " <<  ( *dd != nullptr) << std::endl;
      this->doit( std::move(*dd), handler);
    });
  }
  
  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }

  wfc::io::handler handler;
};

struct options
{
  std::function<void()> not_alive = nullptr;
};

struct aspect: fas::aspect<
  fas::type<wfc::io::_options_type_, options>
  
> {};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class jsonrpc:
  public jsonrpc_base< typename fas::merge_aspect< A, aspect>::type, AspectClass>
{
public:
  typedef jsonrpc<A, AspectClass> self;
  typedef jsonrpc_base< typename fas::merge_aspect< A, aspect>::type, AspectClass> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  jsonrpc(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
  {
  }

};

}} // wfc

int main()
{
  int dd[2];
  ::pipe(dd);

  wfc::io_service io_service;
  wfc::jsonrpc::options options;
  wfc::jsonrpc::jsonrpc<> jsonrpc(io_service, options);
  
  wfc::io::rn::reader::options init;
  init.input_buffer_size = 8096;
  init.handler = jsonrpc.handler;
  typedef wfc::io::reader::reader< reader_aspect > reader_type;
  boost::asio::posix::stream_descriptor sd(io_service, dd[0]);
  reader_type reader( std::move(sd), init);    
  
  //jsonrpc.start(jsonrpc);
  
  std::thread th([&dd, &io_service]()
  {
    write(dd[1], "test1", 5);
    usleep(10000);
    write(dd[1], "\r\n", 2);
    usleep(10000);
    write(dd[1], "te", 2);
    usleep(10000);
    write(dd[1], "st2\r\n", 5);
    usleep(10000);
    write(dd[1], "test3\r\ntest4\r\n", 17);
    
    // RUN jsonrpc
    io_service.run();
  });
    
  std::vector<std::string> result;
    
  for (int i=0; i < 4; ++i)
  {
    auto d = reader.read();
    result.push_back( std::string(d->begin(), d->end()) );
  }
  
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

