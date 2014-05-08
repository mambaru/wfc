#include <wfc/logger/ilogger.hpp>
#include <wfc/core/global.hpp>
#include <memory>

namespace {
class logger
  : public wfc::ilogger
{
public:
  virtual void initialize(const std::string& , std::stringstream& )
  {
    
  }
  
  virtual void write(const std::string& name, const std::string& ident,  const std::string& str)
  {
    std::stringstream ss;
    ss << "LOG " << name << "\t" << ident << "\t" << str << std::endl;
    
    std::cerr << ss.str();
  }
};

class test_logger
{
  wfc::io_service _io_service;
public:
  test_logger()
  {
    std::cout << "test_logger()" << std::endl;
    reg = std::make_shared<wfc::registry< wfc::ilogger> >();
    log = std::make_shared<logger>();
    global = std::make_shared<wfc::global>(_io_service) ;
    wfc::global::static_global = global;
    global->loggers = reg;
    reg->set("common", log);
    reg->set("trace", log);
  }
  
private:
  std::shared_ptr< wfc::registry< wfc::ilogger> > reg;
  std::shared_ptr< logger > log;
  std::shared_ptr< wfc::global > global;
};

static const std::shared_ptr<test_logger> test_log = std::make_shared<test_logger>();
  
}