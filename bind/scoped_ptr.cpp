// Copyright (C) 2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
    class test1 {
    public:
      test1() {
        if (verbose()) {
          std::cout << "[VERBOSE] " << this << " test1()\n";
        }
      }

      ~test1() {
        if (verbose()) {
          std::cout << "[VERBOSE] " << this << " ~test1()\n";
        }
      }
    };

    class test2;

    void impl_test(lua_State* L) {
      if (verbose()) {
        std::cout << "[VERBOSE] impl_test p1\n";
      }

      {
        scoped_ptr<test1> ptr1(new test1());
        scoped_ptr<test1> ptr2;

        DROMOZOA_CHECK(ptr1);
        DROMOZOA_CHECK(!!ptr1);
        DROMOZOA_CHECK(!ptr1 == false);
        DROMOZOA_CHECK(ptr1.get());
        DROMOZOA_CHECK(ptr1.get() != 0);

        bool b = ptr1;
        DROMOZOA_CHECK(b == true);

        DROMOZOA_CHECK(static_cast<bool>(ptr2) == false);
        DROMOZOA_CHECK(!ptr2);
        DROMOZOA_CHECK(!ptr2.get());
        DROMOZOA_CHECK(ptr2.get() == 0);

        ptr2.swap(ptr1);
        DROMOZOA_CHECK(!ptr1);
        DROMOZOA_CHECK(ptr2);

        // compile error
        // int i = ptr1;
        // scoped_ptr<test2> ptr3;
      }

      if (verbose()) {
        std::cout << "[VERBOSE] impl_test p2\n";
      }

      {
        scoped_ptr<test1> ptr1;
        {
          scoped_ptr<test1> ptr2;
          DROMOZOA_CHECK(!ptr2);
          ptr2.reset(new test1());
          DROMOZOA_CHECK(ptr2);
          ptr1.reset(ptr2.release());

          if (verbose()) {
            std::cout << "[VERBOSE] impl_test p3\n";
          }
        }

        if (verbose()) {
          std::cout << "[VERBOSE] impl_test p4\n";
        }
      }

      if (verbose()) {
        std::cout << "[VERBOSE] impl_test p5\n";
      }

      return luaX_push_success(L);
    }
  }

  void initialize_scoped_ptr(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "test", impl_test);
    }
    luaX_set_field(L, -2, "scoped_ptr");
  }
}
