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
    std::cout << "~~~work_thread()" << std::endl;
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
    std::cout << "work_thread()::stop [[[" << std::endl;
    callback_owner::reset();
    std::cout << "work_thread()::stop 1" << std::endl;
    this->_io_service.stop();
    std::cout << "work_thread()::stop 2" << std::endl;
    _thread.join();
    std::cout << "]]] work_thread()::stop" << std::endl;
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