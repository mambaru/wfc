//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/wfcglobal.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{
  
class testing_domain
{
  typedef std::shared_ptr<iinstance> iinterface_ptr;

public:

  testing_domain()
  {
    _g = std::make_shared<wfcglobal>(_io);
    _g->common_workflow = std::make_shared<workflow>(_io);
  }
  
  template<typename D>
  std::shared_ptr<D> create(const typename D::domain_config& opt)
  {
    auto inst = std::make_shared< instance<D> >();
    inst->create(opt.name, _g);
    inst->configure(opt);
    _inst_list.push_back(inst);
    return inst->object();
  }
  
  void initialize() 
  {
    auto li = startup_sorted_();
    initialized = true;
    for (auto pobj: li)
      pobj->initialize();
  }

  void start(const std::string& args = std::string()) 
  {
    if (!initialized)
      this->initialize();
    started = true;
    auto li = startup_sorted_();
    for (auto pobj: li)
      pobj->start(args);
  }
  
  void stop(const std::string& args = std::string())
  {
    auto li = shutdown_sorted_();
    for (auto pobj: li)
      pobj->stop(args);
    started = false;
    initialized = false;
  }

  std::shared_ptr<wfcglobal> global() const { return _g; }
private:
  std::vector<iinterface_ptr> startup_sorted_()
  {
    std::vector<iinterface_ptr> il(_inst_list.begin(), _inst_list.end() );
    std::sort(il.begin(), il.end(), [](const iinterface_ptr& l, const iinterface_ptr& r){
      return l->startup_priority() < r->startup_priority();
    });
    return il;
  }
  std::vector<iinterface_ptr> shutdown_sorted_()
  {
    std::vector<iinterface_ptr> il(_inst_list.begin(), _inst_list.end() );
    std::sort(il.begin(), il.end(), [](const iinterface_ptr& l, const iinterface_ptr& r){
      return l->shutdown_priority() < r->shutdown_priority();
    });
    return il;
  }
private:
  bool initialized = false;
  bool started = false;
  std::shared_ptr<wfcglobal> _g;
  asio::io_service _io;
  std::list<iinterface_ptr> _inst_list;
};

}
