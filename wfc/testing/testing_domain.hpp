//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/wfcglobal.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{

class testing_domain
{
  typedef std::shared_ptr<iinstance> instance_ptr;

public:

  virtual ~testing_domain();

  testing_domain();

  template<typename D>
  std::shared_ptr<D> create(const typename D::domain_config& opt);

  void initialize();

  void start(const std::string& args = std::string());

  void stop(const std::string& args = std::string());

  std::shared_ptr<wfcglobal> global() const;

private:

  std::vector<instance_ptr> startup_sorted_();

  std::vector<instance_ptr> shutdown_sorted_();

private:

  bool initialized = false;
  bool started = false;
  boost::asio::io_context _io;
  std::shared_ptr<wfcglobal> _g;
  std::list<instance_ptr> _inst_list;
};


template<typename D>
std::shared_ptr<D> testing_domain::create(const typename D::domain_config& opt)
{
  auto inst = std::make_shared< instance<D> >();
  inst->create(_g);
  inst->configure(opt);
  _inst_list.push_back(inst);
  return inst->object();
}

}
