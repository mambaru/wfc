#include <iostream>


#include <wfc/io/writer/writer.hpp>
#include <wfc/io/strategy/posix/writer/config.hpp>
#include <wfc/io/strategy/posix/writer/sync_write.hpp>

#include <wfc/memory.hpp>
/*
#include <wfc/io/strategy/posix/reader/sync_read_return.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/config.hpp>
*/
#include <boost/asio.hpp>
#include <string>



typedef wfc::io::strategy::posix::writer::config init_info;


struct write_aspect: 
  fas::aspect<
    wfc::io::strategy::posix::writer::sync_write
    /*
    wfc::io::strategy::posix::reader::trace,
    wfc::io::strategy::posix::reader::log,
    
    */
  >
{};

    

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
  init.output_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count;};
  
  {
    
    
    typedef wfc::io::writer::writer< write_aspect > writer_type;
    
    boost::asio::posix::stream_descriptor sd(*io_service, dd[1]);
    writer_type writer( std::move(sd), init);
    //typedef writer_type::data_type data_type;
    handler = writer.wrap([&](){ ++handler_count;});
    std::string data = "test1";
    size_t size = writer.write_string( "test1" );
    if ( size != 5 )
      abort();
    
    char buffer[100];
    size = read( dd[0], buffer, 100);
    buffer[size]=0;
    std::string result = buffer;
    if ( result != "test1" )
      abort();
    
    
    /*
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
    */
    writer.post( handler );
    io_service->run_one();
    close(dd[0]);
    close(dd[1]);
    //d = reader.read();
    writer.post( handler );
    
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

