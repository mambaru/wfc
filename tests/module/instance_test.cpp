#define IOW_DISABLE_ALL_LOG

#include <wfc/domain_object.hpp>
#include <wfc/module/instance.hpp>

struct options1
{
  bool test = false;
};

struct itest
  : public wfc::iinterface
{
  virtual ~itest(){}
  virtual bool testtest() = 0;
};

class test
  : public ::wfc::domain_object<itest, options1>
{
public:
 
  virtual void reconfigure()
  {
    testval = true;
  }
  
  virtual bool testtest() 
  {
    return testval; 
  }
  
  static void generate(options1& opt, const std::string&)
  {
    opt.test = true;
  }
  
  bool testval = false;
};

class test_impl: public ::wfc::instance<test> {};

int main()
{
  test_impl t;
  ::iow::asio::io_service io;
  auto g = std::make_shared<wfc::wfcglobal>(io);
  t.create(g);
  test_impl::options_type opt;
  t.generate(opt, "true");
  
  if ( !opt.test || !opt.enabled || !opt.name.empty() 
        || opt.startup_priority!=0 || opt.shutdown_priority!=0)
    return -1;

  opt.name = "test";
  t.configure(opt);
  auto tmp1 = g->registry.get<itest>("test");
  
  if ( tmp1 == nullptr )
    return -1;
  if ( tmp1->testtest() != false )
    return -2;
  t.initialize();
  if ( tmp1->testtest() != true )
    return -3;
  t.start("");
  t.configure(opt);
  t.initialize();
  t.stop("");
  /*
  if ( t.options().test != false )
    return -1;

  if ( !t.name().empty() )
    return -2;
  
  options opt;
  opt.test = true;
  
  t.start();
  t.initialize("name", nullptr, opt);

  if ( t.options().test != true )
    return -3;

  if ( t.name() != "name" )
    return -2;

  if ( t.testval != true )
    return -4;
  
  options opt2; 
  test::generate(opt2, "");
  if ( opt2.test != false )
    return -5;
  
  return 0;
  */
}
