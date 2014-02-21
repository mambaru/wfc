#include <iostream>

#include <wfc/io/reader/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/reader/read/aspect.hpp>
#include <string>
#include <boost/asio.hpp>

struct init_info 
{
  std::shared_ptr<boost::asio::io_service> io_service;
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
  size_t input_buffer_size;
  std::function<void()> not_alive;
};

std::vector<std::string> result_list;

struct ad_handler
{
  template<typename T>
  void operator()(T& , typename T::data_ptr d)
  {
    std::cout << "handle " << std::string(d->begin(), d->end() ) << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
  }
};

struct reader_aspect: fas::aspect<
  wfc::io::context<>,
  fas::advice<wfc::io::_incoming_, ad_handler>,
  fas::alias<wfc::io::reader::_outgoing_, wfc::io::_incoming_>,
  //fas::advice<wfc::io::reader::_outgoing_, ad_handler>,
  wfc::io::reader::loop::aspect,
  wfc::io::reader::aspect,
  wfc::io::posix::aspect,
  wfc::io::reader::read::aspect,
  wfc::io::basic::aspect
> {};

int main()
{
  /*
  size_t test_count = 0;
  size_t handler_count = 0;
  size_t not_alive_count = 0;
  std::function< void() > handler = nullptr;
  */
  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  init_info init;
  init.io_service = io_service;
  init.native_handle = dd[0];
  init.input_buffer_size = 8096;
  init.not_alive = nullptr;
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;

    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    reader.start();
    write(dd[1], "test1", 5);
    io_service->run_one();
    write(dd[1], "test2", 5);
    io_service->run_one();
  }
  
  
  if ( result_list.size()!=2 )
    abort();
  if ( result_list[0]!="test1" )
    abort();
  if ( result_list[1]!="test2" )
    abort();
}
