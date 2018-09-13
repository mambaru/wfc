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
  workflow(io_service_type& io, const workflow_options& opt )
    : super(io, opt)
  {};
};

/*using ::wflow::workflow;*/

}
