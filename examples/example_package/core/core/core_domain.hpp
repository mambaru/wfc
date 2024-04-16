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

  virtual void core_abort( const std::string& ) override ;

  virtual void core_restart() override;

  virtual void set_status(wfc::core_status, const std::string&) override{}

  virtual void set_stage(wfc::core_stage) override{}

  virtual wfc::core_status get_status(wfc::core_stage*, std::vector<std::pair<wfc::core_status, std::string>>* ) override
  {
    return wfc::core_status::OK;
  }

  virtual std::string get_status_text(size_t, size_t) override
  {
    return "OK";
  }
};

