#pragma once

#include <wfc/io/posix/rn/writer_options.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace io{ namespace posix{ namespace rn{

class writer_impl;
  
class writer
{
public:
  typedef writer_options options_type;
  typedef boost::asio::posix::stream_descriptor descriptor_type;
  ~writer();
  writer(descriptor_type&& desc, const options_type& conf);
  void start();
  void stop();
  void shutdown();
  
  bool status() const;
  void write(wfc::io::data_ptr d);
  void async_write(wfc::io::data_ptr d);

private:
  std::shared_ptr<writer_impl> _impl;
};
  
}}}}
