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
    return BuildInfo().enabled();
  }

  virtual std::string name() override
  {
    return BuildInfo().name();
  }

  virtual std::string version_tag() override
  {
    return BuildInfo().version_tag();
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

  virtual std::string version_ex() override
  {
    return BuildInfo().version_ex();
  }

  virtual std::string build_type() override
  {
    return BuildInfo().build_type();
  }

  virtual std::string build_date() override
  {
    return BuildInfo().build_date();
  }

  virtual std::string build_flags() override
  {
    return BuildInfo().build_flags();
  }

  virtual std::string build_count() override
  {
    return BuildInfo().build_count();
  }

  virtual std::string branch() override
  {
    return BuildInfo().branch();
  }

  virtual std::string commit() override
  {
    return BuildInfo().commit();
  }

  virtual std::string commit_date() override
  {
    return BuildInfo().commit_date();
  }

  virtual std::string commit_author() override
  {
    return BuildInfo().commit_author();
  }

  virtual std::string initial_author() override
  {
    return BuildInfo().initial_author();
  }

  virtual std::string commit_message() override
  {
    return BuildInfo().commit_message();
  }

  virtual std::string all_authors() override
  {
    return BuildInfo().all_authors();
  }
};

template<typename BuildInfo>
inline std::shared_ptr<ibuild_info> 
make_build_info()
{
  return std::make_shared< build_info<BuildInfo> >();
}


}
