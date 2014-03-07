#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <wfc/callback/callback_status.hpp>
#include <atomic>

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

typedef std::function< void(data_ptr) > callback;
typedef std::function<void(data_ptr, std::weak_ptr<iio>, callback )> handler;

inline 
std::function<void(data_ptr, std::weak_ptr<iio>, callback  )> 
simple_handler( std::function<void(data_ptr, callback )> handler )
{
  return [handler]( data_ptr d, std::weak_ptr<iio>, callback clb  )
  {
    handler( std::move(d), clb);
  };
}

typedef size_t io_id_t;

typedef std::function< void( std::function< void(io_id_t) > ) > add_shutdown_handler;
typedef std::function< void(io_id_t, callback, add_shutdown_handler )> startup_handler_t;
typedef std::function< void(io_id_t) > shutdown_handler_t;
//typedef std::function< void(data_ptr, io_id_t, callback )> transfer_handler_t;
typedef callback transfer_handler_t;


//std::atomic<io_id_t> global_io_id_counter(0);
  
}}
