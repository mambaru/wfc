#pragma once

#include <wfc/io/reader/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace reader{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class reader
  : public io_base<A, AspectClass>
  , public std::enable_shared_from_this< reader<A, AspectClass> >
{
public:
  
  typedef reader<A, AspectClass> self;
  typedef io_base<A, AspectClass> super;
  typedef typename super::config_type config_type;
  typedef typename super::init_type   init_type;
  
  typedef typename super::aspect::template advice_cast<_data_type_>::type data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
  /*
  typedef typename super::context_type context_type;
  typedef typename super::aspect::advice_cast<_data_type_>::type data_type;
  typedef typename super::aspect::advice_cast<_config_type_>::type config_type;
  typedef typename super::aspect::advice_cast<_init_type_>::type init_type;
  typedef std::unique_ptr<data_type> data_ptr;
  */
  
public:
  
  reader() 
  {
    super::create(*this);
  };

  reader(const config_type& conf) 
  {
    super::create(*this, conf);
  };

  reader(const init_type& conf) 
  {
    super::create(*this, conf);
  };

  reader(const config_type& conf, const init_type& init) 
  {
    super::create(*this, conf, init);
  };
  
  void configure(const config_type& conf)
  {
    super::configure(*this, conf);
  }

  void initialize(const init_type& init)
  {
    super::initialize(*this, init);
  }

  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }

  template<typename Callback>
  void async_read(Callback callback = nullptr/*std::function<void(data_ptr)>*/)
  {
    this->get_aspect().template get<_async_read_>().call(*this, callback);
  }
  
  data_ptr read()
  {
    return this->get_aspect().template get<_read_>()(*this);
  }

private:

};

}}}
