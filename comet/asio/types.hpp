#pragma once

#include <memory>
#include <vector>
#include <chrono>

namespace mamba{ namespace comet{ namespace inet{

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;
typedef std::chrono::high_resolution_clock::time_point time_point;

}}}
