#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/types.hpp>


//#include <wfc/io/reader/read_some.hpp>
//#include <wfc/io/basic/aspect.hpp>
//#include <wfc/io/common/fork.hpp>
//#include <wfc/memory.hpp>
//#include <fas/aop.hpp>
//#include <wfc/logger.hpp>
//#include <wfc/callback/callback_status.hpp>


#include <wfc/io/reader/aspect/ad_async_read_some.hpp>
#include <wfc/io/reader/aspect/ad_handler.hpp>
#include <wfc/io/reader/aspect/ad_log_abort.hpp>
#include <wfc/io/reader/aspect/ad_log_error.hpp>
#include <wfc/io/reader/aspect/ad_make_buffer.hpp>
#include <wfc/io/reader/aspect/ad_read_error.hpp>
#include <wfc/io/reader/aspect/ad_read_ready.hpp>
#include <wfc/io/reader/aspect/ad_read_more.hpp>
#include <wfc/io/reader/aspect/ad_start.hpp>
#include <wfc/io/reader/aspect/ad_user_handler.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

#include <fas/aop.hpp>

#include <list>
#include <atomic>
#include <chrono>

namespace wfc{ namespace io{ namespace reader{ 


  /*
template<typename Tg, typename TgEvent>
struct ready_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<TgEvent>()(t, d->begin(), d->end() );
    t.get_aspect().template get<Tg>()(t, std::move(d) );
  }
};
*/


/*
struct make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    return std::make_unique<typename T::data_type>( t.options().input_buffer_size );
  }
};

*/
/*
struct _result_ready_;
struct _result_error_;
struct _result_aborted_;
*/

// result_handler<_read_ready_, _read_aborted_, _read_error_>
// -> ad_read_handler
//template<typename TgReady, typename TgAborted, typename TgError>


/*
template<typename Tg, typename TgEvent>
struct error_handler
{
  template<typename T>
  void operator()(T& t, boost::system::error_code ec, typename T::data_ptr d)
  {
    t.get_aspect().template gete<TgEvent>()(t, ec );
    t.get_aspect().template get<Tg>()(t, std::move(d) );
  }
};
*/

/*
template<typename TgMain, typename TgSecond>
struct readed
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template get< TgMain >()(t, std::move(d));
    
    if ( t.get_aspect().template get<_is_started_>() )
    {
      t.get_aspect().template get< TgSecond >()(t);
    }
  }
};*/

///

struct _set_transfer_handler_;

template<typename TgResult>
struct set_transfer_handler
{
  template<typename T/*, typename Tmp*/>
  void operator()( T& t/*, const Tmp&*/)
  {
    
    auto& th = t.get_aspect().template get< ::wfc::io::_transfer_handler_ >();
    if ( th == nullptr )
    {
      th = t.callback([&t](typename T::data_ptr d)
      {
        t.get_aspect().template get<TgResult>()(t, std::move(d) );
      });
    }
  }
};


struct ad_free_buffer
{
  template<typename T>
  void operator()( T& , typename T::data_ptr )
  {
  }
};


/*
struct _start_;
struct _output_;
//struct _on_start_;
struct _read_handler_;
struct _readed_;
struct _aborted_;
struct _error_;
struct _on_read_;
struct _on_error_;
struct _on_aborted_;
struct _user_handler_;
*/

  
struct basic_options
{
  //size_t input_buffer_size = 8096;
  size_t input_buffer_size = 8096;
};

/*
 * Унивирсальный аспект
 * read_some ->
 *  ^  read_ready
 *  |------ handler ---> incoming ---> outgoing ---> user_handler --> output
 */

// wfc::io::rn::reader::_incoming_, wfc::io::rn::writer::_incoming_
//template< template<typename> class AsyncReadSome, typename TgOutgoing = _incoming_, typename TgOutput = _output_ >

struct aspect: fas::aspect<
  fas::group< ::wfc::io::_on_start_, _start_>,
  fas::advice< _read_more_,    ad_read_more >,
  fas::advice< _start_, ad_start >, 
  fas::advice< _read_some_,    ad_async_read_some >,
  fas::advice< _read_ready_,   ad_read_ready >,
  fas::advice< _read_error_,   ad_read_error >,
  fas::advice< _handler_,      ad_handler >,
  fas::advice< _user_handler_, ad_user_handler>,
  fas::advice< _make_buffer_,  ad_make_buffer>,
  fas::advice< _free_buffer_,  ad_free_buffer>,
  fas::stub<_output_>, // результат чтения и обработки 
  fas::alias<_incoming_, _outgoing_>, // заглушка для цепочки обработки
  fas::alias<_outgoing_, _user_handler_>
>{};

  
  
  /*
  fas::value< _error_code_, boost::system::error_code>,
  fas::advice< _make_buffer_, make_buffer>,
  fas::value< _is_started_, std::atomic<bool> >,
  
  fas::advice< _set_transfer_handler_, set_transfer_handler<TgOutput> >,
  
  fas::group<  ::wfc::io::_on_create_, _set_transfer_handler_ >,
  
  // start
  fas::advice< _start_, ad_start >, 
  fas::group< ::wfc::io::_on_start_, _start_>,
  
  //fas::advice<  ::wfc::io::_on_start_, dispatch<_start_> >,
  //fas::advice< _start_, event< _read_more_, _on_start_> >, 
  
  // loop
  fas::advice< _read_more_,    read_more<_read_some_> >,
  fas::advice< _read_some_,    AsyncReadSome<_read_handler_> >,      /// async_read_some - Вынести 
  fas::advice< _read_handler_, ad_read_handler  >,
  
  // events
  fas::advice< _read_ready_,   ready_handler< _readed_,   _on_read_> > ,
  fas::advice< _read_aborted_, error_handler< _aborted_,  _on_aborted_> >,
  fas::advice< _read_error_,   error_handler< _error_,    _on_error_> >,
    
  // ready
  fas::advice< _readed_, readed<TgOutgoing, _read_more_> >,         /// _outgoing_ - вынести или добавить handle??
  // fas::alias< TgOutgoing, _incoming_ >,   // заглушка для переопределения 
  fas::alias< _incoming_, _user_handler_>,
  fas::advice< _user_handler_, user_handler< TgOutput > >,
  fas::advice< TgOutput, output_stub>,
  fas::advice< _aborted_, output_stub>,
  fas::advice< _error_, output_stub>
  */


struct _log_abort_;
struct _log_error_;

/*

struct ad_log_aborted
{
  template<typename T>
  void operator()(T&, boost::system::error_code ec)
  {
     ::wfc::only_for_log(ec);
     TRACE_LOG_WARNING( "READER aborted: "  << ec.value() << " " << ec.message() )
  }
};

struct _log_error_;
struct ad_log_error
{
  template<typename T>
  void operator()(T&, boost::system::error_code ec)
  {
    ::wfc::only_for_log(ec);
    if( ec.value() != boost::system::errc::no_such_file_or_directory )
    {
      COMMON_LOG_ERROR( "READER error: " << ec.value() << " " << ec.message() )
    }
  }
};
*/

/// Еррор лог аспект

struct error_log: fas::aspect<
  fas::advice<_log_error_, ad_log_error>,
  fas::advice<_log_abort_, ad_log_abort>,
  fas::group<_on_error_, _log_error_>,
  fas::group<_on_abort_, _log_abort_>
>{};



}}}
