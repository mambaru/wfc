#include <wfc/wfc.hpp>
#include <wfc/json/json.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/imodule.hpp>
#include <wfc/core/global.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace ::wfc;

class core_moke: public icore
{
public:
  // icore
  void reconfigure()
  {
    
  }
  
  int run( /*int argc, char* argv[],*/ std::shared_ptr<global> g )
  {
    if ( auto gl = g )
    {
      //if ( auto& gm = gl->registry )
      {
        gl->registry.for_each<imodule>([](const std::string& /*prefix*/, const std::string& /*name*/, std::shared_ptr<imodule> module ){
          if ( auto m = module )
            m->start("");
        });
      }
    }
    return 0;
  }

  void stop()
  {
    
  }
};

class core_module_moke: public imodule
{
public:
  /// imodule

  virtual priority startup_priority() const { return priority::core;}
  virtual priority shutdown_priority() const { return priority::core;}

  virtual std::string version() const { return std::string();}
  virtual std::string description() const { return std::string();}
  virtual std::string generate(const std::string& /*type*/) const { return std::string();}
  virtual bool parse_config(const std::string& /*conf*/) { return true;}

  virtual void create( const std::string&,  std::shared_ptr<global> g )
  {
    std::cout << "create" << std::endl;
    _global = g;
    _core = std::make_shared<core_moke>();
     _global->registry.set("core", _core);
    //if ( auto gl = _global.lock() )
    //  gl->core = _core;
  }
  
  virtual void configure(const std::string& /*conf*/, const std::string& /*arg*/)
  {
    
  }
  
  virtual void initialize(const std::string& /*arg*/)
  {
    
  }
  
  virtual void start(const std::string& /*arg*/)
  {
    std::cout << "start" << std::endl;
  }
  
  virtual void stop(const std::string& /*arg*/)
  {
    
  }
  
  virtual void shutdown(const std::string& /*arg*/)
  {

  }


  virtual void idle()
  {

  }
private:
  std::shared_ptr<core_moke> _core;
  std::shared_ptr<global> _global;
};

class module_moke: public imodule
{
public:
  // icore
  void reconfigure()
  {

  }

  void run( int /*argc*/, char* /*argv*/[], std::weak_ptr<global> /*g*/ )
  {

  }

  virtual priority startup_priority() const { return priority::normal;}
  virtual priority shutdown_priority() const { return priority::normal;}

  virtual std::string version() const { return std::string();}
  virtual std::string description() const { return std::string();}
  virtual std::string generate(const std::string& /*type*/) const { return std::string();}
  virtual bool parse_config(const std::string& /*conf*/) { return true;}

  virtual void create( const std::string&,  std::shared_ptr<global> /*g*/ )
  {
    std::cout << "create" << std::endl;
  }

  virtual void configure(const std::string& /*conf*/, const std::string& /*arg*/)
  {

  }

  virtual void initialize(const std::string& /*arg*/)
  {

  }

  virtual void start(const std::string& /*arg*/)
  {
    std::cout << "start" << std::endl;
  }

  virtual void stop(const std::string& /*arg*/)
  {

  }

  virtual void shutdown(const std::string& /*arg*/)
  {

  }

  virtual void idle()
  {

  }
};

int main(int argc, char* argv[])
{
  
  wfc::wfc( "",  {
    {"core", std::make_shared<core_module_moke>()},
    {"module1", std::make_shared<module_moke>()},
    {"module2", std::make_shared<module_moke>()},
    {"module2", std::make_shared<module_moke>()}
  }).run(argc, argv);
  
  return 0;
}
