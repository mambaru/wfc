#include <wfc/domain_object.hpp>
#include <wfc/core/icore.hpp>
#include <iostream>
#include <memory>
#include "core_options.hpp"

class core_domain
  : public wfc::domain_object< wfc::icore, core_options >
{
public:
  virtual int run( ) override;
  
  virtual void core_reconfigure() override;
  
  virtual void core_stop() override;
  
  virtual void core_abort( std::string ) override ;
};

