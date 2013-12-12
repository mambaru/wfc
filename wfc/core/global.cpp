#include <wfc/core/global.hpp>
#include <wfc/logger/ilogger.hpp>
#include <wfc/memory.hpp>
#include <memory>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



namespace wfc{

std::weak_ptr<global> global::static_global;
  /*
std::weak_ptr<ilogger> global::global_logger= std::weak_ptr<ilogger>();
std::weak_ptr<icore>   global::global_core   = std::weak_ptr<icore>();
*/
  
  /*
void  global::idle(std::chrono::milliseconds ms, std::function<void()> handle)
{
  if ( auto io = this->io_service.lock() )
  {
    std::cout << "ms.count() " << ms.count() << std::endl;
    std::shared_ptr<boost::asio::deadline_timer> t = 
      std::make_shared<boost::asio::deadline_timer>(*io, boost::posix_time::milliseconds(ms.count()));
      
    std::function<void(const boost::system::error_code&)> func = [func, t, ms, handle](const boost::system::error_code& e)
    {
      if (!e)
      {
        std::cout << "expires_at 1" << std::endl;
        handle();
        std::cout << "expires_at 2" << std::endl;
        t->expires_at(t->expires_at() + boost::posix_time::milliseconds(ms.count()));
        std::cout << "expires_at 3" << std::endl;
        t->async_wait( func );
        std::cout << "expires_at 4" << std::endl;
      }
      else
        std::cout << "expires_at error" << std::endl;
    };
    t->async_wait(func);
    */

    /*
    t->async_wait([&t, ms, handle](const boost::system::error_code& e)
    {
      if (!e)
      {
        
      }
    });
    */
    
    //auto f = [this, ms, handle](){}
  /*}
}*/

}
