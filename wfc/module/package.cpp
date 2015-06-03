
#include <wfc/module/package.hpp>

namespace wfc{

package::~package()
{
  
}
  
package::package(std::shared_ptr<ipackage> p)
  : _impl(p)
{
  
}

std::shared_ptr<ibuild_info> package::build_info() const
{
  return _impl->build_info();
}

std::string package::description() const
{
  return _impl->description();
}

std::vector< std::shared_ptr<imodule> > package::modules() const
{
  return _impl->modules();
}

void package::create( std::shared_ptr<wfcglobal> g)
{
  return _impl->create(g);
}

void package::start(const std::string& arg)
{
  return _impl->start(arg);
}

void package::stop(const std::string& arg)
{
  return _impl->stop(arg);
}

void package::startup_io(io_id_t io_id, outgoing_handler_t handler)
{
  return _impl->startup_io( std::move(io_id), std::move(handler) );
}

void package::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
{
  return _impl->perform_io(std::move(d), std::move(io_id),  std::move(handler) );
}

void package::shutdown_io(io_id_t io_id)
{
  return _impl->shutdown_io(std::move(io_id));
}


}
