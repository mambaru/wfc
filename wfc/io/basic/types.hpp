#pragma once

#include <vector>
#include <memory>

namespace wfc{ namespace io{ namespace basic{

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

  
}}}
