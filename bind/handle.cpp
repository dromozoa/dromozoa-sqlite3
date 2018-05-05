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
#include <set>

#include "dromozoa/bind.hpp"
#include "common.hpp"

namespace dromozoa {
  namespace {
    std::set<int> destructed_;

    class handle {
    public:
      explicit handle(int key) : key_(key) {
        if (verbose()) {
          std::cout << "[VERBOSE] " << this << " handle(" << key << ")\n";
        }
      }

      ~handle() {
        if (verbose()) {
          std::cout << "[VERBOSE] " << this << " ~handle()\n";
        }
        destructed_.insert(key_);
      }

      int get() {
        return key_;
      }

    private:
      int key_;
      handle(const handle&);
      handle& operator=(const handle&);
    };

    void new_handle_ref(lua_State* L, int key) {
      luaX_new<handle>(L, key);
      luaX_set_metatable(L, "dromozoa.bind.handle_ref");
    }

    void new_handle(lua_State* L, int key) {
      luaX_new<handle>(L, key);
      luaX_set_metatable(L, "dromozoa.bind.handle");
    }

    handle* check_handle(lua_State* L, int arg) {
      return luaX_check_udata<handle>(L, arg, "dromozoa.bind.handle_ref", "dromozoa.bind.handle");
    }

    void impl_call(lua_State* L) {
      int key = luaX_check_integer<int>(L, 2);
      new_handle(L, key);
    }

    void impl_gc(lua_State* L) {
      check_handle(L, 1)->~handle();
    }

    void impl_get(lua_State* L) {
      luaX_push(L, check_handle(L, 1)->get());
    }

    void impl_clear_destructed(lua_State*) {
      destructed_.clear();
    }

    void impl_is_destructed(lua_State* L) {
      int key = luaX_check_integer<int>(L, 1);
      luaX_push(L, destructed_.find(key) != destructed_.end());
    }

    void impl_handle_ref(lua_State* L) {
      int key = luaX_check_integer<int>(L, 1);
      new_handle_ref(L, key);
    }
  }

  void initialize_handle(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.bind.handle_ref");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      lua_pop(L, 1);

      luaL_newmetatable(L, "dromozoa.bind.handle");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "get", impl_get);
      luaX_set_field(L, -1, "clear_destructed", impl_clear_destructed);
      luaX_set_field(L, -1, "is_destructed", impl_is_destructed);
    }
    luaX_set_field(L, -2, "handle");

    luaX_set_field(L, -1, "handle_ref", impl_handle_ref);
  }
}
