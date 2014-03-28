//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

namespace wfc{ namespace detail {

template<typename L>
struct extract_aspect_tags_helper
{
  typedef typename L::left_type advice_type;

  typedef type_list<
    typename extract_advice_tags< advice_type >::tag_list,
    typename extract_aspect_tags_helper< typename L::rigth_type>::tag_list
  > tag_list;

  typedef type_list<
    typename extract_advice_tags< advice_type >::gtag_list,
    typename extract_aspect_tags_helper< typename L::rigth_type>::gtag_list
  > gtag_list;

  typedef type_list<
    typename extract_advice_tags< advice_type >::all_tag_list,
    typename extract_aspect_tags_helper< typename L::rigth_type>::all_tag_list
  > all_tag_list;
};

template<>
struct extract_aspect_tags_helper< empty_type >
{
  typedef empty_type tag_list;
  typedef empty_type gtag_list;
  typedef empty_type all_tag_list;
};

}}

