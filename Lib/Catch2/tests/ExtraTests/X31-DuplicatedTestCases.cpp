
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

/**\file
 * Checks that test cases with identical name and tags are reported as error
 */

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "A test case with duplicated name and tags", "[tag1][tag2]" ) {}
TEST_CASE( "A test case with duplicated name and tags", "[tag1][tag2]" ) {}
