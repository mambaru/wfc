
#define IOW_DISABLE_LOG
#include <iow/logger.hpp>
#include <wfc/core/object_registry.hpp>
#include <vector>
#include <algorithm>

using namespace ::wfc;

struct number: iinterface {
  int count = 0;
  virtual ~number() = default;
  number() = default;
  explicit number(int c) noexcept: count(c) {}

  number(const number& n)
    : iinterface() , count(n.count)
  {}

  number& operator=(const number& n)
  {
    count = n.count;
    return *this;
  }
};

struct number_greater
{
  bool operator () (const number& left, const number& right) const
  {
    return left.count > right.count;
  }
};

int main()
{
  object_registry reg;
  std::shared_ptr<number> i0 = std::make_shared<number>(0);
  std::shared_ptr<number> i1 = std::make_shared<number>(1);
  std::shared_ptr<number> i2 = std::make_shared<number>(2);
  std::shared_ptr<number> i3 = std::make_shared<number>(3);
  reg.set_target("i0", i0);
  reg.set_target("i1", i1);
  reg.set_target("i2", i2);
  reg.set_target("i3", i3);

  reg.erase("i0");
  std::vector<number> v;
  reg.for_each<number>([&v](const std::string&, const std::string&, std::shared_ptr<number> p){
    if ( auto i = p )
      v.push_back(*i);
  });

  std::sort(v.begin(), v.end(), number_greater());

  if (v.size()!=3) return -1;

  if (v[0].count!=3) return -1;
  if (v[1].count!=2) return -1;
  if (v[2].count!=1) return -1;

  if ( reg.get_target<number>("i2")->count != 2 )
    return -1;

  if ( auto x = reg.get_target<number>("i4") )
    return -1;

  return 0;
}
