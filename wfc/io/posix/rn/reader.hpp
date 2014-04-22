#pragma once

#include <wfc/io/posix/rn/reader_options.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

class reader_impl;
  
class reader
{
public:
  typedef reader_options options_type;
  typedef boost::asio::posix::stream_descriptor descriptor_type;
  ~reader();
  reader(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr);
  void start();
  void stop(std::function<void()> finalize);
  void shutdown();
  
  bool status() const;
  wfc::io::data_ptr read();
  void async_read(wfc::io::callback handler);

private:
  std::shared_ptr<reader_impl> _impl;
};
  
}}}}
