#include <iostream>

#include <wfc/io/reader/aspect.hpp>
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/reader/read/async/aspect.hpp>
#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/rn/reader/check/aspect.hpp>
#include <string>
#include <boost/asio.hpp>

struct init_info 
{
  std::shared_ptr<boost::asio::io_service> io_service;
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
  size_t input_buffer_size;
  std::function<void()> not_alive;
  
  size_t rn_limit_error;
  size_t rn_limit_warning;
};

std::vector<std::string> result_list;

struct ad_reset_buffer
{
  template<typename T>
  void operator()(T& t, size_t)
  {
    t.get_aspect().template get<wfc::io::rn::reader::_buffer_>() = nullptr;
  }
};

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
  wfc::io::rn::reader::check::aspect,
  wfc::io::rn::reader::aspect,
  fas::alias<wfc::io::reader::_incoming_, wfc::io::rn::reader::_incoming_>,
  fas::advice<wfc::io::rn::reader::_outgoing_, ad_handler>,
  fas::advice<wfc::io::rn::reader::check::_on_limit_error_, ad_reset_buffer>,
  wfc::io::posix::aspect,
  wfc::io::reader::aspect,
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
  init.rn_limit_error = 0;
  init.rn_limit_warning = 0;
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;
    reader_type reader(init);
    reader.start();
    write(dd[1], "te", 2);
    io_service->run_one();
    write(dd[1], "st1\r\n", 5);
    io_service->run_one();
    write(dd[1], "test2\r", 6);
    io_service->run_one();
    write(dd[1], "\n", 1);
    io_service->run_one();
  }
  
  if ( result_list.size()!=2 )
    abort();
  if ( result_list[0]!="test1" )
    abort();
  if ( result_list[1]!="test2" )
    abort();
}
