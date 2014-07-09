
#include <wfc/jsonrpc/service/worker_manager.hpp>

namespace wfc{ namespace jsonrpc{



worker_manager::worker_manager(wfc::io_service& io_service, const options_type& config)
  : _io_service(io_service)
  , _config(config)
{
  if ( _config.workers.empty() )
    _config.workers.push_back(options::worker());
}

void worker_manager::start()
{
  this->stop();
  lock_guard lk(_mutex);
  for (auto &w : _config.workers)
  {
    io_service_ptr io_ptr = nullptr;
    if (w.threads != 0)
    {
      io_ptr = std::make_shared< ::wfc::io_service>();
      _services.push_back(io_ptr);
    }
    
    ::wfc::io_service& io_ref = (io_ptr == nullptr ? _io_service : *io_ptr );
    
    for (auto s: w.strands)
    {
      auto opt = static_cast< typename worker_type::options_type >(_config);
      
      for (int i = 0; i < s.count; ++i)
      {
        auto wrk = std::make_shared<worker_type>( io_ref, opt );
        _workers.push_back( wrk );
        for (auto& m : s.methods)
        {
          auto itr = _method_map.find(m);
          if ( itr == _method_map.end() )
          {
            pair_worker_list pwl;
            itr = _method_map.insert( std::make_pair(m, std::move(pwl) ) ).first;
          }
          itr->second.first.push_back(wrk);
          if ( itr->second.first.size() == 1)
            itr->second.second.init( itr->second.first.begin() );
        }
      }
    }
    
    for (int i=0; i < w.threads; ++i)
    {
      _threads.push_back( std::thread([io_ptr]() {
        ::wfc::io_service::work wrk(*io_ptr);
        io_ptr->run();
      }));
    }
  }
}

void worker_manager::stop()
{
  lock_guard lk(_mutex);
  
  for (auto& s: this->_services)
    s->stop();
    
  for (auto& t: this->_threads)
    t.join();
    
  this->_workers.clear();
  this->_threads.clear();
  this->_method_map.clear();
  this->_services.clear();
}

std::weak_ptr< worker_manager::worker_type > 
worker_manager::get_worker(const char* name) const
{
  read_lock lk(_mutex);
  
  auto itr = _method_map.find( name );
  if ( itr == _method_map.end() )
  {
    itr = _method_map.find( "*" );
  }

  if ( itr!=_method_map.end() )
  {
    auto wrk_itr =  itr->second.second.inc(
      itr->second.first.begin(),
      itr->second.first.end()
    );
    return *wrk_itr;
  }
  return std::weak_ptr< worker_manager::worker_type >();
}

}} // wfc


