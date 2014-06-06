#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <iostream>

typedef wfc::jsonrpc::incoming_holder holder_type;

// TODO: в тесты

struct context
{
  typedef wfc::jsonrpc::incoming_holder holder_type;
  typedef int call_id_t; 
  typedef holder_type::data_type data_type;
  typedef holder_type::data_ptr  data_ptr;
  typedef std::function<void(holder_type::data_ptr)> outgoing_handler_t; 
  
  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;
  typedef std::function< void(holder_type) > result_handler_t;
  
  void send_request(
    const char* name, 
    std::unique_ptr<int> req, 
    std::function< data_ptr(const char*, std::unique_ptr<int>, call_id_t) > ser, 
    std::function< void(holder_type)> result_handler 
  )
  {
    data_ptr str = ser( name, std::move(req), 1);
    std::cout << std::string( str->begin(), str->end() ) << std::endl;
    
    const char result[]="{\"result\":66, \"id\":1}";
    auto resp = std::make_unique<data_type>( std::begin(result), std::end(result) );
    holder_type holder( std::move(resp) );
    holder.parse();
    result_handler( std::move(holder));
    /*
     data_ptr str = ser( name, std::move(req), 1);
     std::cout << std::string( str->begin(), str->end() ) << std::endl;
     holder_type holder( std::move(str) );
     holder.parse();
     std::unique_ptr<int> params = holder.get_params< wfc::json::value<int> >();
     ResultHandler(  std::move(str) );
     */
  }
  template<typename PReq, typename Serializer>
  void send_notify(const char* /*name*/, PReq /*req*/, Serializer) 
  {
    
  }
};

struct foo_method
{
  const char* name() const { return "foo";}
};

int main(int , char* [])
{
  
  typedef wfc::jsonrpc::call< wfc::json::value<int>, wfc::json::value<int> > foo_call;
  
  context ctx;
  foo_call foo;
  foo_method method;
  
  foo( ctx, method, std::make_unique<int>(33), 
  [](std::unique_ptr<int> resp, std::unique_ptr<wfc::jsonrpc::error> err )
  {
    std::cout << "ok" << std::endl;
    if ( resp!= nullptr )
    {
      std::cout << *resp << std::endl;
    }
    else if (err!=nullptr)
    {
      std::cout << err->message << std::endl;
    }
    else
    {
      std::cout << "emdpty" << std::endl;
    }
  });
  
}