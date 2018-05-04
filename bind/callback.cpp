// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include "common.hpp"

#include "dromozoa/bind.hpp"

namespace dromozoa {
  namespace {
    class callback {
    public:
      callback(lua_State* L, int index0, int index1) : ref_(L, index0, index1) {
        if (verbose()) {
          std::cout << "[VERBOSE] " << this << " callback(" << index0 << ", " << index1 << ")\n";
        }
      }

      ~callback() {
        if (verbose()) {
          std::cout << "[VERBOSE] " << this << " ~callback()\n";
        }
      }

      void run(lua_State* L, bool use_current) {
        lua_State* state = 0;
        if (use_current) {
          state = L;
        } else {
          state = ref_.state();
        }
        ref_.get_field(state, 0);
        ref_.get_field(state, 1);
        int r = lua_pcall(state, 1, 0, 0);
        if (r != 0) {
          if (verbose()) {
            std::cout << "[VERBOSE] lua_pcall error: " << lua_tostring(state, -1) << "\n";
          }
          lua_pop(state, 1);
        }
        luaX_push(L, r);
      }

    private:
      luaX_reference<2> ref_;
    };

    void new_callback(lua_State* L, int index0, int index1) {
      luaX_new<callback>(L, L, index0, index1);
      luaX_set_metatable(L, "dromozoa.bind.callback");
    }

    callback* check_callback(lua_State* L, int arg) {
      return luaX_check_udata<callback>(L, arg, "dromozoa.bind.callback");
    }

    void impl_call(lua_State* L) {
      new_callback(L, 2, 3);
    }

    void impl_gc(lua_State* L) {
      check_callback(L, 1)->~callback();
    }

    void impl_run(lua_State* L) {
      bool use_current = lua_toboolean(L, 2);
      check_callback(L, 1)->run(L, use_current);
    }
  }

  void initialize_callback(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.bind.callback");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "run", impl_run);
    }
    luaX_set_field(L, -2, "callback");
  }
}
