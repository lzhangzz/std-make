// Copyright (C) 2017-2018 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// Based on https://github.com/akrzemi1/explicit/blob/master/test/test_explicit.cpp

// <experimental/numerics/v1/numbers/double_wide_types.hpp>

//#define JASEL_CONFIG_CONTRACT_LEVEL_MASK 0x111
#define JASEL_CONFIG_CONTRACT_VIOLATION_THROWS_V 1

#include <iostream>
#include <experimental/numerics/v1/numbers/overflow_detection_arithmetic.hpp>
#include <string>

#include <boost/detail/lightweight_test.hpp>

namespace stdex = std::experimental;
namespace nmx = std::experimental::numerics;

#if __cplusplus >= 201402L

static_assert(nmx::overflow_cvt<short>(1) == std::make_pair(false, short(1)), "error");
static_assert(nmx::overflow_cvt<short>(100000) == std::make_pair(true, short(0)), "error");
static_assert(nmx::check_overflow_cvt<short>(100000) == true, "error");

#endif

int main()
{
    {
        int i=0;
        short s;
        BOOST_TEST_EQ(false, nmx::overflow_cvt(&s, i));
        BOOST_TEST_EQ(s, 0);
    }
    {
        int i=100000;
        short s;
        BOOST_TEST_EQ(true, nmx::overflow_cvt(&s, i));
    }
    {
        int i=1;
        int r;
        BOOST_TEST_EQ(false, nmx::overflow_neg(&r, i));
        BOOST_TEST_EQ(r, -1);
    }
    {
        signed char i=-128;
        signed char r;
        BOOST_TEST_EQ(true, nmx::overflow_neg(&r, i));
    }
    {
        unsigned r;
        unsigned x = 1;
        BOOST_TEST_EQ(false, nmx::overflow_lsh(&r, x, 2));
        BOOST_TEST_EQ(r, 4);
    }
    {
        unsigned r;
        unsigned x = 1;
        BOOST_TEST_EQ(true, nmx::overflow_lsh(&r, x, -1));
    }
    {
        unsigned r;
        unsigned x = 1;
        BOOST_TEST_EQ(true, nmx::overflow_lsh(&r, x, std::numeric_limits<unsigned>::digits));
    }
    {
        unsigned r;
        unsigned x = 1;
        unsigned y = 1;
        BOOST_TEST_EQ(false, nmx::overflow_add(&r, x, y));
        BOOST_TEST_EQ(r, 2);
    }
    {
        unsigned char r;
        unsigned char x = 128;
        unsigned char y = 128;
        BOOST_TEST_EQ(true, nmx::overflow_add(&r, x, y));
    }
    {
        signed char r;
        signed char x = -64;
        signed char y = -65;
        BOOST_TEST_EQ(true, nmx::overflow_add(&r, x, y));
    }
  return boost::report_errors();
}
