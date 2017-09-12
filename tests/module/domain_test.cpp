#define IOW_DISABLE_ALL_LOG

#include <wfc/domain_object.hpp>

using namespace wfc;

struct options
{
  bool test = false;
};

struct itest: iinterface
{
  virtual ~itest(){}
  virtual void testtest() = 0;
};

class test
  : public domain_object<itest, options>
{
public:
 
  virtual void reconfigure()
  {
    testval = true;
  }
  
  virtual void testtest() 
  {
    
  }
  
  bool testval = false;
};

int main()
{
  test t;
  
  if ( t.options().test != false )
    return 1;

  
  if ( !t.name().empty() )
    return 2;
  
  test::config_type opt;
  opt.test = true;
  opt.name = "name";
  
  t.create_domain("name", nullptr);
  t.start();
  t.configure_domain(opt);
  t.initialize();

  if ( t.options().test != true )
    return 3;

  //std::cout << t.name() << std::endl;
  if ( t.name() != "name" )
    return 5;

  if ( t.testval != true )
    return 6;
  
  options opt2; 
  opt2 = t.generate("");
  if ( opt2.test != false )
    return 7;
  
  return 0;
}
