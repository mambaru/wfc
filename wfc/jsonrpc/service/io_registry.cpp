
#include <wfc/jsonrpc/service/io_registry.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

  
io_registry::io_registry()
  : _call_id_counter(0)
{
  
}
  
void io_registry::set_io(io::io_id_t io_id, std::shared_ptr<handler_base> jsonrpc_handler, io::outgoing_handler_t outgoing_handler)
{
  auto result = _io_map.insert( std::make_pair( io_id, io_info(jsonrpc_handler, outgoing_handler) ) );
  if ( !result.second )
  {
    DAEMON_LOG_FATAL("jsonrpc::io_registry::set_io io_id=" << io_id << " exists");
    abort();
  }
}
  
std::shared_ptr<handler_base> io_registry::erase_io( io::io_id_t io_id )
{
  std::shared_ptr<handler_base> result;
  auto itr = _io_map.find(io_id);
  if ( itr != _io_map.end() )
  {
    result = itr->second.jsonrpc_handler;
    for ( auto& r : itr->second.result_map )
    {
      this->_call_io_map.erase(r.first);
    }
    this->_io_map.erase(itr);
  }
  return result;
}
  
std::pair<int, io::outgoing_handler_t>
io_registry::add_result_handler(io::io_id_t io_id, result_handler_t result_handler)
{
  std::pair<int, io::outgoing_handler_t> result(-1, nullptr);
  
  auto itr = this->_io_map.find(io_id);
  if (itr!=this->_io_map.end())
  {
    result.first = ++this->_call_id_counter;
    result.second = itr->second.outgoing_handler;
    itr->second.result_map.insert( std::make_pair(result.first, result_handler));
    this->_call_io_map[result.first] = io_id;
  }
  return result;
}

io_registry::result_handler_t io_registry::get_result_handler(int call_id) const
{
  auto itr = _call_io_map.find(call_id);
  if ( itr != _call_io_map.end() )
  {
    auto itr2 = _io_map.find(itr->second);
    if ( itr2 != _io_map.end() )
    {
      auto itr3 = itr2->second.result_map.find(call_id);
      if ( itr3 != itr2->second.result_map.end() )
      {
        return itr3->second;
      }
      else
      {
        DAEMON_LOG_ERROR("jsonrpc::service: jsonrpc id=" << call_id << " not found in response list");
      }
    }
    else
    {
      COMMON_LOG_WARNING("jsonrpc::service: io id=" << itr->second << " not found");
    }
  }
  else
  {
    COMMON_LOG_WARNING("jsonrpc::service: jsonrpc id=" << call_id << " not found");
  }
  return nullptr;
}
  
std::weak_ptr<handler_base> io_registry::get_jsonrpc_handler(io::io_id_t io_id) const
{
  auto itr = _io_map.find(io_id);
  
  return itr != _io_map.end() 
    ? itr->second.jsonrpc_handler
    : nullptr;
}
  
io::outgoing_handler_t io_registry::get_outgoing_handler(io::io_id_t io_id) const
{
  auto itr = _io_map.find(io_id);
  
  return itr != _io_map.end() 
    ? itr->second.outgoing_handler
    : nullptr;
}

void io_registry::clear()
{
  _io_map.clear();
  _call_io_map.clear();
}

}} // wfc


