#define IOW_DISABLE_ALL_LOG

#include <wfc/domain_object.hpp>

struct options
{
  bool test = false;
};

struct itest
{
  virtual ~itest(){}
  virtual void testtest() = 0;
};

class test
  : public ::wfc::domain_object<itest, options>
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
    return -1;

  if ( !t.name().empty() )
    return -2;
  
  options opt;
  opt.test = true;
  
  t.start("");
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
}
