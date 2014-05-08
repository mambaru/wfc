#include <wfc/core/global.hpp>
#include <wfc/logger/ilogger.hpp>
#include <wfc/memory.hpp>
#include <memory>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



namespace wfc{

std::weak_ptr<global> global::static_global;

}
