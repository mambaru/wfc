
#include <wfc/module/package.hpp>

namespace wfc{

package::~package()
{
  
}
  
package::package(std::shared_ptr<ipackage> p)
  : _impl(p)
{
  
}

std::string package::build_info() const
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

}
