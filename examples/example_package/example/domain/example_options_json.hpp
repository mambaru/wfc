#include <wfc/json.hpp>
#include <example/domain/example_options.hpp>

struct example_options_json                                                                                                                                                          
{                                                                                                                                                                                 
  typedef wfc::json::object<
    example_options,
    wfc::json::member_list<
    >
  > type;                                                                                                                                                                         

  typedef typename type::target      target;                                                                                                                                      
  typedef typename type::member_list member_list;                                                                                                                                 
  typedef typename type::serializer  serializer;                                                                                                                                  
};

