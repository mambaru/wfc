#pragma once

#include <wfc/inet/tags.hpp>
#include <wfc/inet/srv/tags.hpp>
#include <wfc/inet/srv/connection_manager.hpp>
#include <wfc/memory.hpp>
#include <fas/aop/tag.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace wfc{ namespace inet{

struct ad_server_start
{
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    _timer = std::make_shared<boost::asio::deadline_timer>(t.get_io_service(), boost::posix_time::seconds(1));
    
    //_timer->async_wait([this, &t](){
      this->do_shutdown_inactive(t);
    //});
  }
  
  template<typename T>
  void operator()(T& t, fas::tag<_stop_>)
  {
    //t.get_aspect().template get<_connection_manager_>()->stop();
    t.connection_manager()->stop();
  }

  template<typename T>
  void operator()(T& /*t*/, fas::tag<_startup_>)
  {
    //t.get_aspect().template get<_connection_manager_>() = std::make_shared<connection_manager<> >();
  }
  
  template<typename T>
  void do_shutdown_inactive(T& t)
  {
    
    _timer->async_wait([this, &t](const ::boost::system::error_code&)
    {
      //t.get_aspect().template get<_connection_manager_>()->shutdown_inactive(10);
      t.connection_manager()->shutdown_inactive(60);
      this->_timer->expires_at(_timer->expires_at() + boost::posix_time::seconds(1));
      this->do_shutdown_inactive(t);
    });

    //t.get_io_service()
  }
private:
  std::shared_ptr<boost::asio::deadline_timer> _timer;

};

}}