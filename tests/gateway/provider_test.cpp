#define WFC_DISABLE_GLOBAL_LOG

#include <wfc/gateway/provider/provider.hpp>
#include <wfc/memory.hpp>

namespace request{
  struct test{};
}

namespace response{
  struct test{};
}


struct test
{
  typedef std::function<void(std::unique_ptr<response::test>)> callback1;
  typedef std::function<void(std::unique_ptr<response::test>, int, std::string)> callback2;

  void method1(
    std::unique_ptr<request::test>, 
    callback1 callback
  )
  {
    callback(std::make_unique<response::test>());
  }
  
  void method2(
    std::unique_ptr<request::test>, 
    callback2 callback
  )
  {
    callback(std::make_unique<response::test>(), 0, "");
  }
};


int main()
{
  typedef ::wfc::gateway::provider<test> provider_type;
  typedef ::wfc::gateway::provider_config provider_config;
  provider_config conf;
  conf.mode = wfc::gateway::provider_mode::sequenced;
  provider_type privider(conf);
  int callback1_count = 0;
  int callback2_count = 0;

  test::callback1 c1 = [&callback1_count](std::unique_ptr<response::test>)
  {
    ++callback1_count;
  };

  test::callback2 c2 = [&callback2_count](std::unique_ptr<response::test>, int p, std::string s)
  {
    if (p!=42)
      abort();
    
    if (s!="42")
      abort();
    
    ++callback2_count;
  };
  auto t = std::make_shared<test>();
  privider.startup(1, t);
  privider.post(&test::method1, std::make_unique<request::test>(), c1);  
  privider.post(&test::method2, std::make_unique<request::test>(), c2);
  
  return 0;
}
