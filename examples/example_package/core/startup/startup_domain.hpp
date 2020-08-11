#include <wfc/domain_object.hpp>
#include <wfc/core/istartup.hpp>
#include <iostream>
#include <memory>
#include "startup_options.hpp"

class startup_domain
  : public wfc::domain_object< wfc::istartup, startup_options >
{
public:
  virtual int startup(int argc, char* argv[], std::string helpstring) override;

  virtual bool ready_for_run() override;

private:
  bool _ready = false;

};

