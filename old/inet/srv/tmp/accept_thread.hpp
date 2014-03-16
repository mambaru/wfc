#pragma once

#include <wfc/inet/srv/tmp/connection_manager.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <memory>

namespace wfc{ namespace inet{

class accept_one
{
public:
  
  /*typedef ::boost::asio::ip::tcp::socket socket_type;*/
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;
  
  accept_one(
    boost::asio::io_service& io_service,
    acceptor_type& acceptor//,
    //socket_type& socket,
    //std::weak_ptr<connection_manager> manager
  ) : _io_service(io_service)
    , _acceptor(acceptor)
    //, _socket(socket)
    //, _manager( manager )
  {}
  
  template<typename T>
  void do_accept(T& t)
  {
    typedef typename T::socket_type socket_type;
    typedef typename T::connection_type connection_type;
    typedef std::shared_ptr<socket_type> socket_ptr;
    socket_ptr sock = std::make_shared< socket_type >( _io_service );
    _acceptor.async_accept(
      *sock,
      [this, &t, sock](boost::system::error_code ec)
      {
        if (!this->_acceptor.is_open())
          return;

        if (!ec)
        {
          t.get_aspect().template get<_socket_>()(t, *sock); 
          t.get_aspect().template get<_worker_>()(t, sock, [this, &t](socket_ptr sock) 
          {
            
            std::shared_ptr<connection_type> pconn = t.create_connection(
              sock, 
              [this, &t](std::shared_ptr<connection_type> pconn)->void
              {
                t.connection_manager()->erase(pconn);
                //t.get_aspect().template get<_connection_manager_>()->erase(pconn);
                /*
                if ( auto m = this->_manager.lock() )
                  m->erase(pconn);
                */
              }
            );
            //pconn->context().activity = t.get_aspect().template get<_connection_manager_>();
            //pconn->context().activity = t.connection_manager();
            pconn->start();
            
            //t.get_aspect().template get<_connection_manager_>()->insert(pconn);
            t.connection_manager()->insert(pconn);
            /*
            if ( auto m = this->_manager.lock() )
              m->insert(pconn);
              */
            
          });
        }
        
        this->do_accept(t);
      }
    );
  }
  
  // TODO: stop чере post io_service
  
private:
  boost::asio::io_service& _io_service;
  acceptor_type& _acceptor;
  //socket_type& _socket;
  //std::weak_ptr<connection_manager> _manager;
};

/*
 *          Times for 0 RPS:                                                             HTTP for 0 RPS:                                  . Duration: 0:01:26                                                          ETA: 0:00:00
                                     132   0.01%: 3000 -- 3500  100.00% < 3500                                                                                                       . 
                                    1847   0.12%: 1000 -- 1500   99.99% < 1500                               NET for 0 RPS:                                                          . Hosts: migashko-linux => localhost:12345
                                     877   0.06%:  500 --  600   99.87% <  600                                1516551 100.00%: 0 Success                                             .  Ammo: queries.txt 
                                     100   0.01%:  400 --  450   99.81% <  450                                                                                                       . Count: 2000000
                                     457   0.03%:  350 --  400   99.81% <  400                              Cumulative Cases Info:                                                   .  Load: const(100000, 20s)
                                     432   0.03%:  300 --  350   99.77% <  350                                                                                                       . 
                                       3   0.00%:  250 --  300   99.75% <  300                              Cumulative Percentiles:                                                  . Active instances: 1000
                                       1   0.00%:  200 --  250   99.75% <  250                                 100% < 3051 ms                                                        . Planned requests: 0 for 0:01:05
                                       1   0.00%:   90 --  100   99.75% <  100                                  99% < 60 ms                                                          . Actual responses: 24531
                                      40   0.00%:   80 --   90   99.75% <   90                                  98% < 58 ms                                                          .         Accuracy: 93.74%
                                    1025   0.07%:   70 --   80   99.74% <   80                                  95% < 55 ms                                                          .         Time lag: 0:00:02
                                    8738   0.58%:   60 --   70   99.68% <   70                                  90% < 53 ms                                                          . 
                                  227414  15.00%:   50 --   60   99.10% <   60                                  85% < 51 ms                                                          . 
                                  603230  39.78%:   40 --   50   84.10% <   50                                  80% < 49 ms                                                          . 
                                  373742  24.64%:   30 --   40   44.33% <   40                                  75% < 48 ms                                                          . 
                                  290295  19.14%:   20 --   30   19.68% <   30                                  50% < 42 ms                                                          . 
                                    8217   0.54%:   10 --   20    0.54% <   20                                  25% < 32 ms                                                          . 
                                                                                                                                                                 
  */

/*
 *                                                                                                                                                                     . ==============================================================================/~~ 97.8%
                                Times for 0 RPS:                                                           HTTP for 0 RPS:                                   . Duration: 0:01:07                                                          ETA: 0:00:01
                                       15   0.00%: 650 -- 700  100.00% < 700                                                                                                         . 
                                      980   0.07%: 600 -- 650  100.00% < 650                                NET for 0 RPS:                                                           . Hosts: migashko-linux => localhost:12345
                                      995   0.07%: 350 -- 400   99.93% < 400                                 1371318 100.00%: 0 Success                                              .  Ammo: queries.txt 
                                       12   0.00%:  50 --  60   99.85% <  60                                                                                                         . Count: 2000000
                                    11484   0.84%:  40 --  50   99.85% <  50                               Cumulative Cases Info:                                                    .  Load: const(100000, 20s)
                                   925528  67.49%:  30 --  40   99.02% <  40                                                                                                         . 
                                   430336  31.38%:  20 --  30   31.52% <  30                               Cumulative Percentiles:                                                   . Active instances: 1000
                                        1   0.00%:  10 --  20    0.14% <  20                                  100% < 651 ms                                                          . Planned requests: 0 for 0:00:46
                                        1   0.00%:   9 --  10    0.14% <  10                                   99% < 40 ms                                                           . Actual responses: 28466
                                        2   0.00%:   8 --   9    0.14% <   9                                   98% < 40 ms                                                           .         Accuracy: 96.44%
                                        2   0.00%:   7 --   8    0.14% <   8                                   95% < 39 ms                                                           .         Time lag: 0:00:02
                                        7   0.00%:   6 --   7    0.14% <   7                                   90% < 38 ms                                                           . 
                                       31   0.00%:   5 --   6    0.14% <   6                                   85% < 37 ms                                                           . 
                                      142   0.01%:   4 --   5    0.14% <   5                                   80% < 36 ms                                                           . 
                                      321   0.02%:   3 --   4    0.13% <   4                                   75% < 35 ms                                                           . 
                                      405   0.03%:   2 --   3    0.11% <   3                                   50% < 32 ms                                                           . 
                                      374   0.03%:   1 --   2    0.08% <   2                                   25% < 30 ms                                                           . 
                                      682   0.05%:   0 --   1    0.05% <   1                                                                                                         . 
                                                                                                           Request/Response Sizes:                                                   . 
                                 1371318 100.00%: Total                                                       Avg Request at 0 RPS: 63 bytes                                         . 
                                                                                                             Avg Response at 0 RPS: 79 bytes                                         . 
                                                                                    
 * */
class accept_thread
{
  /*
  typedef ::boost::asio::ip::tcp::socket socket_type;*/
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;

public:

