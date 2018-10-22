#include <wfc/module/winterface.hpp>

namespace wfc{ 

winterface::winterface(std::weak_ptr<iinterface> target, bool suspend)
  : _target(target)
  , _suspend(suspend)
{}

void winterface::unreg_io(io_id_t id)
{
  auto target = _target.lock();
  if ( !_suspend && target!=nullptr )
  {
    target->unreg_io(id);
  }
}

void winterface::reg_io(io_id_t id, std::weak_ptr<iinterface> itf)
{
  auto target = _target.lock();
  if ( !_suspend && target!=nullptr )
  {
    target->reg_io(id, itf);
  }
}
  
void winterface::perform_io(data_ptr d, io_id_t id, output_handler_t cb)
{
  auto target = _target.lock();
  if ( !_suspend && target!=nullptr )
  {
    target->perform_io(std::move(d), id, cb);
  }
  else if (_suspend)
  {
    cb(std::move(d));
  }
  else
  {
    cb(nullptr);
  }
}
  
void winterface::set_target(std::weak_ptr<iinterface> target)
{
  _target = target;
}
  
void winterface::set_suspend( bool suspend)
{
  _suspend = suspend;
}

}

