// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Copyright (C) 2017 Vicente J. Botet Escriba

#ifndef JASEL_FUNDAMENTAL_V3_STRONG_STRONG_COUNTER_HPP
#define JASEL_FUNDAMENTAL_V3_STRONG_STRONG_COUNTER_HPP

/**
 * strong_counter<Domain, Rep> is a generalization of std::chrono::duration<Rep, Period>
 *
 * Where Domain is a tag class that has a conversion function between Domains
 *
 * The idea is to be able to define std::chrono::duration<Rep, Period> as an alias of strong_counter<domain::duration<Period>, Rep>.
 *
 * In addition to the underlying() function it provides the count() member function.
 */
#if __cplusplus >= 201402L

#include <experimental/fundamental/v3/strong/tagged.hpp>
#include <experimental/fundamental/v3/strong/underlying_type.hpp>

#include <limits>
#include <functional>
#include <type_traits>

namespace std
{
namespace experimental
{
inline namespace fundamental_v3
{

  /**
  `strong_counter` is a strongly type tagged by the tag @c Domain that wraps an integral type @c UT and behaves like an @c UT
  The main goal is to be able to define strong integers that don't mix between them.
  No conversion to the underlying integer type is provided.

  Note that this is not a safe int, it just forbids the conversions between
  different strong integers types.

  Example
  <code>
  using X = strong_counter<XDomain, int>;
  using Y = strong_counter<YDomain, int>;
  void f(X, Y);

  f(X(1), Y(1));

  </code>
  */

  template <class Domain, class UT>
  struct strong_counter final : private_tagged<Domain, UT>
  {
      using base_type = private_tagged<Domain, UT>;
      using base_type::base_type;

      // constructors
      // todo add mixed conversions for strong_counter

      // assignment

      constexpr UT count() const noexcept
          { return this->underlying(); }

#if 0
      // special counter values
      static constexpr strong_counter zero()  noexcept
          // fixme, these should depend on the Domain
          { return strong_counter{Domain::zero<UT>()}; }
      static constexpr strong_counter min()  noexcept
          // fixme, these should depend on the Domain
          { return strong_counter{Domain::min<UT>()}; }
      static constexpr strong_counter max()  noexcept
          // fixme, these should depend on the Domain
          { return strong_counter{Domain::max<UT>()}; }
#endif

      // additive operators
      friend constexpr strong_counter operator+(strong_counter x)  noexcept
      { return x; }
      friend constexpr strong_counter operator+(strong_counter x, strong_counter y)  noexcept
      { return strong_counter(x.value + y.value); }
      constexpr strong_counter& operator+=(strong_counter y)  noexcept
      { this->value += y.value; return *this; }
      constexpr strong_counter operator++()  noexcept
      { return strong_counter(++this->value); }
      constexpr strong_counter operator++(int)  noexcept
      { return strong_counter(this->value++); }

      friend constexpr strong_counter operator-(strong_counter x)  noexcept
      { return strong_counter(-x.value); }
      friend constexpr strong_counter operator-(strong_counter x, strong_counter y)  noexcept
      { return strong_counter(x.value - y.value); }
      constexpr strong_counter& operator-=(strong_counter y)  noexcept
      { this->value -= y.value; return *this; }
      constexpr strong_counter operator--()  noexcept
      { return strong_counter(--this->value); }
      constexpr strong_counter operator--(int)  noexcept
      { return strong_counter(this->value--); }

      //  Multiplicative operators
      friend constexpr strong_counter operator*(strong_counter x, UT y)  noexcept
      { return strong_counter(x.value * y); }
      friend constexpr strong_counter operator*(UT x, strong_counter y)  noexcept
      { return strong_counter(x * y.value); }
      constexpr strong_counter& operator*=(UT y)  noexcept
      { this->value *= y; return *this; }

      friend constexpr UT operator/(strong_counter x, strong_counter y)  noexcept
      { return x.value / y.value; }
      friend constexpr strong_counter operator/(strong_counter x, UT y)  noexcept
      { return strong_counter(x.value / y); }
      constexpr strong_counter& operator/=(UT y)  noexcept
      { this->value /= y; return *this; }

      friend constexpr strong_counter operator%(strong_counter x, strong_counter y)  noexcept
      { return strong_counter(x.value % y.value); }
      friend constexpr strong_counter operator%(strong_counter x, UT y)  noexcept
      { return strong_counter(x.value % y); }
      constexpr strong_counter& operator%=(strong_counter y)  noexcept
      { this->value %= y.value; return *this; }
      constexpr strong_counter& operator%=(UT y)  noexcept
      { this->value %= y; return *this; }

      // relational operators
      friend constexpr bool operator==(strong_counter x, strong_counter y)  noexcept { return x.value == y.value; }
      friend constexpr bool operator!=(strong_counter x, strong_counter y)  noexcept { return x.value != y.value; }
      friend constexpr bool operator<(strong_counter x, strong_counter y)  noexcept { return x.value < y.value; }
      friend constexpr bool operator>(strong_counter x, strong_counter y)  noexcept { return x.value > y.value; }
      friend constexpr bool operator<=(strong_counter x, strong_counter y)  noexcept { return x.value <= y.value; }
      friend constexpr bool operator>=(strong_counter x, strong_counter y)  noexcept { return x.value >= y.value; }
  };

  // todo add mixed arithmetic for strong_counter
  // fixme do we need swap?

  template <class Domain, class UT>
  constexpr strong_counter<Domain, UT> make_strong_counter(UT x)
  {
    return strong_counter<Domain, UT>(x);
  }

  //! underlying_type specialization for strong_counter
  template <class Domain, class UT>
  struct underlying_type<strong_counter<Domain,UT>> { using type = UT; };

  static_assert(std::is_pod<strong_counter<bool,int>>::value, "");
  static_assert(std::is_trivially_default_constructible<strong_counter<bool,int>>::value, "");
  static_assert(std::is_trivially_copyable<strong_counter<bool,int>>::value, "");
  static_assert(std::is_standard_layout<strong_counter<bool,int>>::value, "");
  static_assert(std::is_trivial<strong_counter<bool,int>>::value, "");

}
}

  template <class Domain1, class UT1, class Domain2, class UT2>
  struct common_type<experimental::strong_counter<Domain1,UT1>, experimental::strong_counter<Domain2,UT2>>
  {
      using type =  experimental::strong_counter<
          typename std::common_type<Domain1, Domain2>::type,
          typename std::common_type<UT1, UT2>::type
          >;
  };

  template <class Domain, class UT>
  struct hash<experimental::strong_counter<Domain,UT>>
    : experimental::wrapped_hash<experimental::strong_counter<Domain, UT>> {};

  template <class Domain, class UT>
  struct numeric_limits<experimental::strong_counter<Domain,UT>> : numeric_limits<UT> {  };


}
#endif
#endif // header
