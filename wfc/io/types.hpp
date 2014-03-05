#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <wfc/callback/callback_status.hpp>

namespace wfc{ namespace io{ 

struct iio;
  
typedef std::function<void(std::weak_ptr<iio>)> release_handler;

struct iio
{
  virtual ~iio() {}
  virtual void add_release_handler( release_handler ) = 0;
};
  
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef std::function< callback_status(data_ptr) > callback;
typedef std::function<void(data_ptr, std::weak_ptr<iio>, callback )> handler;

std::function<void(data_ptr, std::weak_ptr<iio>, callback  )> 
simple_handler( std::function<void(data_ptr, callback )> handler )
{
  return [handler]( data_ptr d, std::weak_ptr<iio>, callback clb  )
  {
    handler( std::move(d), clb);
  };
}
  
}}
