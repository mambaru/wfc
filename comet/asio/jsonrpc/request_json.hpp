#pragma once


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct request_json
{
  NAME(id);
  NAME(method);
  
  typedef json::object<
    request, 
    fas::type_list_n<
      // members
    >::type
  > type;
};

}}}}

#endif
