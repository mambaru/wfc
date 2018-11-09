//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/build_info.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/core/wfcglobal.hpp>

#include <fas/type_list.hpp>
#include <memory>
#include <string>
#include <list>

namespace wfc{

template<typename BuildInfo, typename... Args>  
class module_list
  : public ipackage
{
  typedef typename fas::type_list_n< Args... >::type module_types;
  typedef std::shared_ptr<imodule> module_ptr;
  typedef std::list< module_ptr > module_map;
  typedef std::shared_ptr<wfcglobal> global_ptr;

public:

  virtual std::shared_ptr<ibuild_info> build_info() override
  {
    return make_build_info<BuildInfo>();
  }

  virtual std::string name() const override
  {
    return BuildInfo().name();
  }

  virtual std::string description() const override
  {
    return "no description";
  }

  virtual int order() const override
  {
    return 1000;
  }

  virtual void create( std::shared_ptr<wfcglobal> g) override
  {
    this->create_( g, module_types() );
  }

  virtual std::vector< std::shared_ptr<imodule> > modules() override
  {
    std::vector< std::shared_ptr<imodule> > result;
    for (auto& p : _modules)
    {
      result.push_back(p);
    }
    return result;
  }

  // only for external control
  virtual void start(const std::string& arg) override
  {
    for (auto& p : _modules)
    {
      p->start(arg);
    }
  }

  virtual void stop(const std::string& arg) override
  {
    for (auto& p : _modules)
    {
      p->stop(arg);
    }
  }

private:

  static void create_(global_ptr, fas::empty_list) {}

  template<typename H, typename L>
  void create_(global_ptr g, fas::type_list< H, L > ) 
  {
    auto obj = std::make_shared<H>();
    _modules.push_back( obj );
    if ( g!=nullptr )
      g->registry.set("module", obj->name(), obj);
    obj->create(g);
    this->create_( g, L() );
  }
private:
  module_map _modules;
};

}
