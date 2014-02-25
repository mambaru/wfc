#include <iostream>


#include <wfc/io/writer/writer.hpp>
#include <wfc/io/strategy/posix/writer/config.hpp>
#include <wfc/io/strategy/posix/writer/async_loop.hpp>
#include <boost/asio.hpp>
#include <string>



typedef wfc::io::strategy::posix::writer::config init_info;


struct write_aspect: 
  fas::aspect<
    wfc::io::strategy::posix::writer::async_loop
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
    // typedef writer_type::data_type data_type;
    handler = writer.wrap([&](){ ++handler_count;});
    size_t size = writer.write_string( "test1" );
    if ( size != 0 )
      abort();
    size = writer.write_string( "test2" );
    if ( size != 0 )
      abort();
    //std::cout << "-1-" << std::endl;
    //io_service->run_one();
    //std::cout << "-2-" << std::endl;
    
    
    /*if ( result != "test1test2" )
      abort();*/
    
    init.output_buffer_size = 1;
    writer.reconfigure(init);
    size = writer.write_string( "test1" );
    if ( size != 0 )
      abort();
    size = writer.write_string( "test2" );
    if ( size != 0 )
      abort();
    for (int i=0; i < 5+5 + 5 + 5; ++i)
    {
      std::cout << "-" << i << "-" << std::endl;
      io_service->run_one();
    }

    char buffer[100];
    size = read( dd[0], buffer, 100);
    buffer[size]=0;
    std::string result = buffer;

    std::cout << result << std::endl;
    
    if ( result != "test1test2test1test2" )
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
    
    reader.get_io_service().post( handler );
    io_service->run_one();
    close(dd[0]);
    d = reader.read();
    reader.get_io_service().post( handler );
    */
  }
  
  // not posted last handler
  //io_service->run_one();
  // execute not alive
  /*handler();
  
  if ( handler_count!=1 || not_alive_count!=2)
  {
    abort();
  }
  */
}

