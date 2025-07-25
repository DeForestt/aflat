
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_errno_guard.hpp>
#include <cerrno>

namespace Catch {
    ErrnoGuard::ErrnoGuard(): m_oldErrno( errno ) {}
    ErrnoGuard::~ErrnoGuard() { errno = m_oldErrno; }
} // namespace Catch
