#pragma once

#include <string>

namespace mamba{ namespace comet{

struct iconfig
{
  virtual ~iconfig() {}
  virtual void reconfigure() = 0;
  virtual bool parse_config(const std::string& path) = 0;
  virtual void configure(const std::string& path) = 0;
  virtual std::string get_config(const std::string& name) = 0;
  virtual std::string generate(const std::string& type, const std::string& path) = 0;
};

}}
