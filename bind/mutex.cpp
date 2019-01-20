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
#include "dromozoa/bind/condition_variable.hpp"
#include "dromozoa/bind/mutex.hpp"
#include "common.hpp"

namespace dromozoa {
  namespace {
    mutex m;

    void impl_test(lua_State* L) {
      lock_guard<> lock(m);
      if (verbose()) {
        std::cout << lock.mutex() << "\n";
      }

      if (false) {
        condition_variable cond;
        cond.wait(lock);
      }

      luaX_push_success(L);
    }
  }

  void initialize_mutex(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "test", impl_test);
    }
    luaX_set_field(L, -2, "mutex");
  }
}
