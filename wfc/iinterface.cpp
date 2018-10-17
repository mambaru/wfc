//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#include "iinterface.hpp"

namespace wfc{

void iinterface::reg_io(io_id_t, std::weak_ptr<iinterface>) 
{
  
}

void iinterface::unreg_io(io_id_t) 
{
  
}

void iinterface::perform_io(data_ptr, io_id_t, output_handler_t handler)
{
  if (handler!=nullptr)
    return handler(nullptr);
}


}