  accept_thread(const accept_thread&) = delete;
  accept_thread& operator=(const accept_thread&) = delete;
  
  ~accept_thread()
  {
    //this->stop();
  }
  
  accept_thread(::boost::asio::io_service& io)
    : _work(io)
    , _thread_io(nullptr)
    //, _socket(io)
    , _acceptor()
  {
  }
  
  
  
  void stop()
  {
    if ( _thread_io!=nullptr) 
      this->_thread_io->stop();
    _thread.join();
  }
  
  template<typename T>
  void start(T& t, std::weak_ptr<acceptor_type> acceptor)
  {
    auto acc = acceptor.lock();
    if (!acc)
      return;
    
    // typedef typename T::connection_type connection_type;
    
    acceptor_type::endpoint_type endpoint = acc->local_endpoint();
    acceptor_type::native_type fd = ::dup( acc->native() );

    _thread = std::thread([this, &t, endpoint, fd]()
    {
      ::boost::asio::io_service io_service;
      this->_thread_io = &io_service;
      acceptor_type acceptor(io_service);
      // socket_type socket( io_service );
      acceptor.assign(endpoint.protocol(), fd);
      
      accept_one one(io_service, acceptor/*,*/ /*socket,*/ /*t.get_aspect().template get<_connection_manager_>()*/ );
      one.do_accept(t);
      io_service.run();
      this->_thread_io = nullptr;
    }); // thread
  }

  /*
  
  template<typename T>
  void start1(T& t, std::weak_ptr<acceptor_type> acceptor)
  {
    _acceptor = acceptor;
    typedef typename T::connection_type connection_type;
    
    _thread = std::thread([this, &t]()
    {
      connection_manager mgr;
      
      for(;;)
      {
        auto acceptor = this->_acceptor.lock();
        if ( !acceptor )
          break;
        
        
        boost::system::error_code ec;
        acceptor->accept( this->_socket, ec);
        
        if (!acceptor->is_open())
          break;
        
        if (!ec)
        {
          t.get_aspect().template get<_socket_>()(t, this->_socket); 
          t.get_aspect().template get<_worker_>()(t, std::move( this->_socket), [&t, &mgr](socket_type sock) 
          {
            std::shared_ptr<connection_type> pconn = t.create_connection();
            mgr.insert(pconn);
            pconn->start( std::move( sock ), [&mgr](std::shared_ptr<connection_type> pconn)->void
            {
              mgr.erase(pconn);
            });
          });
        }
        else
          std::cout << "thread accept error: " << ec.message() << std::endl;
          
      }
    });
    //sleep(1);
  }
  */

private:  
  ::boost::asio::io_service::work _work;
  ::boost::asio::io_service* _thread_io;
  //socket_type _socket;
  std::weak_ptr<acceptor_type> _acceptor;
  //std::shared_ptr<acceptor_type> _acceptor;
  std::thread _thread;
};

  
}}
