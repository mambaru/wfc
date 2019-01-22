//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wflow/workflow.hpp>
#include <wfc/core/workflow_options.hpp>
#include <wfc/core/workflow_options_json.hpp>

namespace wfc{

/** 
 * @brief workflow  
 * @details Описание workflow   
 */
class workflow
  : public ::wflow::workflow
{
  typedef ::wflow::workflow super;
public:
  workflow(io_service_type& io )
    : super(io)
  {};

  workflow(io_service_type& io, const workflow_options& opt )
    : super(io, opt)
  {};
};

}
