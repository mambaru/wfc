#define IOW_DISABLE_LOG
#include <iow/logger.hpp>
#include <wfc/domain_object.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/package.hpp>
#include <wfc/module/component_list.hpp>
#include <wfc/module/module_list.hpp>
#include <wfc/json.hpp>

template<typename Name, bool Singleton>
struct helper
{
  struct options
  {
    std::string value;
    int test = 33;
    
    options()
      : value( Name()() )
      {}
  };
  
  struct itest: 
    public wfc::iinterface
  {
    ~itest(){}
    virtual void doit() = 0;
  };
  
  
  struct options_json
  {
    JSON_NAME(test)
    JSON_NAME(value)
    typedef wfc::json::object<
      options,
      typename fas::type_list_n<
        wfc::json::member<n_test, options, int, &options::test>
      >::type
    > type;

    typedef typename type::member_list member_list;
    typedef typename type::target target;
    typedef typename type::serializer serializer;

  };

  class test:
    public wfc::domain_object<itest, options>
  {
  public:
    int doit_count = 0;
    virtual void doit()  override
    {
      ++doit_count;
    }
    
    test() {  this->doit();  } //cppcheck
  };
  
  class object: public ::wfc::basic_component< Name, wfc::instance<test>, options_json, Singleton, wfc::nostat_json > {};
};


JSON_NAME(package1)
JSON_NAME(module1)
JSON_NAME(object1)
JSON_NAME(object2)
JSON_NAME(object3)
JSON_NAME(object4)

class test_module: public ::wfc::component_list<
    n_module1,
    helper<n_object1, true>::object,
    helper<n_object2, true>::object,
    helper<n_object3, false>::object,
    helper<n_object4, false>::object
  >
{
};

class test_package: public wfc::module_list< wfc::build_info< wfc::empty_build_info>, test_module> {};

int main()
{
  auto t = std::make_shared<wfc::package>( std::make_shared<test_package>() ) ;
  iow::asio::io_service io;
  auto g = std::make_shared< wfc::wfcglobal>(io);
  t->create(g);
  std::cout << g->registry.size() << std::endl;
  t->stop("");
  t.reset();
  std::cout << "g.use_cout(): " << g.use_count() << std::endl;
  g->clear();
  std::cout << "g.use_cout(): " << g.use_count() << std::endl;
  g.reset();
}
