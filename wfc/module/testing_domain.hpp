//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/wfcglobal.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{
  
class domain_test
{
  typedef std::shared_ptr<iinstance> iinterface_ptr;

public:

  domain_test()
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
    inst->initialize();
    inst->start("");
    for (auto pobj: _inst_list)
      pobj->initialize();
    _inst_list.push_back(inst);
    return inst->object();
  }
  
private:
  std::shared_ptr<wfcglobal> _g;
  asio::io_service _io;
  std::list<iinterface_ptr> _inst_list;
};

}
