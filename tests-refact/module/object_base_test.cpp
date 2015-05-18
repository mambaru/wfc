#define IOW_DISABLE_ALL_LOG

#include <wfc/module/object_base.hpp>

struct options
{
};

struct itest
{
  
};

class test
  : public wfc::object_base<options, itest>
{
  
};

int main()
{
  test t;
}
