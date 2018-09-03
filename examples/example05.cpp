#include <wfc/wfc.hpp>
#include <wfc/module/module_list.hpp>
#include <wfc/module/component_list.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/name.hpp>
#include <wfc/domain_object.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/build_info.hpp>
#include <wfc/core/global.hpp>
#include <memory>

struct empty_conf{};

class example_core
  : public wfc::domain_object< wfc::icore, empty_conf >
{
  virtual int run( ) override
  {
    std::cout << "example_core::run()" << std::endl;
    return 0;
  }
  virtual void core_reconfigure() override{ }
  virtual void core_stop() override{}
  virtual void core_abort( std::string ) override {}
};


class example_startup
  : public wfc::domain_object< wfc::istartup, empty_conf >
{
public:
  virtual int startup(int argc, char* argv[], std::string helpstring) 
  {
    std::cout << "example_startup::startup: " << helpstring << std::endl;
    if ( argc > 1 )
    {
      if ( std::string(argv[1]) == std::string("run"))
        _ready = true;
      else
        return 1;
    }
    return 0;
  }
  
  virtual bool ready_for_run()
  {
    return _ready;
  }
  
private:
  bool _ready = false;
};


WFC_NAME2(module_name, "example-module")
WFC_NAME2(core_name, "core")
WFC_NAME2(startup_name, "startup")


struct empty_conf_json                                                                                                                                                          
{                                                                                                                                                                                 
  typedef wfc::json::object<
    empty_conf,
    wfc::json::member_list<
    >
  > type;                                                                                                                                                                         

  typedef typename type::target      target;                                                                                                                                      
  typedef typename type::member_list member_list;                                                                                                                                 
  typedef typename type::serializer  serializer;                                                                                                                                  
};

struct example_package: 
  wfc::module_list<
    wfc::empty_build_info,
    wfc::component_list<
      module_name,
      wfc::singleton<core_name, wfc::instance<example_core>, empty_conf_json>,
      wfc::singleton<startup_name, wfc::instance<example_startup>, empty_conf_json>
    >
  >
{
  virtual std::string name() const override
  {
    return "example_package";
  }
};

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({
    std::make_shared<example_package>()
  }).run(argc, argv);
}
