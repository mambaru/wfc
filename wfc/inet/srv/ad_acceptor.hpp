#pragma once

#include <wfc/inet/tags.hpp>
#include <wfc/inet/srv/tags.hpp>
#include <wfc/memory.hpp>
#include <fas/aop/tag.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace inet{

struct ad_acceptor
{
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;
  
  ad_acceptor()
  {
  }
  
  /* initialize */
  /*
  template<typename T>
  void operator()(T& t, fas::tag<_startup_> )
  {
    //_acceptor = std::make_shared<acceptor_type>(t.get_io_service());
  }
  */
  
  /* start */
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    _acceptor = std::make_shared<acceptor_type>(t.get_io_service());
    
    boost::asio::ip::tcp::resolver resolver( _acceptor->get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      t.server_context().host, 
      t.server_context().port
    });

    _acceptor->open(endpoint.protocol());
    _acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor->bind(endpoint);
    _acceptor->listen();
    
    t.get_aspect().template get<_st_acceptor_>()(t, _acceptor);
    t.get_aspect().template get<_mt_acceptor_>()(t, _acceptor);
  }
  
  template<typename T>
  void operator()(T& t, fas::tag<_stop_>)
  {
    t.get_aspect().template get<_st_acceptor_>()(t);
    t.get_aspect().template get<_mt_acceptor_>()(t);
  }

  template<typename T>
  void operator()(T& /*t*/, fas::tag<_reconfigure_>)
  {
    // TODO: мягкий перезапуск
  }

private:
  std::shared_ptr< acceptor_type > _acceptor;
};

}}