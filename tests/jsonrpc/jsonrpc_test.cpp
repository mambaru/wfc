#define IOW_DISABLE_ALL_LOG

#include <wfc/domain_object.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/asio.hpp>
#include <wfc/name.hpp>

using namespace wfc;

struct options
{
  bool test = false;
  std::string target;
};

class test
  : public jsonrpc::domain_proxy<options, nostat>
{
public:
 
  virtual void reconfigure()
  {
    testval = true;
  }
  
  virtual int testtest() 
  {
    if ( !this->has_arg("param1") ) return 1001;
    if ( !this->has_arg("param2") ) return 1002;
    if ( !this->has_arg("param3") ) return 1003;
    if (  this->has_arg("param4") ) return 1004;
    if ( this->get_arg("param1")!="val1" ) return 1005;
    if ( this->get_arg("param2")!="100" ) return 1006;
    if ( this->get_arg("param3")!="" ) return 1007;
    if ( this->get_arg("param4")!="" ) return 1008;
    
    if ( this->get_arg_t<int>("param2")!=100 ) return 1007;
    if ( this->get_arg_t<int>("param1")!=0 ) return 1008;
    std::string err;
    if ( this->get_arg_t<int>("param1", &err)!=0 ) return 1008;
    if ( err!="Invalid Number" ) return 1009;
    
    return 0;
  }
  
  bool testval = false;
};

struct service_method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::target<iinterface>
>
{
};

WFC_NAME2(test_service_name, "test-service")

class test_service
  : public ::wfc::jsonrpc::service_multiton< test_service_name, service_method_list> 
{};
  
int main()
{
  wfc::asio::io_service ios;
  auto g = std::make_shared<wfc::wfcglobal>(ios);
  wfc::wfcglobal::static_global = g;
  wfc::instance<test> t;
    
  if ( t.object()!=nullptr )
    return 1;

  test::domain_config opt;
  opt.test = true;
  opt.name = "name";
  
  t.create("name", g);
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
  
  std::map<std::string, std::string> args;
  args["param1"]="val1";
  args["param2"]="100";
  args["param3"]="";
  g->args.insert("name", wfc::instance_args("name", args) );
  if ( auto itst = g->registry.get_target<ijsonrpc>("name") )
    return 0;
  else
    return 8;
}
