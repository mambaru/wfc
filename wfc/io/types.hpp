#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace wfc{ namespace io{ 

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef std::function<void(data_ptr)> callback;
typedef std::function<void(data_ptr, callback)> handler;
  
}}
