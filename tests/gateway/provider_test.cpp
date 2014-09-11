#include <wfc/gateway/provider/provider.hpp>

struct itest
{
  
};

int main()
{
  typedef ::wfc::gateway::provider<itest> provider_type;
  typedef ::wfc::gateway::provider_config provider_config;
  provider_config conf;
  provider_type privider(conf);
  return 0;
}
