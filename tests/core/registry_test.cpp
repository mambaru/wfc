#include <comet/core/registry.hpp>
#include <vector>
#include <algorithm>

using namespace ::mamba::comet;

int main()
{
  
  registry<int> reg;
  std::shared_ptr<int> i0 = std::make_shared<int>(0);
  std::shared_ptr<int> i1 = std::make_shared<int>(1);
  std::shared_ptr<int> i2 = std::make_shared<int>(2);
  std::shared_ptr<int> i3 = std::make_shared<int>(3);
  reg.insert("i0", i0);
  reg.insert("i1", i1);
  reg.insert("i2", i2);
  reg.insert("i3", i3);

  reg.erase("i0");
  std::vector<int> v;
  reg.for_each([&v](const std::string&, std::weak_ptr<int> p){
    if ( auto i = p.lock() )
      v.push_back(*i);
  });

  std::sort(v.begin(), v.end(), std::greater<int>());

  if (v.size()!=3) return -1;

  if (v[0]!=3) return -1;
  if (v[1]!=2) return -1;
  if (v[2]!=1) return -1;

  if ( *(reg.find("i2").lock()) != 2 )
    return -1;

  if ( auto x = reg.find("i4").lock() )
    return -1;
  
  return 0;
}
