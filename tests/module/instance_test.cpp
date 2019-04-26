#define IOW_DISABLE_LOG
#include <iow/logger.hpp>
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
  : public wfc::domain_object<itest, options1>
{
public:
  typedef domain_object::domain_config config_type;
  virtual void initialize() override
  {
    testval = true;
  }
  
  virtual bool testtest() override
  {
    return testval; 
  }
  
  virtual config_type generate(const std::string&) override
  {
    config_type opt;
    opt.test = true;
    return opt;
  }
  
  // restart
  virtual void restart() override
  {
    std::cout << "RESTART!!!!!!" << std::endl;
  }

  // start
  virtual void start() override
  {
    std::cout << "START!!!!!!" << std::endl;
  }
  bool testval = false;
};

//class test_impl: public ::wfc::instance<test> {};
typedef ::wfc::instance<test> test_impl;

int main()
{
  test_impl t;
  ::iow::asio::io_service io;
  auto g = std::make_shared<wfc::wfcglobal>(io);
  t.create(g);
  test_impl::domain_config opt;
  t.generate(opt, "true");
  
  if ( !opt.test || !opt.enabled || !opt.name.empty() 
        || opt.startup_priority!=0 || opt.shutdown_priority!=0)
    return 1;

  opt.name = "test";
  t.configure(opt);
  auto tmp1 = g->registry.get_target<itest>("test");
  
  if ( tmp1 == nullptr )
    return 2;
  if ( tmp1->testtest() != false )
    return 3;
  t.initialize();
  if ( tmp1->testtest() != true )
    return 4;
  t.start("");
  t.configure(opt);
  t.initialize();
  t.stop("");
  
  return 0;
}
