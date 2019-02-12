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
#include "dromozoa/bind/atomic.hpp"
#include "dromozoa/bind/thread.hpp"
#include "common.hpp"

namespace dromozoa {
  namespace {
    void* f(void* data) {
      atomic_count<int>& count = *static_cast<atomic_count<int>*>(data);
      for (int i = 0; i < 1000; ++i) {
        ++count;
      }
      return 0;
    }

    void impl_test(lua_State* L) {
      atomic_count<int> count;
      thread t1(f, &count);
      thread t2(f, &count);
      thread t3(f, &count);
      thread t4(f, &count);
      t1.join();
      t2.join();
      t3.join();
      t4.join();
      luaX_push(L, --count);
    }
  }

  void initialize_atomic(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "test", impl_test);
    }
    luaX_set_field(L, -2, "atomic");
  }
}
