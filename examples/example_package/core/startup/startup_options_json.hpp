#include <wfc/json.hpp>
#include "startup_options.hpp"

struct startup_options_json                                                                                                                                                          
{                                                                                                                                                                                 
  typedef wfc::json::object<
    startup_options,
    wfc::json::member_list<
    >
  > type;                                                                                                                                                                         

  typedef typename type::target      target;                                                                                                                                      
  typedef typename type::member_list member_list;                                                                                                                                 
  typedef typename type::serializer  serializer;                                                                                                                                  
};
