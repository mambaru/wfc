
#include <wfc/jsonrpc/service/service_impl.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace jsonrpc{

service::service( io_service& ios, const options_type& opt, const handler_interface& handler)
  : _impl( std::make_shared<service_impl>(ios, opt, handler) )
  //, _threads(opt.threads)
{
  /*
  for (int i = 0; i < _threads; ++i)
  {
    _io_service_list.push_back( std::make_shared<io_service>() );
  }

  for (int i = 0; i < _threads; ++i)
  {
    _impl_list.push_back( std::make_shared<service_impl>( *(_io_service_list[i]), opt, handler) );
  }
  */
}

void service::create_handler( io_id_t io_id,  outgoing_handler_t writer, add_shutdown_handler_t add_shutdown )
{
  /*if ( _threads == 0  )
  {
    */
    _impl->create_handler(io_id, writer, add_shutdown);
  /*}
  else
  {
    _impl_list[ io_id % _threads ]->create_handler(io_id, writer, add_shutdown);
  }
  */
}

void service::operator()( data_ptr d,  io_id_t io_id,  outgoing_handler_t outgoing_handler)
{
  /*if ( _threads == 0  )
  {
    */
    _impl->operator ()( std::move(d), io_id, outgoing_handler);
  /*}
  else
  {
    _impl_list[ io_id % _threads ]->operator ()( std::move(d), io_id, outgoing_handler);
  }
  */
}

std::vector<std::string> service::get_methods() const
{
  return _impl->get_methods();
}

void service::start()
{
  /*if ( _threads == 0  )
  {*/
    _impl->start();
  /*}
  else
  {
    for (int i = 0; i < _threads; ++i)
    {
      std::function<void()> f = std::bind(&service::run_thread, this, i);
      _thread_list.push_back( std::thread( f ) );
    }
  }*/
}

void service::stop()
{
  /*if ( _threads == 0  )
  {*/
    _impl->stop();
  /*}
  else
  {
    for (int i = 0; i < _threads; ++i)
      _io_service_list[i]->stop();
    
    for (int i = 0; i < _threads; ++i)
      _thread_list[i].join();
    
    _thread_list.clear();
  }*/
}

auto service::get_prototype() const
->std::weak_ptr<handler_interface> 
{
  return _impl->get_prototype();
}
  
auto service::clone_prototype() const
->std::shared_ptr<handler_interface> 
{
  return _impl->clone_prototype();
}

/*void service::run_thread(int idx)
{
  io_service::work wrk( *(_io_service_list[idx]) );
  _impl_list[idx]->start();
  _io_service_list[idx]->run();
}
*/


}} 