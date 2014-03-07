#include <iostream>

#include <wfc/io/posix/rn/writer.hpp>
#include <wfc/io/posix/rn/writer_impl.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

writer::~writer()
{
}

  
writer::writer(writer::descriptor_type&& desc, const writer::options_type& conf)
  : _impl( std::make_shared<writer_impl>(std::move(desc), conf) )
{
  
}

void writer::start()
{
  _impl->start();
}

void writer::stop()
{
  _impl->stop();
}

bool writer::status() const
{
  return _impl->status();
}
  
void writer::shutdown()
{
  
}

void writer::write(wfc::io::data_ptr d)
{
  return _impl->write( std::move(d) );
}

void writer::async_write(wfc::io::data_ptr d)
{
  _impl->async_write( std::move(d) );
}

}}}}
