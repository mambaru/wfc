#include <comet/logger/logger.hpp>
#include <iostream>

using namespace ::mamba::comet;

class logger1:public ilogger
{
public:
  virtual ~logger1(){}
  virtual void initialize(const std::string& name, std::stringstream& str) 
  {
    std::cout << "initialize" << std::endl; 
  };
  virtual void write(const std::string& name, const std::string& ident, const std::string& s) 
  {
    std::cout << "writes" << std::endl; 
    str=name + ident + s;
  }
  std::string str;
};

int main()
{
  logger1 test_logger;
  typedef registry<ilogger> test_registry_type;
  std::shared_ptr<test_registry_type> test_registry = std::make_shared<test_registry_type>();
  test_registry->insert("1", std::make_shared<logger1>(test_logger));
  global_log::instance().set_registry(test_registry);
  logger("1", "-") <<  "test" << 1;
  std::cout <<  test_logger.str <<  std::endl;
  if ( test_logger.str != "1-test1")
    return -1;
  return 0;
}
