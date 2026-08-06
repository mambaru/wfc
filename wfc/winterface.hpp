#pragma once 
#include <wfc/iinterface.hpp>
#include <memory>
#include <atomic>

namespace wfc{
  
class winterface
  : public iinterface
{
  typedef iinterface::io_id_t io_id_t;
  typedef iinterface::output_handler_t output_handler_t;
  typedef iinterface::data_ptr data_ptr;

public:
  
  winterface(std::weak_ptr<iinterface> target, bool suspend);

  winterface(const winterface&) = delete;

  virtual void unreg_io(io_id_t id) override;

  virtual void reg_io(io_id_t id, std::weak_ptr<iinterface> itf) override;
  
  virtual void perform_io(data_ptr d, io_id_t id, output_handler_t cb) override;
  
  void set_target(std::weak_ptr<iinterface> target);
  
  void set_suspend( bool suspend);
  
private:
  std::weak_ptr<iinterface> _target;
  std::atomic<bool> _suspend;
};

}
