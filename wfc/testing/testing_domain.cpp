//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//


#include "testing_domain.hpp"

namespace wfc{

testing_domain::~testing_domain()
{
  _g->common_workflow = nullptr;
  _g = nullptr;
  _inst_list.clear();
}

testing_domain::testing_domain()
  : _io()
{
  _g = std::make_shared<wfcglobal>(_io);
  _g->common_workflow = std::make_shared< wflow::workflow>(_io);
}

void testing_domain::initialize()
{
  auto li = startup_sorted_();
  _initialized = true;
  for (auto pobj: li)
    pobj->initialize();
}

void testing_domain::start(const std::string& args )
{
  if (!_initialized)
    this->initialize();
  _started = true;
  auto li = startup_sorted_();
  for (auto pobj: li)
    pobj->start(args);
}

void testing_domain::stop(const std::string& args )
{
  auto li = shutdown_sorted_();
  for (auto pobj: li)
    pobj->stop(args);
  _g->common_workflow->stop();
  _started = false;
  _initialized = false;
}

std::shared_ptr<wfcglobal> testing_domain::global() const
{
  return _g;
}


std::vector<testing_domain::instance_ptr> testing_domain::startup_sorted_()
{
  std::vector<instance_ptr> il(_inst_list.begin(), _inst_list.end() );
  std::sort(il.begin(), il.end(), [](const instance_ptr& l, const instance_ptr& r){
    return l->startup_priority() < r->startup_priority();
  });
  return il;
}

std::vector<testing_domain::instance_ptr> testing_domain::shutdown_sorted_()
{
  std::vector<instance_ptr> il(_inst_list.begin(), _inst_list.end() );
  std::sort(il.begin(), il.end(), [](const instance_ptr& l, const instance_ptr& r){
    return l->shutdown_priority() < r->shutdown_priority();
  });
  return il;
}

}
