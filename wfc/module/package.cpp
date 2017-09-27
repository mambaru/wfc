
#include <wfc/module/package.hpp>
#include <wfc/core/ibuild_info.hpp>

namespace wfc{

package::~package()
{
  
}
  
package::package(std::shared_ptr<ipackage> p)
  : _impl(p)
{
  
}

std::shared_ptr<ibuild_info> package::build_info() 
{
  return _impl->build_info();
}

std::string package::name() const
{
  return _impl->build_info()->name();
}

std::string package::description() const
{
  return _impl->description();
}

int package::order() const
{
  return _impl->order();
}

std::vector< std::shared_ptr<imodule> > package::modules() 
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

void package::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  return _impl->reg_io( std::move(io_id), itf );
}

void package::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  return _impl->perform_io(std::move(d), std::move(io_id),  std::move(handler) );
}

void package::unreg_io(io_id_t io_id)
{
  return _impl->unreg_io(std::move(io_id));
}


}
