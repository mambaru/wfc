#define IOW_DISABLE_ALL_LOG

#include <wfc/module/domain_object.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/object_list.hpp>
#include <wfc/module/module_list.hpp>

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
    public ::wfc::iinterface
  {
    ~itest(){}
    virtual void doit() = 0;
  };
  
  
  struct options_json
  {
    JSON_NAME(test)
    JSON_NAME(value)
    typedef ::iow::json::object<
      options,
      typename fas::type_list_n<
        //::iow::json::member<n_value, options, std::string, &options::value>,
        ::iow::json::member<n_test, options, int, &options::test>
      >::type
    > type;

    typedef typename type::member_list member_list;
    typedef typename type::target target;

  };

  class test:
    public ::wfc::domain_object<itest, options>
  {
  public:
    int doit_count = 0;
    virtual void doit() 
    {
      ++doit_count;
    }
  };
  
  class object: public ::wfc::basic_object< Name, ::wfc::instance<test>, options_json, Singleton > {};
};


JSON_NAME(package1)
JSON_NAME(module1)
JSON_NAME(object1)
JSON_NAME(object2)
JSON_NAME(object3)
JSON_NAME(object4)

class test_module: public ::wfc::object_list<
    n_module1,
    helper<n_object1, true>::object,
    helper<n_object2, true>::object,
    helper<n_object3, false>::object,
    helper<n_object4, false>::object
  >
{
};

class test_package: public ::wfc::module_list< n_package1, test_module> {};

int main()
{
  test_package t;
  iow::asio::io_service io;
  auto g = std::make_shared< wfc::wfcglobal>(io);
  t.create(g);
  std::cout << g->registry.size() << std::endl;
}
