//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace mamba{ namespace comet{

template<typename T>
struct type_count<T, empty_type>
{
  enum { result = 0};
};

template<>
struct type_position<empty_type, empty_type >
{
  enum { result = 0 } ;
};

/// /////////////////////////////////////////////////////////////////////////

template<typename T>
struct erase<T, empty_type >
{
  typedef empty_type type;
};

template<typename L>
struct erase<empty_type, L>
{
  typedef L type;
};


/// /////////////////////////////////////////////////////////////////////////


template<typename T>
struct erase_all<T, empty_type >
{
  typedef empty_type type;
  typedef empty_type removed;
};

template<typename L>
struct erase_all<empty_type, L>
{
  typedef L type;
  typedef empty_type removed;
};


/// /////////////////////////////////////////////////////////////////////////

template< typename S, int C>
struct erase_from_list<empty_type, S, C>
{
  typedef empty_type type;
};

template< typename F, int C>
struct erase_from_list<F, empty_type, C>
{
  typedef F type;
};

template< int C>
struct erase_from_list<empty_type, empty_type, C>
{
  typedef empty_type type;
};


/// /////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////

template<>
struct organize< empty_type >
{
  typedef empty_type type;
};

template<>
struct unique< empty_type >
{
  typedef empty_type type;
};


template<typename L1>
struct unique_from_list<L1, empty_type>
{
  typedef L1 type;
};

template<typename T, typename L1, typename R1>
struct unique_from_list< T, type_list<L1, R1> >
{
  typedef typename detail::unique_from_list_helper<
            L1,
            T,
            type_count<L1, T>::result >= 2
          >::type L1_unique;

  typedef typename unique_from_list< L1_unique, R1>::type type;
};

/// /////////////////////////////////////////////////////////////////////////

template<typename T>
struct type_list_cast< T, empty_type >
{
  typedef empty_type type;
};

/// //////////////////////////////////////////

}}

