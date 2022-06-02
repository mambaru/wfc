#pragma once

#include <wfc/tank/igun.hpp>
#include <wfc/iinterface.hpp>
#include <iow/io/io_id.hpp>
#include <wflow/owner.hpp>
#include <functional>
#include <iostream>

namespace wfc{

template<typename I, typename Opt>
class tank_gun
  : public igun<I, Opt>
  , public std::enable_shared_from_this< tank_gun<I,Opt> >
  
{
public:
  typedef igun<I, Opt> super;

  typedef typename super::interface_type target_interface;
  typedef typename super::interface_ptr target_ptr;
  typedef typename super::options_type options_type;
  typedef typename super::request_type request_type;
  typedef typename super::response_type response_type;
  typedef typename super::user_options user_options;
  typedef typename super::bang_handler bang_handler;

  typedef std::unique_ptr<request_type> request_ptr;
  typedef std::unique_ptr<response_type> response_ptr;
  typedef std::function< void(response_ptr) >  handler_t;
  typedef typename fas::if_<
    fas::same_type<I, iinterface>,
    void (iinterface::*)(request_ptr, iinterface::io_id_t, handler_t),
    void (I::*)(request_ptr, handler_t)
  >::type target_method;


  typedef std::function<bool(request_type&, 
                             response_type&)> request_generator1;
  typedef std::function<bool(const user_options&, 
                             request_type&, 
                             response_type&)> request_generator2;

  typedef typename fas::if_<
    fas::same_type<user_options, fas::empty_type>,
    request_generator1,
    request_generator2
  >::type request_generator;

  typedef std::function<bool(const request_type&, 
                             const response_type&, 
                             const response_type&)> response_checker1;
                             
  typedef std::function<bool(const user_options&, 
                             const request_type&, 
                             const response_type&, 
                             const response_type&)> response_checker2;

  typedef typename fas::if_<
    fas::same_type<user_options, fas::empty_type>,
    response_checker1,
    response_checker2
  >::type response_checker;

  tank_gun(
    wflow::owner& own,
    target_method tm,
    request_generator req_gen,
    response_checker res_check
  )
    : _id(::iow::io::create_id<iinterface::io_id_t>())
    , _owner(own)
    , _target_method(tm)
    , _req_gen(req_gen)
    , _res_check(res_check)
  {
    _is_abort = false;
  }

  virtual void stop( ) override
  {
    if ( _options.reg_io && _target!=nullptr)
      _target->unreg_io(_id);
  }

  virtual void initialize( target_ptr target, const options_type& opt) override
  {
    _target = target;
    _options = opt;

    if ( opt.reg_io && _target!=nullptr)
      _target->reg_io(_id, this->shared_from_this() );
  }
  
  virtual void perform_io(iinterface::data_ptr d, iinterface::io_id_t, iinterface::output_handler_t ) override
  {
    if (_is_abort)
      return;
    this->perform_check_<>( 
      std::make_shared<iinterface::data_type>(_options.request.begin(), _options.request.end() ),
      std::make_shared<iinterface::data_type>(_options.response.begin(), _options.response.end() ),
      std::move(d), 
      fas::bool_< fas::same_type<response_type, iinterface::data_type>::value >()
    );
  }

  virtual bool bang(bang_handler handler) override
  {
    if (_is_abort)
      return false;
    auto req = std::make_shared<request_type>(_options.request);
    auto res = std::make_shared<response_type>(_options.response);
    typedef fas::bool_< fas::same_type<user_options, fas::empty_type>::value > no_user_options;
    if ( !req_gen_( static_cast<const user_options&>(_options), *req,  *res, no_user_options() ) )
      return false;

    return this->bang_<no_user_options>(std::move(req), std::move(res), handler);
  }

private:
  
  template<typename UO, typename Req, typename Res>
  bool bang_(Req req, Res res, bang_handler handler)
  {
    (_target.get()->*_target_method)(std::make_unique<request_type>(*req), 
                                     _owner.callback([req, res, handler, this](response_ptr res2)
    {
      bool bad_gateway = (res2 == nullptr);
      bool check_result = true;
      if ( !bad_gateway )
        check_result = this->res_check_( static_cast<const user_options&>(this->_options), *req, *res, *res2, UO());
      else if ( _options.abort_if_bad_gateway )
      {
        _is_abort = true;
        wfc_abort("The tank is shutting down because it got a bad gateway result.");
      }
      handler( !bad_gateway, check_result );
    }));
    return true;
  }

  
  template<typename UO>
  bool bang_(
    std::shared_ptr<iinterface::data_type> req, 
    std::shared_ptr<iinterface::data_type> res, 
    bang_handler handler)
  {
    if ( _io_handler == nullptr )
      _io_handler = handler;
    
    using namespace std::placeholders;
    typedef std::function<void(iinterface::data_ptr)> perform_handler_f;
    perform_handler_f perform_handler = _owner.wrap(std::bind(
      &tank_gun<I,Opt>::io_check_, this, 
      req, res, _1), nullptr);
    _target->perform_io(std::make_unique<iinterface::data_type>(*req), _id, std::move(perform_handler));    
    return true;
  }

  template<typename UserOptions>
  bool req_gen_(const UserOptions&, request_type& req, response_type& res, fas::true_)
  {
    if ( _req_gen == nullptr ) return true;
    return _req_gen(req, res);
  }

  template<typename UserOptions>
  bool req_gen_(const UserOptions& uopt, request_type& req, response_type& res, fas::false_)
  {
    if ( _req_gen == nullptr ) return true;
    return _req_gen(uopt, req, res);
  }

  template<typename UserOptions>
  bool res_check_(const UserOptions&, const request_type& req, const response_type& res1, const response_type& res2, fas::true_)
  {
    if ( _res_check == nullptr ) return true;
    return _res_check(req, res1, res2);
  }

  template<typename UserOptions>
  bool res_check_(const UserOptions& uopt, const request_type& req, const response_type& res1, const response_type& res2, fas::false_)
  {
    if ( _res_check == nullptr ) return true;
    return _res_check(uopt, req, res1, res2);
  }


  void io_check_(std::shared_ptr<iinterface::data_type> req, 
                      std::shared_ptr<iinterface::data_type> res1,
                      iinterface::data_ptr res2)
  {
    if ( _io_handler != nullptr )
    {
      typedef fas::bool_< fas::same_type<user_options, fas::empty_type>::value > no_user_options;
      bool bad_gateway = (res2 == nullptr);
      bool check_result = true;
      if ( !bad_gateway )
        check_result = this->res_check_<user_options>( static_cast<const user_options&>(this->_options), 
                                *req, *res1, *res2, no_user_options());
      else if ( _options.abort_if_bad_gateway )
      {
        _is_abort = true;
        wfc_abort("The tank is shutting down because it got a bad gateway result.");
      }
      _io_handler(!bad_gateway, check_result );
    }
  }

  template<typename = void>
  void perform_check_(std::shared_ptr<iinterface::data_type> req, 
                      std::shared_ptr<iinterface::data_type> res1,
                      iinterface::data_ptr res2,
                      fas::true_)
  {
    
    io_check_(req, res1, std::move(res2));
  }

  template<typename = void>
  void perform_check_(std::shared_ptr<iinterface::data_type>, std::shared_ptr<iinterface::data_type>,  iinterface::data_ptr, fas::false_)
  {
  }


private:
  iinterface::io_id_t _id = 0;
  wflow::owner& _owner;
  target_ptr _target;
  target_method _target_method;
  options_type _options;
  request_generator _req_gen;
  response_checker _res_check;
  bang_handler _io_handler;
  std::atomic_bool _is_abort;
};




}
