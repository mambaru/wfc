#include <iostream>

#include <wfc/logger/ilogger.hpp>
#include <wfc/core/global.hpp>

/*
#include <wfc/io/reader/common/trace/aspect.hpp>
#include <wfc/io/reader/errors/log/aspect.hpp>
#include <wfc/io/reader/read/sync/aspect.hpp>
#include <wfc/io/basic/log/aspect.hpp>
*/
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/sync_read.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <string>
#include <boost/asio.hpp>


class logger
  : public wfc::ilogger
{
public:
  virtual void initialize(const std::string& , std::stringstream& )
  {
    
  }
  
  virtual void write(const std::string& name, const std::string& ident,  const std::string& str)
  {
    std::clog << "LOG " << name << "\t" << ident << "\t" << str << std::endl;
  }
};


struct init_info 
{
  std::shared_ptr<boost::asio::io_service> io_service;
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
  size_t input_buffer_size;
  std::function<void()> not_alive;
};

int main()
{
  auto reg = std::make_shared<wfc::registry< wfc::ilogger> >();
  auto log = std::make_shared<logger>();
  auto global = std::make_shared<wfc::global>() ;
  wfc::global::static_global = global;
  global->loggers = reg;
  reg->set("common", log);
  reg->set("trace", log);
  
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
  
  //_daemon_log = std::make_shared<logger>( lconf );
  
  
  {
    /*typedef fas::aspect< 
      wfc::io::strategy::posix::reader::sync_read,
      //wfc::io::basic::log::aspect, 
      wfc::io::reader::errors::log::aspect,
      wfc::io::reader::common::trace::aspect
      //wfc::io::reader::read::sync::aspect 
    > reader_aspect;*/
    typedef fas::type_list_n<
        wfc::io::strategy::posix::reader::trace,
        wfc::io::strategy::posix::reader::log,
        wfc::io::strategy::posix::reader::sync_read
    >::type sync_read_ws_log;
    typedef wfc::io::reader::reader< fas::aspect<sync_read_ws_log> > reader_type;
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
    d = reader.read();
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

