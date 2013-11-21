#include <comet/fas/type_list.hpp>
#include <comet/fas/conversion.hpp>
#include <comet/fas/linear_hierarchy.hpp>
#include <comet/fas/scatter_hierarchy.hpp>

using namespace ::mamba::comet;

typedef type_list_n<
  int, char, double
>::type list1;

typedef type_list_n<
  int, char, double
>::type list2;

typedef type_list_n<
  list1,
  type_list_n<char>::type,
  list2
>::type list3;

int main()
{
  enum { result = static_check< length<list3>::result == 7 >::result };
  return 0;
}
