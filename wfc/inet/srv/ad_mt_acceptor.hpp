#pragma once

#include <wfc/inet/srv/accept_thread.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace inet{

struct ad_mt_acceptor
{
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;
  
  template<typename T>
  void operator()(T& /*t**/)
  {
    std::cout << "ad_mt_acceptor stop" << std::endl;
    for (auto& thr : _accept_threads)
    {
      thr->stop();
    }
    _accept_threads.clear();
  }

  template<typename T>
  void operator()(T& t, std::weak_ptr<acceptor_type> acceptor)
  {
    int current = _accept_threads.size();
    int threads = t.server_context().listen_threads;
    _accept_threads.resize(threads);
    for (int i=current; i< threads; ++i)
    {
      _accept_threads[i] = std::make_unique<accept_thread>(t.get_io_service());
    }
    for (int i=0; i< threads; ++i)
    {
      _accept_threads[i]->start(t, acceptor );
    }
  }
private:
  std::vector< std::unique_ptr<accept_thread> > _accept_threads;
};

}}