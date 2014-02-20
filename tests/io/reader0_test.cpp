#include <iostream>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/reader/read/sync/aspect.hpp>
#include <string>
#include <boost/asio.hpp>


struct init_info 
{
  std::shared_ptr<boost::asio::io_service> io_service;
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
  size_t input_buffer_size;
  std::function<void()> not_alive;
};

int main()
{
  size_t handler_count = 0;
  size_t not_alive_count = 0;
  std::function< void() > handler = nullptr;

  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  init_info init;
  init.io_service = io_service;
  init.native_handle = dd[0];
  init.input_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count;};
  
  
  {
    typedef wfc::io::reader::reader< wfc::io::reader::read::sync::aspect > reader_type;
    reader_type reader(init);
    
    handler = reader.wrap([&](){ ++handler_count;});
    
    write(dd[1], "test1", 5);
    auto d = reader.read();
    std::string result( d->begin(), d->end() );
    if ( result != "test1" )
      abort();
    
    write(dd[1], "test2", 5);
    d = reader.read();
    result.assign( d->begin(), d->end() );
    if ( result != "test2" )
      abort();
    
    reader.get_io_service().post( handler );
    io_service->run_one();
    reader.get_io_service().post( handler );
  }
  
  // not posted last handler
  io_service->run_one();
  // execute not alive
  handler();
  
  if ( handler_count!=1 || not_alive_count!=2)
  {
    abort();
  }
}

