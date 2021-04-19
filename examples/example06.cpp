#include <wfc/wfc.hpp>
#include <wfc/module/module_list.hpp>
#include <wfc/module/component_list.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/package.hpp>
#include <wfc/module/module.hpp>
#include <wfc/module/component.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/name.hpp>
#include <wfc/domain_object.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/build_info.hpp>

#include <memory>
#include <iostream>

struct core_options{};
struct core_options_json
{
  typedef wfc::json::object<
    core_options,
    wfc::json::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

struct startup_options{};
struct startup_options_json
{
  typedef wfc::json::object<
    startup_options,
    wfc::json::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};


class example_core
  : public wfc::domain_object< wfc::icore, core_options >
{
public:
  typedef wfc::domain_object< wfc::icore, core_options >::domain_config config_type;

  virtual int run( ) override
  {
    std::cout << "example_core::run()!" << std::endl;
    return 0;
  }
  virtual void core_reconfigure() override{ }
  virtual void core_stop() override{}
  virtual void core_abort( const std::string& ) override {}
  virtual void core_restart() override{}
};


class example_startup
  : public wfc::domain_object< wfc::istartup, startup_options >
{
public:
  virtual int startup(int argc, char* argv[], std::string helpstring) override
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

  virtual bool ready_for_run() override
  {
    return _ready;
  }

private:
  bool _ready = false;
};

class core_singleton: public wfc::component
{
public:
  core_singleton();
};

WFC_NAME2(core_name, "core")
struct core_impl: wfc::singleton<core_name, wfc::instance<example_core>, core_options_json>{};

core_singleton::core_singleton()
  : wfc::component(std::make_shared<core_impl>())
{}

class startup_singleton: public wfc::component
{
public:
  startup_singleton();
};


WFC_NAME2(startup_name, "startup")
struct startup_impl: wfc::singleton<startup_name, wfc::instance<example_startup>, startup_options_json>{};

startup_singleton::startup_singleton()
  : wfc::component(std::make_shared<startup_impl>())
{}


class example_module: public wfc::module
{
  struct impl;
public:
  example_module();
};

WFC_NAME2(module_name, "example-module")

struct example_module::impl:
  wfc::component_list<
    module_name,
    core_singleton,
    startup_singleton
  >
{};

example_module::example_module()
  : wfc::module(std::make_shared<impl>())
{

}


class example_package: public wfc::package
{
  struct impl;
public:
  example_package();
};

struct example_package::impl:
  wfc::module_list<
    wfc::empty_build_info,
    example_module
  >
{
  virtual std::string name() const override
  {
    return "example_package";
  }
};

example_package::example_package()
  : wfc::package(std::make_shared<impl>())
{

}

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({
    std::make_shared<example_package>()
  }).run(argc, argv, "example6");
}
