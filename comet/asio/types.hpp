#pragma once

#include <memory>
#include <vector>

namespace mamba{ namespace comet{ namespace inet{

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

}}}
