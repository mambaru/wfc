//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#include <comet/fas/type_list.hpp>

namespace mamba{ namespace comet{

template<typename T>
struct tag_traits< tag<T> >
{
  typedef tag<T> type;
};

template<typename T>
struct tag_traits< gtag<T> >
{
  typedef gtag<T> type;
};

template<int I, typename T>
struct tag_traits< ntag<I, T> >
{
  typedef ntag<I, T> type;
};

template<>
struct make_tag_list<empty_type>
{
  typedef empty_type type;
};

template<typename L, typename R>
struct make_tag_list< type_list< tag<L>, R> >
{
  typedef type_list< tag<L>, typename make_tag_list<R>::type > type;
};

template<typename L, typename R>
struct make_tag_list< type_list< gtag<L>, R> >
{
  typedef type_list< gtag<L>, typename make_tag_list<R>::type > type;
};

template<int I, typename L, typename R>
struct make_tag_list< type_list< ntag<I, L>, R> >
{
  typedef type_list< ntag<I, L>, typename make_tag_list<R>::type > type;
};

template<typename L, typename R>
struct make_tag_list< type_list< L, R> >
{
  typedef type_list< tag<L>, typename make_tag_list<R>::type > type;
};

template< typename T0, typename T1, typename T2,
          typename T3, typename T4, typename T5,
          typename T6, typename T7, typename T8,
          typename T9>
struct tag_list_n
{
  typedef typename make_tag_list< 
    typename type_list_n<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type
  >::type type;
};

template<>
struct tag_hierarchy< empty_type > {};

}}
