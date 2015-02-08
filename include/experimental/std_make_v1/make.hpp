// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Copyright (C) 2014-2015 Vicente J. Botet Escriba

#ifndef VIBOES_STD_EXPERIMENTAL_FUNDAMENTALS_V2_MAKE_HPP
#define VIBOES_STD_EXPERIMENTAL_FUNDAMENTALS_V2_MAKE_HPP

#include <utility>
#include <type_traits>

#if defined VIBOES_STD_EXPERIMENTAL_FACTORIES_USE_OPTIONAL
#include <optional.hpp>
#endif

namespace std
{
#if __cplusplus < 201103L
  template <class T>
  using decay_t = typename decay<T>::type;

  template <class T>
  using remove_reference_t = typename remove_reference<T>::type;

#endif

namespace experimental
{
#if ! defined VIBOES_STD_EXPERIMENTAL_FACTORIES_USE_OPTIONAL
  constexpr struct in_place_t{} in_place{};
#endif

inline namespace fundamental_v2
{

  // identity meta-function
  template<class T>
  struct identity
  {
    typedef T type;
  };

  // tag type
  template <class T>
  struct type {
    using underlying_type = T;
  };

  // holder type
  struct _t {};

  namespace detail {
    template <class TC, class ...U>
    struct has_apply
    {
    private:
        struct two {char a; char b;};
        template <class X> static two test(...);
        template <class X> static char test(typename X::template apply<U...>* = 0);
    public:
        static const bool value = sizeof(test<TC>(0)) == 1;
    };

    template <class T>
    struct deduced_type_impl
    {
        typedef T type;
    };

    template <class T>
    struct deduced_type_impl<reference_wrapper<T>>
    {
        typedef T& type;
    };

  }

  template <class T>
  struct deduced_type
  {
      typedef typename detail::deduced_type_impl<decay_t<T>>::type type;
  };

  template <class T>
  using deduced_type_t = typename deduced_type<T>::type;

  template <class TC, class U >
  struct is_type_constructor_for : detail::has_apply<TC, U> {};


  // apply a type constuctor TC to the type parameters Args
  template<class TC, class... Args>
  using apply = typename TC::template apply<Args...>;

#ifdef VIBOES_STD_EXPERIMENTAL_FUNDAMENTALS_V2_MAKE_TYPE_CONSTRUCTOR
  // type constructor customization point.
  // Default implementation make use of a nested type type_constructor
  template <class M >
  struct type_constructor : identity<typename  M::type_constructor> {};

  // type constructor getter meta-function
  template <class M >
  using type_constructor_t = typename type_constructor<M>::type;

  // rebinds a type having a underlying type with another underlying type
  template <class M, class ...U>
  using rebind = apply<type_constructor_t<M>, U...>;
#endif

  // transforms a template class into a type_constructor that adds the parameter at the end
  template <template <class ...> class TC, class... Args>
  struct lift
  {
    template<class... Args2>
    using apply = TC<Args..., Args2...>;
  };

  // transforms a template class into a type_constructor that adds the parameter at the begining
  template <template <class ...> class TC, class... Args>
  struct reverse_lift
  {
    template<class... Args2>
    using apply = TC<Args2..., Args...>;
  };

  template <template <class ...> class A>
  using lift_tc = A<_t>;

  //  // undelying_type customization point.
  //  // Default implementation make use of a nested type underlying_type
  //  template <class M >
  //  struct underlying_type : identity<typename  M::underlying_type> {};
  //
  //  // underlying_type getter meta-function
  //  template <class M >
  //  using underlying_type_t = typename underlying_type<M>::type;

  template <template <class ...> class TC>
  auto none()
  {
    return none_ovl(type<lift_tc<TC>>{});
  }

  template <class TC>
  auto none() {
    return none_ovl(type<TC>{});
  }


  // make() overload
  template <template <class ...> class M>
  M<void> make()
  {
    return make(type<M<void>>{});
  }

  // make overload: requires a template class, deduce the underlying type
  template <template <class ...> class M, int = 0, int..., class ...X>
  M<deduced_type_t<X>...>
  make(X&& ...x)
  {
    return make(type<M<deduced_type_t<X>...>>{}, std::forward<X>(x)...);
  }

  // make overload: requires a type construcor, deduce the underlying type
  template <class TC, int = 0, int..., class ...X>
  typename enable_if<detail::has_apply<TC, deduced_type_t<X>...>::value,
    apply<TC, deduced_type_t<X>...>
  >::type
  make(X&& ...x)
  {
    return make(type<apply<TC, deduced_type_t<X>...>>{}, std::forward<X>(x)...);
  }

  // make overload: requires a type with a specific underlying type, don't deduce the underlying type from X
  template <class M, int = 0, int..., class X>
  typename enable_if<   ! detail::has_apply<M, deduced_type_t<X>>::value
                     //&&   is_same<X, underlying_type_t<M>>::value
  , M
  >::type
  make(X&& x)
  {
    return make(type<M>{}, std::forward<X>(x));
  }

  // make emplace overload: requires a type with a specific underlying type, don't deduce the underlying type from X
  template <class M, class ...Args>
  typename enable_if<
    ! detail::has_apply<M, deduced_type_t<Args>...>::value, M
  >::type
  make(Args&& ...args)
  {
    return make(type<M>{}, in_place, std::forward<Args>(args)...);
  }

  // default customization point for TC<void> default constructor
  template <class M>
  typename enable_if<std::is_default_constructible<M>::value,  M>::type
  make(type<M>)
  {
    return M();
  }

  // default customization point for constructor from X
  template <class M, class ...X>
  typename enable_if<std::is_constructible<M, deduced_type_t<X>...>::value,  M>::type
  make(type<M>, X&& ...x)
  {
    return M(std::forward<deduced_type_t<X>>(x)...);
  }

  // default customization point for in_place constructor
  template <class M, class ...Args>
  typename enable_if<std::is_constructible<M, in_place_t, deduced_type_t<Args>...>::value,  M>::type
  make(type<M>, in_place_t, Args&& ...args)
  {
    return M(in_place, std::forward<Args>(args)...);
  }

}
}
}

#endif // VIBOES_STD_EXPERIMENTAL_FUNDAMENTALS_V2_MAKE_HPP
