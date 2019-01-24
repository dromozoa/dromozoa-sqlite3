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
#include <sstream>

#include "dromozoa/bind.hpp"
#include "dromozoa/bind/thread.hpp"
#include "common.hpp"

namespace dromozoa {
  namespace {
    int v1 = 0;
    int v2 = 0;
    int v3 = 0;
    int v4 = 0;

    void* f1(void*) {
      if (verbose()) {
        std::cout << "f1\n";
      }
      v1 = 1;
      return 0;
    }

    void* f2(void*) {
      if (verbose()) {
        std::cout << "f2\n";
      }
      v2 = 2;
      return 0;
    }

    void* f3(void*) {
      if (verbose()) {
        std::cout << "f3\n";
      }
      v3 = 3;
      return 0;
    }

    void* f4(void*) {
      if (verbose()) {
        std::cout << "f4\n";
      }
      v4 = 4;
      return 0;
    }

    void impl_test(lua_State* L) {
      v1 = 0;
      v2 = 0;
      v3 = 0;
      v4 = 0;
      thread t1(f1, 0);
      thread t2(f2, 0);
      thread t3(f3, 0);
      thread t4(f4, 0);

      std::ostringstream out;
      out << this_thread::get_id() << "\n"
          << t1.get_id() << "\n"
          << t2.get_id() << "\n"
          << t3.get_id() << "\n"
          << t4.get_id() << "\n";

      if (verbose()) {
        std::cout << out.str();
      }

      t1.join();
      t2.join();
      t3.join();
      t4.join();
      luaX_push(L, v1, v2, v3, v4);
    }
  }

  void initialize_thread(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "test", impl_test);
    }
    luaX_set_field(L, -2, "thread");
  }
}
