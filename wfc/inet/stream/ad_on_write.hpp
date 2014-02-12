#pragma once

#include <wfc/inet/stream/tags.hpp>
#include <wfc/logger.hpp>


namespace wfc{ namespace inet{ namespace stream{

struct ad_on_write
{
  template<typename T>
  void operator()(T& t)
  {
    size_t limit = t.context().outgoing_limit;
    size_t warn  = t.context().outgoing_warning;
    bool shutdown = t.context().shutdown;
    size_t size  = t.get_aspect().template get<_writer_>().outgoing_size();
    if ( limit!=0 && size>=limit)
    {
      // TODO: перенести в адвайсы
      COMMON_LOG_WARNING(" Outgoing data size=" << size << "(outgoing_limit==" << limit << "). Сonnection will be closed!" )
      t.close();
    }
    else if ( warn!=0 && size>=warn )
    {
      COMMON_LOG_WARNING(" Outgoing data size=" << size << "(outgoing_warning==" << warn << ")" )
    }
    
    //std::cout << "size=" << size << " " << shutdown << std::endl;
    if ( shutdown && size==0 )
    {
      //std::cout << "shutdown!!!" << std::endl;
      t.close();
    }
  }
};


}}}
