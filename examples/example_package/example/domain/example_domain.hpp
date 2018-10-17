#include <wfc/domain_object.hpp>
#include <iostream>
#include <memory>
#include <example/domain/example_options.hpp>
#include <example/iexample.hpp>

class example_domain
  : public wfc::domain_object< iexample, example_options >
{
public:
  virtual ~example_domain();
};

