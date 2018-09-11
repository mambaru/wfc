#define IOW_DISABLE_ALL_LOG

#include <wfc/domain_object.hpp>
#include <wfc/module/instance.hpp>

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
  
  wfc::instance<test> t;
    
  if ( t.object()!=nullptr )
    return 1;

  test::config_type opt;
  opt.test = true;
  opt.name = "name";
  
  t.create("name", nullptr);
  t.start("");
  t.configure(opt);
  t.initialize();
  t.reconfigure(opt);

  if ( t.object()->options().test != true )
    return 3;

  //std::cout << t.name() << std::endl;
  if ( t.object()->name() != "name" )
    return 5;

  if ( t.object()->testval != true )
    return 6;
  
  options opt2; 
  opt2 = t.object()->generate("");
  if ( opt2.test != false )
    return 7;
  
  return 0;
}
