// Copyright (C) 2016 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef JASEL_EXAMPLE_FRAMEWORK_STD_VECTOR_HPP
#define JASEL_EXAMPLE_FRAMEWORK_STD_VECTOR_HPP

#if __cplusplus >= 201402L
#include "../mem_usage.hpp"
#include <vector>

namespace std
{
  template <typename T>
  size_t mem_usage_impl(const vector<T>& v)
  {
    size_t ans = sizeof(v);
    for (const T& e : v) ans += experimental::mem_usage_able::mem_usage_impl(e);
    ans += (v.capacity() - v.size()) * sizeof(T);
    return ans;
  }
}

#endif

#endif