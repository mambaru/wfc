#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/method/aspect/invoke.hpp>
#include <iostream>

typedef wfc::jsonrpc::incoming_holder holder_type;

struct context
{
  typedef wfc::jsonrpc::incoming_holder holder_type;
  typedef holder_type::data_type data_type;
  typedef holder_type::data_ptr  data_ptr;
  typedef std::function<void(holder_type::data_ptr)> outgoing_handler_t; 
  
 
};


struct foo_handler
{
  template<typename T>
  void operator()(T& , std::unique_ptr<int> req, std::function<void(std::unique_ptr<int>, std::unique_ptr<wfc::jsonrpc::error> )> callback ) const
  {
    if ( callback != nullptr )
    {
      callback( std::make_unique<int>((*req)*2), nullptr );
    }
  }
};


int main(int , char* [])
{
  
  typedef wfc::jsonrpc::invoke< wfc::json::value<int>, wfc::json::value<int>, foo_handler > foo_invoke;
  /*
  context ctx;
  foo_invoke foo;
  
  const char jsonrpc[]=
    "{\"method\":\"method\", \"params\":33, \"id\":1}"
    ;
  auto ptr = std::make_unique<holder_type::data_type>( std::begin(jsonrpc), std::end(jsonrpc));
  holder_type holder( std::move(ptr) );
  holder.parse();
  foo( ctx, ctx, std::move( holder ), []( holder_type::data_ptr d) 
  {
    std::cout << std::string(d->begin(), d->end()) << std::endl;
  });
  */
}