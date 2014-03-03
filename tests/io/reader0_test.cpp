#include <iostream>

#include <wfc/logger/ilogger.hpp>
#include <wfc/core/global.hpp>

/*
#include <wfc/io/reader/common/trace/aspect.hpp>
#include <wfc/io/reader/errors/log/aspect.hpp>
#include <wfc/io/reader/read/sync/aspect.hpp>
#include <wfc/io/basic/log/aspect.hpp>
*/
/*#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/sync_read_return.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/config.hpp>
*/
#include <wfc/io/reader/reader.hpp>
#include <string>
#include <boost/asio.hpp>


/*
typedef wfc::io::strategy::posix::reader::config init_info;

struct reader_aspect: 
  fas::aspect<
    fas::advice< wfc::io::_options_type_, init_info>,
    wfc::io::strategy::posix::reader::trace,
    wfc::io::strategy::posix::reader::log,
    wfc::io::strategy::posix::reader::sync_read_return
  >
{};
*/

struct reader_aspect
  : fas::aspect<
      wfc::io::reader::stream< boost::asio::posix::stream_descriptor >,
      wfc::io::reader::error_log
    >
{};

    

int main()
{
  std::cout << sizeof(boost::system::error_code) << std::endl;
  
  size_t handler_count = 0;
  size_t not_alive_count = 0;
  std::function< void() > handler = nullptr;

  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  wfc::io::reader::options init;
  init.input_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count;};
  
  {
    
    typedef wfc::io::reader::reader< reader_aspect > reader_type;
    
    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
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
    close(dd[0]);
    
    if ( !reader.status() )
      abort();
    
    d = reader.read();
    
    if ( d!=nullptr )
      abort();

    if ( reader.status() )
      abort();
    
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

