#include <wfc/json.hpp>
#include "core_options.hpp"

struct core_options_json                                                                                                                                                          
{                                                                                                                                                                                 
  typedef wfc::json::object<
    core_options,
    wfc::json::member_list<
    >
  > type;                                                                                                                                                                         

  typedef typename type::target      target;                                                                                                                                      
  typedef typename type::member_list member_list;                                                                                                                                 
  typedef typename type::serializer  serializer;                                                                                                                                  
};

