#include <iostream>

#include <wfc/io/posix/rn/reader.hpp>
#include <wfc/io/posix/rn/reader_impl.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

reader::~reader()
{
}

  
reader::reader(reader::descriptor_type&& desc, const reader::options_type& conf/*, wfc::io::incoming_handler handler*/ )
  : _impl( std::make_shared<reader_impl>(std::move(desc), conf/*, handler*/) )
{
  
}

void reader::start()
{
  _impl->start();
}

void reader::stop(std::function<void()> finalize)
{
  _impl->stop(finalize);
}

bool reader::status() const
{
  return _impl->status();
}
  
void reader::shutdown()
{
  
}

wfc::io::data_ptr reader::read()
{
  return _impl->read();
}

void reader::async_read(wfc::io::outgoing_handler_t handler)
{
  _impl->async_read(handler);
}

}}}}
