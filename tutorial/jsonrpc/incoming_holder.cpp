#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <iostream>

int main(int , char* [])
{
  //std::cout << " ------------------------------------" << std::endl;
  typedef wfc::jsonrpc::incoming_holder parser_type;
  const char jsonrpc[]=
    "{}"
    "{\"method\":\"method\", \"params\":33, \"id\":1}"
    "{\"method\":\"method\", \"params\":\"33\", \"id\":1}"
    "{\"method\":\"method\", \"id\":{} }"
    "{\"method\":\"method\"}"
    "[]"
    "{}";
  auto ptr = std::make_unique<parser_type::data_type>( std::begin(jsonrpc), std::end(jsonrpc));

  for( int i=1; ptr!=nullptr; ++i )
  {
    std::cout << i << ": ------------------------------------" << std::endl;
    parser_type parser( std::move(ptr) );
    try 
    {
      ptr = parser.parse();
    }
    catch( wfc::json::json_error& e)
    {
      std::cout << parser.error_message(e) << std::endl;
    }
    
    if ( !parser.is_valid() )
    {
      std::cout << "Invalid json object" << std::endl;
    }
    
    if ( parser.is_request() )
    {
      std::cout << "Request." << std::endl;
      
      try
      {
        std::cout << "id=" << parser.get_id<int>() << std::endl;
      }
      catch(wfc::json::json_error& e)
      {
        std::cout << "invalid id" << std::endl;
        std::cout << parser.id_error_message(e) << std::endl;
      }
      
      try
      {
        auto params_ptr = parser.get_params< wfc::json::value<int> >();
        
        if ( params_ptr!=nullptr )
          std::cout << "params = " << *params_ptr << std::endl;
        else
          std::cout << "params = null" << std::endl;
      }
      catch( wfc::json::json_error& e)
      {
        std::cout << "invalid params" << std::endl;
        std::cout << parser.params_error_message(e) << std::endl;
      }
    }
    else if (parser.is_notify() )
    {
      std::cout << "Notify." << std::endl;
    }
    else if (parser.is_response() )
    {
      std::cout << "Result." << std::endl;
    }
    else if (parser.is_error() )
    {
      std::cout << "Error." << std::endl;
    }
    
  }
}