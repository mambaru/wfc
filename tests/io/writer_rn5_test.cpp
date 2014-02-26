#include <iostream>


#include <wfc/io/writer/writer.hpp>
#include <wfc/io/strategy/posix/writer/config.hpp>
#include <wfc/io/strategy/posix/writer/rn/async_loop.hpp>
#include <wfc/io/strategy/posix/writer/trace.hpp>
#include <wfc/io/strategy/posix/writer/log.hpp>
#include <boost/asio.hpp>
#include <string>


namespace ts =  ::wfc::io::writer::stream;
  
size_t global_bytes_transferred = 0;
struct ad_async_write_some
{
  std::vector<char> buff;
  
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    buff.assign(d->begin(), d->begin()+1);
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    t.descriptor().async_write_some
    (
      ::boost::asio::buffer( buff ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
        { 
          global_bytes_transferred+=bytes_transferred;
          std::cout << "test async [" << std::string( (*dd)->begin(), (*dd)->begin() + bytes_transferred ) <<  "]"
                    << "[" << std::string( (*dd)->begin(), (*dd)->end() ) <<  "]"<< std::endl;
          t.get_aspect().template get< ts::async::_outgoing_>()(t, std::move(*dd), ec, bytes_transferred);
        }
      )
    );
  }
};

struct ad_write_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::vector<char> buff(d->begin(), d->begin()+1);
    
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.descriptor().write_some( ::boost::asio::buffer( buff ), ec);
    global_bytes_transferred+=bytes_transferred;
    std::cout << "test sync [" << std::string( d->begin(), d->begin() + bytes_transferred ) <<  "]"
              << "[" << std::string( d->begin(), d->end() ) <<  "]"<< std::endl;
    t.get_aspect().template get< ts::sync::_outgoing_ >()(t, std::move(d), ec, bytes_transferred);
  }
};



typedef wfc::io::strategy::posix::writer::config init_info;


struct write_aspect: 
  fas::aspect<
    fas::advice< ts::sync::_incoming_, ad_write_some>,
    fas::advice< ts::async::_incoming_, ad_async_write_some>,
    wfc::io::strategy::posix::writer::rn::async_loop,
    wfc::io::strategy::posix::writer::trace,
    wfc::io::strategy::posix::writer::log
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
    init.output_buffer_size = 4;
    boost::asio::posix::stream_descriptor sd(*io_service, dd[1]);
    writer_type writer( std::move(sd), init);
    handler = writer.wrap([&](){ ++handler_count;});
    size_t size = writer.write_string( "0123456789" );
    if ( size != 1 )
      abort();
    size = writer.write_string( "0123456789" );
    if ( size != 0 )
      abort();

    while (global_bytes_transferred!=20 + 4)
    {
      std::cout << "-" << global_bytes_transferred << "-" << std::endl;
      io_service->run_one();
    }
    
    init.output_buffer_size = 1;
    writer.reconfigure(init);
    size = writer.write_string( "0123456789" );
    if ( size != 24 )
      abort();
    size = writer.write_string( "0123456789" );
    if ( size != 0 )
      abort();
    //for (int i=0; i < 5+5 + 5 + 5; ++i)
    while (global_bytes_transferred!=40 + 8)
    {
      std::cout << "-" << global_bytes_transferred << "-" << std::endl;
      io_service->run_one();
    }

    char buffer[100];
    size = read( dd[0], buffer, 100);
    buffer[size]=0;
    std::string result = buffer;

    std::cout << result << std::endl;
    
    if ( result != "0123456789\r\n0123456789\r\n0123456789\r\n0123456789\r\n" )
      abort();


    close(dd[1]);
    size = writer.write_string( "0123456789" );

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

