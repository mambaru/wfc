#pragma once

#include <wfc/io/basic/tags.hpp>

#include <wfc/io/basic/ad_create.hpp>
#include <wfc/io/basic/ad_post.hpp>
#include <wfc/io/basic/ad_wrap.hpp>
#include <wfc/io/basic/ad_dispatch.hpp>

#include <wfc/io/basic/types.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io/context.hpp>
#include <wfc/io/types.hpp>

#include <wfc/callback/callback_owner.hpp>

#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>

#include <fas/aop.hpp>

#include <boost/asio/strand.hpp>
#include <boost/asio/io_service.hpp>

namespace wfc{ namespace io{ namespace basic{

  /*
struct _startup_handler_; 
struct _shutdown_handler_;
*/

  
struct options
{
  std::function<void()> not_alive = [](){ };

  
  startup_handler_t  startup_handler = nullptr;
  shutdown_handler_t shutdown_handler = nullptr;
  incoming_handler_t incoming_handler = nullptr;
  outgoing_handler_t outgoing_handler = nullptr;

  /*
  transfer_handler_t transfer_handler = nullptr;
  */

};

  
typedef fas::type_list_n<
  
  fas::type< ::wfc::io::_io_service_type_, boost::asio::io_service >, 
  fas::type< _strand_type_, boost::asio::strand >,
  fas::type< _owner_type_, ::wfc::callback_owner >,
  fas::type< ::wfc::io::_data_type_, data_type >,
  fas::type< ::wfc::io::_options_type_, options >,
  fas::type< _mutex_type_, rwlock<spinlock> >,
  fas::type< _lock_guard_, std::lock_guard< rwlock<spinlock> > >,
  fas::type< _read_lock_, read_lock< rwlock<spinlock> > >,

  fas::advice< _create_, ad_create >,
  fas::advice< ::wfc::io::_post_, ad_post >,
  fas::advice< ::wfc::io::_wrap_, ad_wrap >,
  fas::advice< ::wfc::io::_dispatch_, ad_dispatch >, 

  fas::value< _strand_, std::shared_ptr<boost::asio::strand> >, 
  fas::value< _owner_, std::shared_ptr< ::wfc::callback_owner> >, 
  fas::value< _not_alive_, std::function<void()> >, 
  //fas::value< _startup_handler_, startup_handler_t>,
  //fas::value< _shutdown_handler_, shutdown_handler_t>,
   fas::value< _transfer_handler_, outgoing_handler_t >, // TODO: удалить 

  fas::stub< ::wfc::io::_start_>, 
  fas::group< ::wfc::io::_create_, _create_>
  
>::type advice_list;


struct aspect: fas::aspect
< 
  ::wfc::io::context<>,
  advice_list
>
{};

}}}
