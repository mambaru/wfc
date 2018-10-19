//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/module/module.hpp>

namespace wfc{

module::~module()
{
  
}
  
module::module(const std::shared_ptr<imodule>& p)
  : _impl(p)
{
  
}

std::string module::name() const
{
  return _impl->name();
}

std::string module::description() const
{
  return _impl->description();
}

std::vector< std::shared_ptr<icomponent> > module::components() const
{
  return _impl->components();
}

void module::create( std::shared_ptr<wfcglobal> g)
{
  return _impl->create(g);
}

void module::start(const std::string& arg)
{
  return _impl->start(arg);
}

void module::stop(const std::string& arg)
{
  return _impl->stop(arg);
}

void module::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  return _impl->reg_io( std::move(io_id), itf );
}

void module::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  return _impl->perform_io(std::move(d), std::move(io_id),  std::move(handler) );
}

void module::unreg_io(io_id_t io_id)
{
  return _impl->unreg_io(std::move(io_id));
}


}
