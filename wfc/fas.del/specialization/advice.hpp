//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wfc{ 

/** Специализация для обозначения класса-адвайса без тега.
  * Всегда создается уникальный тег
  * @param A - класс-адвайс
  * @typedef tags_list - список тегов
  * @typedef advice_class - класс-адвайс 
  */
template<typename A, bool B>
struct advice<A, empty_type, B>
  : type2type<A>, A
{
  typedef typename tag_list_traits< type2type<A> >::type all_tag_list;
  typedef typename tag_list_traits< type2type<A> >::tag_list tag_list;
  typedef typename tag_list_traits< type2type<A> >::gtag_list gtag_list;

  typedef A advice_class;
  enum { make_advice_tag = B};

  template< typename LL /*= empty_type*/, typename AA /*= A*/, bool BB /*= B*/>
  struct rebind
  {
    typedef typename detail::rebind_advice<LL, AA, BB>::type type;
  };

  advice_class& get_class() { return static_cast<advice_class&>(*this);}
  const advice_class& get_class() const { return static_cast<const advice_class&>(*this);}

};


template<typename L, typename A >
struct advice<L, A, true>
  : tag_list_traits< type_list< type2type<A> , L > >::hierarchy, A
{
  typedef typename tag_list_traits< L >::type all_tag_list;
  typedef typename tag_list_traits< L >::tag_list tag_list;
  typedef typename tag_list_traits< L >::gtag_list gtag_list;

  typedef A advice_class;
  enum { make_advice_tag = true};

  template< typename LL /*= L*/, typename AA /*= A*/, bool BB /*= true*/>
  struct rebind
  {
    typedef typename detail::rebind_advice<LL, AA, BB>::type type;
  };

  advice_class& get_class() { return static_cast<advice_class&>(*this);}
  const advice_class& get_class() const { return static_cast<const advice_class&>(*this);}
};

/// Не можем создавать адвайс без тегов
template< typename A >
struct advice<empty_type, A, false>;


template<>
struct advice_list_traits<empty_type>
{
  typedef empty_type type;
  typedef empty_type common_list;
};

}

