#pragma once

#include <wfc/callback/callback_owner.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <functional>

namespace wfc{ namespace inet{
  
class work_thread
 : callback_owner<>
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef std::function<void(socket_type)> func_type;

public:
  work_thread()
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
     this->_io_service.stop();
    callback_owner::reset();
    _thread.join();
  }

  void post(socket_type sock , func_type fun)
  {
    socket_type::native_handle_type fd = ::dup(sock.native_handle());
    ::boost::asio::ip::tcp::endpoint::protocol_type protoc = sock.local_endpoint().protocol();
    sock.close();

    _io_service.post( callback_owner::callback( [this, fd, protoc, fun]() 
    {
      socket_type sock( this->_io_service, protoc, fd );
      fun( std::move(sock) );
      return callback_status::ready;
    }));
  }
 
private:
  ::boost::asio::io_service _io_service;
  std::thread _thread;
};

}}