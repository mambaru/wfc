//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/fas/type_list.hpp>
#include <wfc/fas/tag.hpp>

namespace wfc{

template< typename L, typename A = empty_type, bool B = false >
struct advice;

template<typename T, typename L>
struct alias;

template<typename T>
struct advice_traits;

template<typename T>
struct advice_list_traits;

template<typename T>
struct extract_advice_tags;

template<typename T>
struct apply_alias;


}

#include <wfc/fas/detail/advice.hpp>

namespace wfc{


/** Класс используется для обозначения класса-адвайса и "помечает"
  * его одним или несколькими тегами. Как правило, используеться
  * в качестве дного из элементов аспекта.
  * @param L - тег или список тегов
  * @param A - класс-адвайс
  * @param bool - флаг создания дополнительного тега
  * @typedef tag_list - список тегов
  * @typedef advice_class - класс-адвайс 
  */
template< typename L, typename A, bool B >
struct advice: tag_list_traits< L >::hierarchy, A
{
  typedef A advice_class;

  typedef typename tag_list_traits< L >::type all_tag_list;
  typedef typename tag_list_traits< L >::tag_list tag_list;
  typedef typename tag_list_traits< L >::gtag_list gtag_list;

  enum { make_advice_tag = B};

  /** Перестраивает адвайс с новым списком тегов. Если список 
    * тегов пустой (LL == empty_type), то убивает адвайс 
    * ( type = empty_type)
    * @typedef type - адвайс с новым списком тегов или empty_type */
  template< typename LL /*= L*/, typename AA /*= A*/, bool BB /*= B */>
  struct rebind
  {
    typedef typename detail::rebind_advice<LL, AA, BB>::type type;
  };

  advice_class& get_class() { return static_cast<advice_class&>(*this);}
  const advice_class& get_class() const { return static_cast<const advice_class&>(*this);}
};

template<typename T, typename L>
struct alias
{
  typedef typename tag_traits<T>::type tag;
  typedef typename tag_list_traits<L>::type tag_list;
};

/** Извлекает список тегов из списка адвайсов. Каждый адвайс должен 
  * предоставлять тип tag_list
  * @param T - список типов адвайсов
  * @typedef type - извлеченный список типов тегов
  */
template<typename T>
struct extract_advice_tags
{
  typedef typename organize< typename detail::extract_tags_helper<T>::tag_list >::type tag_list;
  typedef typename organize< typename detail::extract_tags_helper<T>::gtag_list >::type gtag_list;
  typedef typename organize< typename detail::extract_tags_helper<T>::all_tag_list >::type all_tag_list;
};


template<typename T>
struct advice_list_traits_helper
{
private:
  typedef T advice_list;

  // Извлекаем все теги (кроме gtag) всех адвайсов из списка T
  typedef typename extract_advice_tags< advice_list >::tag_list/*type*/ tag_list;

  // текущий адвайс
  typedef typename advice_list::left_type current_advice;

  // удаляем дубликаты тегов
  typedef erase_from_list<
            typename current_advice::all_tag_list,
            tag_list,
            1
          > erased;

  // перестраиваем адвайс
  typedef typename current_advice::template rebind<
            typename erased::type,
            typename current_advice::advice_class,
            current_advice::make_advice_tag
          >::type rebinded;

  typedef type_list<
            rebinded,
            typename advice_list_traits_helper<
              typename advice_list::right_type
            >::type
          > rebinded_list;

public:
  typedef typename organize<rebinded_list>::type type;
};

template<>
struct advice_list_traits_helper<empty_type>
{
  typedef empty_type type;
};

/** Перестраивает адвайсы таким образом, чтобы у всех 
  * адвайсов были уникальные теги (удаляет дубликаты тегов).
  * Дубликаты тегов удаляются у адвайсов, которые находяться 
  * в начале списков. Если у адвайса удалены все теги (он 
  * имеет пустой список тегов), то адвайс удаляеться из списка.
  * @param T - список адвайсов или адвайс
  * @typedef type - список адвайсов с уникальными тегами
  */
template<typename T>
struct advice_list_traits
{
public:

  typedef typename type_list_traits<T>::type common_list;

  ///--
  typedef typename apply_alias< common_list >::type advice_list;

  typedef typename advice_list_traits_helper< advice_list >::type type;
  
};


template<typename T>
struct apply_alias
{
  /// Извлекатель из общего списка, списка адвайсов и алиасов 
  typedef detail::extract_alias_list< typename type_list_traits<T>::type > exractor;
  typedef typename exractor::alias_list alias_list;
  typedef typename exractor::advice_list advice_list;

  /// а теперь применим все алиасы к адвайсам - наверное удаение лучше здесь сделать
  typedef typename detail::apply_alias_helper< advice_list, alias_list >::type type1;
  typedef typename type_list_traits<type1>::type type;
};


}

#include <wfc/fas/specialization/advice.hpp>
