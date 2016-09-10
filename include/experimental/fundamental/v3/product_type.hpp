//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef JASEL_FUNDAMENTAL_V3_PRODUCT_TYPE_HPP
#define JASEL_FUNDAMENTAL_V3_PRODUCT_TYPE_HPP

///////////////////////////////////////////////////////////////////////////////////////
///
/// A type PT is a model of a ProductType if given a variables pt of type PT
///
///   product_type::size<PT>::value
///   product_type::element<I,PT>::type
///   product_type::get<I>(pt)
///
/// The definition of these traits/functions depend on whether P0327 is adopted by the standard and
/// can be implemented only by the compiler, as it does for Range-based for loops.
///
/// What follows is just an emulation for the cases 1 (c-arrays) and 2 (tuple-like types).
/// The case 3 can be implemented only by the compiler.
///////////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace std
{
namespace experimental
{
inline namespace fundamental_v3
{
  template <class T>
  struct remove_cvr {
    using type = remove_cv_t<remove_reference_t<T>>;
  };
  template <class T>
  using remove_cvr_t = typename remove_cvr<T>::type;

namespace detail
{
  template <typename T>
  struct has_tuple_like_size_access
  {
      struct yes{ char a[1]; };
      struct no { char a[2]; };

      template<typename U> static yes test(decltype(std::tuple_size<U>::value)*);
      template<typename U> static no test(...);
      static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
  };

  template <size_t N, typename T>
  struct has_tuple_like_element_access
  {
      struct yes{ char a[1]; };
      struct no { char a[2]; };

      template <size_t I, typename U> static yes test(typename std::tuple_element<I,U>::type*);
      template <size_t I, typename U> static no test(...);
      static const bool value = sizeof(test<N, T>(nullptr)) == sizeof(yes);
  };
  namespace get_adl {
    using std::get;
    template <size_t N, class T>
    auto xget(T&& t) {
      return get<N>(forward<T>(t));
    }
  }
  template<size_t N, typename T>
  struct has_tuple_like_get_access
  {
      struct yes{ char a[1]; };
      struct no { char a[2]; };

      template <size_t I, typename U>
          static yes test(decltype(detail::get_adl::xget<I>(declval<U>()))*);
      template <size_t I, typename U>
          static no test(...);
      static constexpr bool value = sizeof(test<N,T>(nullptr)) == sizeof(yes);
  };
  template <size_t I, size_t N, class T>
  struct has_tuple_like_get_access<I, T [N]> : false_type {};

  template <typename T, typename  Indexes>
  struct has_tuple_like_element_get_access_aux;
  template <typename T, size_t ...N>
  struct has_tuple_like_element_get_access_aux<T, index_sequence<N...>> : integral_constant<bool,
    (has_tuple_like_element_access<N, T>::value && ...)
    &&
    (has_tuple_like_get_access<N, T>::value && ...)
    > {};

  template <typename T>
  struct has_tuple_like_element_get_access:
      has_tuple_like_element_get_access_aux<T, make_index_sequence<tuple_size<T>::value>>
  {};

}

  template <typename T>
  struct has_tuple_like_access : integral_constant<bool,
    detail::has_tuple_like_size_access<T>::value &&
    detail::has_tuple_like_element_get_access<T>::value
    > {};

  template <size_t N, class T>
  struct has_tuple_like_access<T [N]> : false_type {};
  template <size_t N, class T>
  struct has_tuple_like_access<T (&)[N]> : false_type {};

  template <class PT, typename = void>
  struct product_type_traits;

  struct product_type_tag{};

  // Forward to customized class using tuple-like access
  template <class PT>
  struct product_type_traits
  <  PT, enable_if_t< has_tuple_like_access<PT>::value >  > : product_type_tag
  {
    using size = tuple_size<PT>;

    template <size_t I>
    using element = tuple_element<I, PT>;

    template <size_t I, class PT2, typename= std::enable_if_t< I < size::value > >
      static constexpr decltype(auto) get(PT2&& pt) noexcept
      {
        return detail::get_adl::xget<I>(forward<PT2>(pt));
      }
  };

  // customization for C-arrays
  template <class T, size_t N>
  struct product_type_traits<T [N]> : product_type_tag
  {
    using size = integral_constant<size_t, N>;
    template <size_t I>
    struct element { using type = T; };

    template <size_t I, class U, size_t M, typename= std::enable_if_t< I<N >>
        static constexpr U& get(U (&arr)[M]) noexcept { return arr[I]; }
  };
  template <class T, size_t N>
  struct product_type_traits<T (&)[N]> : product_type_traits<T [N]> {};

  namespace product_type {
    template <class PT>
    struct size : product_type_traits<PT>::size {};
    template <class T> struct size<const T> : size<T> {};
    template <class T> struct size<volatile T> : size<T> {};
    template <class T> struct size<const volatile T> : size<T> {};
    template <class PT>
    constexpr size_t size_v = size<PT>::value;

    template <size_t I, class PT, typename= std::enable_if_t< I<size_v<PT> >>
    struct element : product_type_traits<PT>::template element<I> {};
    template <size_t I, class PT>
    using element_t = typename element<I,PT>::type;
    template <size_t I, class T> struct element<I, const T> { using type = const element_t<I, T>; };
    template <size_t I, class T> struct element<I, volatile T> { using type = volatile element_t<I, T>; };
    template <size_t I, class T> struct element<I, const volatile T> { using type = const volatile element_t<I, T>; };


    template <size_t I, class PT
      , typename= std::enable_if_t< I < size_v<remove_cvr_t<PT>> >
    >
    constexpr decltype(auto) get(PT && pt) noexcept
    {
        return product_type_traits<remove_cvr_t<PT>>::template get<I>(forward<PT>(pt));
    }

  }
  template <typename T>
  struct is_product_type : is_base_of<product_type_tag, product_type_traits<T>> {};
  template <typename T>
  struct is_product_type<const T> : is_product_type<T> {};
  template <typename T>
  struct is_product_type<volatile T> : is_product_type<T> {};
  template <typename T>
  struct is_product_type<const volatile T> : is_product_type<T> {};
  template <typename T>
  constexpr bool is_product_type_v = is_product_type<T>::value ;

}}
}
#endif // header