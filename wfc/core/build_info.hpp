//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/ibuild_info.hpp>
#include <string>
#include <sstream>
#include <memory>

namespace wfc{

template<typename BuildInfo>
class build_info
  : public ibuild_info
{
public:
  virtual ~build_info(){}

  virtual bool enabled() override
  {
    return _bi.enabled();
  }

  virtual std::string name() override
  {
    return _bi.name();
  }

  virtual std::string version_tag() override
  {
    return _bi.version_tag();
  }
  
  virtual std::string version() override
  {
    std::stringstream ss;
    ss << this->version_tag();
    if ( !this->version_ex().empty() )
      ss << "[" << this->version_ex() << "]";
    ss << "-" << this->build_count();
    return ss.str();
  }
  
  virtual std::string compiler_version() override
  {
    return _bi.compiler_version();
  }
  
  virtual std::string version_ex() override
  {
    return _bi.version_ex();
  }

  virtual std::string build_type() override
  {
    return _bi.build_type();
  }

  virtual std::string build_date() override
  {
    return _bi.build_date();
  }

  virtual std::string build_flags() override
  {
    return _bi.build_flags();
  }

  virtual std::string build_count() override
  {
    return _bi.build_count();
  }

  virtual std::string branch() override
  {
    return _bi.branch();
  }

  virtual std::string commit() override
  {
    return _bi.commit();
  }

  virtual std::string commit_date() override
  {
    return _bi.commit_date();
  }

  virtual std::string commit_author() override
  {
    return _bi.commit_author();
  }

  virtual std::string initial_author() override
  {
    return _bi.initial_author();
  }

  virtual std::string commit_message() override
  {
    return _bi.commit_message();
  }

  virtual std::string all_authors() override
  {
    return _bi.all_authors();
  }
private:
  BuildInfo _bi;
};

template<typename BuildInfo>
inline std::shared_ptr<ibuild_info> 
make_build_info()
{
  return std::make_shared< build_info<BuildInfo> >();
}

struct empty_build_info
{
  static bool enabled()
  {
    return true;
  }

  static std::string name() 
  {
    return std::string();
  }

  static std::string version_tag() 
  {
    return std::string();
  }
  
  static std::string version() 
  {
    return std::string();
  }

  static std::string compiler_version() 
  {
    return std::string();
  }
  
  static std::string version_ex() 
  {
    return std::string();
  }

  static std::string build_type() 
  {
    return std::string();
  }

  static std::string build_date() 
  {
    return std::string();
  }

  static std::string build_flags() 
  {
    return std::string();
  }

  static std::string build_count() 
  {
    return std::string();
  }

  static std::string branch() 
  {
    return std::string();
  }

  static std::string commit() 
  {
    return std::string();
  }

  static std::string commit_date() 
  {
    return std::string();
  }

  static std::string commit_author() 
  {
    return std::string();
  }

  static std::string initial_author() 
  {
    return std::string();
  }

  static std::string commit_message() 
  {
    return std::string();
  }

  static std::string all_authors() 
  {
    return std::string();
  }
};


}
