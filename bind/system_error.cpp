// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-bind.
//
// dromozoa-bind is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-bind is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>

#include "dromozoa/bind.hpp"
#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_test_errno_saver(lua_State* L) {
      errno = ENOENT;
      DROMOZOA_CHECK(errno == ENOENT);
      {
        dromozoa::errno_saver save_errno;
        DROMOZOA_CHECK(errno == ENOENT);
        errno = EINTR;
        DROMOZOA_CHECK(errno == EINTR);
      }
      DROMOZOA_CHECK(errno == ENOENT);
      luaX_push_success(L);
    }

    void impl_test_compat_strerror(lua_State* L) {
      if (verbose()) {
        std::cout
            << "strerror(ENOENT) " << compat_strerror(ENOENT) << "\n"
            << "strerror(-1)     " << compat_strerror(-1)     << "\n"
            << "strerror(0)      " << compat_strerror(0)      << "\n"
            << "strerror(65535)  " << compat_strerror(65535)  << "\n";
      }
      luaX_push_success(L);
    }

    void impl_test_system_error(lua_State*) {
      throw system_error(ENOENT);
    }
  }

  void initialize_system_error(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "test_errno_saver", impl_test_errno_saver);
      luaX_set_field(L, -1, "test_compat_strerror", impl_test_compat_strerror);
      luaX_set_field(L, -1, "test_system_error", impl_test_system_error);
    }
    luaX_set_field(L, -2, "system_error");
  }
}
