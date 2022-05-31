//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2022
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <functional>
#include <memory.h>
#include <wflow/owner.hpp>
#include <iow/io/types.hpp>
#include <wfc/logger.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{ namespace jsonrpc{

struct ad_callback
{
  template<typename T, typename Opt>
  void initialize(T& , const Opt& opt)
  {
    //JSONRPC_LOG_MESSAGE("ad_callback::initialize target_name=" << opt.domain_name)
    //abort(); // Инициализировать опции
    
    this->name = opt.domain_name;
    this->tracking_id = opt.tracking_id;
    this->disable_tracking_log = opt.disable_tracking_log;
    this->powner = opt.powner;

  }

  template<typename T, typename Result>
  std::function<void(std::unique_ptr<Result>)>
    operator()(T& , std::function<void(std::unique_ptr<Result>)> cb)
  {
    if ( powner!=nullptr )
    {
      if ( tracking_id!=iinterface::bad_id )
      {
        typedef std::unique_ptr<Result> result_ptr;
        std::function<void(result_ptr)> track_fall = nullptr;
        if ( !disable_tracking_log )
        {
          std::string showname = name;
          track_fall = [showname](result_ptr){ 
            /* DEBUG: TODO */ 
            JSONRPC_LOG_WARNING("Detect fall tracking response for '" << showname << "'")
          };
        }
        return powner->tracking(
          tracking_id,
          powner->callback(std::move(cb)),
          std::move(track_fall)
        );
      }
      else
        return powner->callback(std::move(cb));
    }
    return cb;
  }
private:
  std::string name;
  iinterface::io_id_t tracking_id = iinterface::bad_id;
  bool disable_tracking_log = false;
  wflow::owner* powner;
 
};

}}
