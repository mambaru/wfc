//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <string>
#include <memory>

namespace wfc{

struct ibuild_info
{
  virtual ~ibuild_info(){}
  virtual bool enabled() = 0;
  virtual std::string name() = 0;
  virtual std::string version() = 0;
  virtual std::string branch() = 0;
  virtual std::string commit() = 0;
  virtual std::string date() = 0;
  virtual std::string author() = 0;
  virtual std::string project() = 0;
  virtual std::string message() = 0;
  virtual std::string authors() = 0;
};


template<typename BuildInfo>
class build_info
  : public ibuild_info
{
public:
  
  virtual ~build_info(){}

  virtual bool enabled()
  {
    return BuildInfo().enabled();
  }

  virtual std::string name()
  {
    return BuildInfo().name();
  }
  
  virtual std::string version()
  {
    return BuildInfo().version();
  }
  
  virtual std::string branch()
  {
    return BuildInfo().branch();
  }
  
  virtual std::string commit()
  {
    return BuildInfo().commit();
  }
  
  virtual std::string date()
  {
    return BuildInfo().date();
  }
  
  virtual std::string author()
  {
    return BuildInfo().author();
  }

  virtual std::string project()
  {
    return BuildInfo().project();
  }
  
  virtual std::string message()
  {
    return BuildInfo().message();
  }
  
  virtual std::string authors()
  {
    return BuildInfo().authors();
  }
};

template<typename BuildInfo>
inline std::shared_ptr<ibuild_info> 
make_build_info()
{
  return std::make_shared< build_info<BuildInfo> >();
}


}
