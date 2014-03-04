

#include <wfc/io/posix/rn/reader.hpp>
#include <wfc/io/posix/rn/reader_impl.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

reader::~reader()
{
}

  
reader::reader(reader::descriptor_type&& desc, const reader::options_type& conf)
  : _impl( std::make_unique<reader_impl>(std::move(desc), conf) )
{
  
}

void reader::start()
{
  _impl->start();
}

void reader::stop()
{
  _impl->stop();
}
  
void reader::shutdown()
{
  
}

wfc::io::data_ptr reader::read()
{
  return _impl->read();
}

void reader::async_read(wfc::io::callback handler)
{
  _impl->async_read(handler);
}

}}}}
