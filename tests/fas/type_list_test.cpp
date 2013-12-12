#include <fas/type_list.hpp>
#include <fas/hierarchy.hpp>
#include <fas/static_check.hpp>

//using namespace ::wfc;

typedef fas::type_list_n<
  int, char, double
>::type list1;

typedef fas::type_list_n<
  int, char, double
>::type list2;

typedef fas::type_list_n<
  list1,
  fas::type_list_n<char>::type,
  list2
>::type list3;

int main()
{
  enum { result = fas::static_check< fas::length<list3>::value == 7 >::value };
  return 0;
}
