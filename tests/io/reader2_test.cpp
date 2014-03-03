#include <iostream>

/*
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/sync_handler.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/config.hpp>
*/

#include <wfc/io/reader/reader.hpp>


#include <string>
#include <boost/asio.hpp>

//typedef wfc::io::strategy::posix::reader::config init_info;

std::vector<std::string> result_list;

struct _handler_;
struct ad_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "handle [" << std::string(d->begin(), d->end() ) << "]" << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
    t.get_aspect().template get< wfc::io::reader::_output_ >()( t, std::move(d));
  }
};

struct reader_aspect: fas::aspect<
  fas::advice< wfc::io::reader::_incoming_, ad_handler>,
  wfc::io::reader::stream< boost::asio::posix::stream_descriptor >,
  wfc::io::reader::error_log
  /*
  fas::advice< wfc::io::_options_type_, init_info>,
  fas::advice< wfc::io::reader::_outgoing_, ad_handler>,
  wfc::io::strategy::posix::reader::sync_handler,
  wfc::io::strategy::posix::reader::log,
  wfc::io::strategy::posix::reader::trace
  */
> {};

int main()
{
  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  wfc::io::reader::options init;
  init.input_buffer_size = 8096;
  init.not_alive = nullptr;
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;

    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    std::cout << "-1-" << std::endl;
    write(dd[1], "test1", 5);
    reader.read();
    std::cout << "-2-" << std::endl;
    write(dd[1], "test2", 5);
    reader.read();
    std::cout << "-3-" << std::endl;
  }
  
  
  if ( result_list.size()!=2 )
    abort();
  if ( result_list[0]!="test1" )
    abort();
  if ( result_list[1]!="test2" )
    abort();
}
