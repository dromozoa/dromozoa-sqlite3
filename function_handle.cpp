// Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-sqlite3.
//
// dromozoa-sqlite3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-sqlite3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-sqlite3.  If not, see <http://www.gnu.org/licenses/>.

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "database_handle.hpp"
#include "function_handle.hpp"

namespace dromozoa {
  function_handle::function_handle(lua_State* L, int ref)
    : L_(L), ref_(ref), ref_step_(LUA_NOREF), ref_final_(LUA_NOREF) {}

  function_handle::function_handle(lua_State* L, int ref_step, int ref_final)
    : L_(L), ref_(LUA_NOREF), ref_step_(ref_step), ref_final_(ref_final) {}

  function_handle::~function_handle() {
    if (ref_ != LUA_NOREF) {
      luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
    }
    if (ref_step_ != LUA_NOREF) {
      luaL_unref(L_, LUA_REGISTRYINDEX, ref_step_);
    }
    if (ref_final_ != LUA_NOREF) {
      luaL_unref(L_, LUA_REGISTRYINDEX, ref_final_);
    }
  }

  lua_State* function_handle::get() const {
    return L_;
  }

  int function_handle::ref() const {
    return ref_;
  }

  int function_handle::ref_step() const {
    return ref_step_;
  }

  int function_handle::ref_final() const {
    return ref_final_;
  }
}
