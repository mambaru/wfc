#include <iostream>

#include <wfc/io/reader/reader.hpp>

#include <string>
#include <boost/asio.hpp>


std::vector<std::string> result_list;

struct _handler_;
struct ad_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "handle " << std::string(d->begin(), d->end() ) << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
    t.get_aspect().template get< wfc::io::reader::_output_ >()( t, std::move(d));
  }
};

struct reader_aspect: fas::aspect<
  fas::advice< wfc::io::reader::_incoming_, ad_handler>,
  wfc::io::reader::stream< boost::asio::posix::stream_descriptor >,
  wfc::io::reader::error_log

> {};

int async_count = 0;

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
    reader.async_read([](wfc::io::data_ptr d) { async_count++; if ( std::string(d->begin(), d->end())!="test1") abort(); return wfc::callback_status::ready;});
    write(dd[1], "test1", 5);
    io_service->run_one();
    reader.async_read([](wfc::io::data_ptr d) { async_count++; if ( std::string(d->begin(), d->end())!="test2") abort(); return wfc::callback_status::ready;});
    write(dd[1], "test2", 5);
    io_service->run_one();
    reader.async_read([](wfc::io::data_ptr d) { async_count++; if ( std::string(d->begin(), d->end())!="test3") abort(); return wfc::callback_status::ready;});
  }
  
  if ( async_count!=2 )
    abort();
  
  if ( result_list.size()!=2 )
    abort();
  if ( result_list[0]!="test1" )
    abort();
  if ( result_list[1]!="test2" )
    abort();
}
