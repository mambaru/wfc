#pragma once

#include <wfc/callback/callback_owner.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <functional>

namespace wfc{ namespace inet{
  
class work_thread
 : callback_owner<std::mutex>
{
  typedef callback_owner<> super;
  typedef super::mutex_type mutex_type;

public:
  
  work_thread()
  {
    
  }
  
  ~work_thread()
  {
    
  }
  
  work_thread(const work_thread&) = delete;
  work_thread& operator=(const work_thread&) = delete;
  
  void start()
  {
    _thread = std::thread([this]()
    {
      ::boost::asio::io_service::work work(this->_io_service);
      this->_io_service.run();
    });
  }
  
  void stop()
  {
    
    callback_owner::reset();
    
    this->_io_service.stop();
    
    _thread.join();
    
  }

  template<typename SockType,  typename F>
  void post( std::shared_ptr<SockType> sock , F fun)
  {
    typename SockType::native_handle_type fd = ::dup(sock->native_handle());
    typename SockType::endpoint_type::protocol_type protoc = sock->local_endpoint().protocol();
    sock->close();

    _io_service.post( callback_owner::callback<callback_status>( [this, fd, protoc, fun]() 
    {
      std::shared_ptr<SockType> sock=std::make_shared<SockType>( this->_io_service, protoc, fd );
      fun( sock );
      return callback_status::ready;
    }));
  }
 
private:
  ::boost::asio::io_service _io_service;
  std::thread _thread;
};

}}