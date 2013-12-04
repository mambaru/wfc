//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <comet/fas/type_list.hpp>

namespace mamba{ namespace comet{

/** Простой тег */
template<typename T>
struct tag;

/** Групирующий тег*/
template<typename T>
struct gtag;

/** Групирующий нумерованый тег*/
template<int I, typename T>
struct ntag;

template<typename T>
struct tag_traits;

template<typename T>
struct make_tag_list;

template< typename T>
struct tag_hierarchy;

template<typename T>
struct tag_list_traits;

template< typename T0 = empty_type, typename T1 = empty_type, typename T2 = empty_type,
          typename T3 = empty_type, typename T4 = empty_type, typename T5 = empty_type,
          typename T6 = empty_type, typename T7 = empty_type, typename T8 = empty_type,
          typename T9 = empty_type>
struct tag_list_n;

}}

namespace mamba{ namespace comet{

/** Создает тег путем наследования от заданного типа.
  * @param T - тип, на основе которого будет создан тег
  * @typedef type - тип, на основе которого был создан тег
  */
template<typename T>
struct tag: T
{
  typedef T type;
};

/** 
  * @param T - тип, на основе которого будет создан тег
  * @typedef type - тип, на основе которого был создан тег
  */
template<typename T>
struct gtag: T
{
  typedef T type;
};

/** 
  * @param T - тип, на основе которого будет создан тег
  * @typedef type - тип, на основе которого был создан тег
  */
template<int I, typename T>
struct ntag: T
{
  enum { number = I};
  typedef T type;
};


/**  */
template<typename T>
struct tag_traits
{
  typedef tag<T> type;
};

/** Создает список тегов на основе списка типов
  * @param T  - список типов
  * @typedef type - список тегов
  */
template<typename T>
struct make_tag_list
{
  typedef type_list< typename tag_traits<T>::type > type;
};

/** создает иерархию тегов, на основе списка тегов
  * @param T  - список тегов
  template< typename L, typename R>
struct tag_hierarchy< type_list< tag<L>, R> > : tag<L>, tag_hierarchy<R>{};

  */
template< typename T >
struct tag_hierarchy
  : T::left_type
  , tag_hierarchy<typename T::right_type>
{
};

template<typename T>
struct tag_list_traits
{
private:
  typedef typename type_list_traits< T >::type organize_list;
  typedef typename make_tag_list< organize_list >::type all_tags_list;
public:
  // все теги
  typedef all_tags_list type;
  // только теги gtag
  typedef typename select1<gtag, all_tags_list>::type gtag_list;

  typedef typename erase_from_list< type, gtag_list, 0 >::type whithout_gtag;
  // все теги кроме gtag
  typedef typename unique<whithout_gtag>::type tag_list;
  typedef tag_hierarchy<type> hierarchy;

};

template< typename T0, typename T1, typename T2,
          typename T3, typename T4, typename T5,
          typename T6, typename T7, typename T8,
          typename T9>
struct tag_list_traits< tag_list_n<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename tag_list_n<T0, T1, T2, T3, T4,
                              T5, T6, T7, T8, T9>::type tag_list_type;

  typedef typename tag_list_traits< tag_list_type >::type type;
  typedef typename tag_list_traits< tag_list_type >::tag_list tag_list;
  typedef typename tag_list_traits< tag_list_type >::gtag_list gtag_list;

  typedef tag_hierarchy<type> hierarchy;
};

}}

#include <comet/fas/specialization/tag.hpp>

