#define IOW_DISABLE_ALL_LOG

#include <wfc/domain_object.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/multiton.hpp>

struct itest: 
  public wfc::iinterface
{
  ~itest(){}
};

struct options
{
  int test = 33;
};

struct options_json
{
  JSON_NAME(test)
  typedef wfc::json::object<
    options,
    fas::type_list_n<
      wfc::json::member<n_test, options, int, &options::test>
    >::type
  > type;

  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
  typedef typename type::target target;
};

class test:
  public ::wfc::domain_object<itest, options>
{
  
};

JSON_NAME2(name_test, "test")

class test_singleton
  : public ::wfc::singleton< name_test, ::wfc::instance<test>, options_json >
{};


class test_multiton
  : public ::wfc::multiton< name_test, ::wfc::instance<test>, options_json >
{};

template<typename T>
void test_gen()
{
  T t;
  t.create(nullptr);
  std::cout << "[" << t.interface_name() << "]" << std::endl;
  std::string genstr = t.generate("");
  std::cout << genstr << std::endl;
  t.configure(genstr, nullptr);
}

int main()
{
  test_gen<test_singleton>();
  test_gen<test_multiton>();
}
