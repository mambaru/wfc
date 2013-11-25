#include <comet/comet.hpp>
#include <comet/core/icore.hpp>
#include <comet/core/imodule.hpp>
#include <comet/core/global.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace ::mamba::comet;

class core_moke: public icore
{
public:
  // icore
  void reconfigure()
  {
    
  }
  
  void run( int argc, char* argv[], std::weak_ptr<global> g )
  {
    if ( auto gl = g.lock() )
    {
      if ( auto gm = gl->modules.lock() )
      {
        gm->for_each([](const std::string& name, std::weak_ptr<imodule> module ){
          if ( auto m = module.lock() )
            m->start();
        });
      }
    }
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
  virtual std::string generate(const std::string& type) const { return std::string();}
  virtual bool parse_config(const std::string& conf) { return true;}

  virtual void create( std::weak_ptr<global> g )
  {
    std::cout << "create" << std::endl;
    _global = g;
    _core = std::make_shared<core_moke>();
    if ( auto gl = _global.lock() )
      gl->core = _core;
  }
  
  virtual void configure(const std::string& conf)
  {
    
  }
  
  virtual void initialize()
  {
    
  }
  
  virtual void start()
  {
    std::cout << "start" << std::endl;
  }
  
  virtual void stop()
  {
    
  }

  virtual void idle()
  {

  }
private:
  std::shared_ptr<core_moke> _core;
  std::weak_ptr<global> _global;
};

class module_moke: public imodule
{
public:
  // icore
  void reconfigure()
  {

  }

  void run( int argc, char* argv[], std::weak_ptr<global> g )
  {

  }

  virtual priority startup_priority() const { return priority::normal;}
  virtual priority shutdown_priority() const { return priority::normal;}

  virtual std::string version() const { return std::string();}
  virtual std::string description() const { return std::string();}
  virtual std::string generate(const std::string& type) const { return std::string();}
  virtual bool parse_config(const std::string& conf) { return true;}

  virtual void create( std::weak_ptr<global> g )
  {
    std::cout << "create" << std::endl;
  }

  virtual void configure(const std::string& conf)
  {

  }

  virtual void initialize()
  {

  }

  virtual void start()
  {
    std::cout << "start" << std::endl;
  }

  virtual void stop()
  {

  }

  virtual void idle()
  {

  }
};

int main(int argc, char* argv[])
{
  comet( "",  {
    {"core", std::make_shared<core_module_moke>()},
    {"module1", std::make_shared<module_moke>()},
    {"module2", std::make_shared<module_moke>()},
    {"module2", std::make_shared<module_moke>()}
  }).run(argc, argv);
  
  return 0;
}
