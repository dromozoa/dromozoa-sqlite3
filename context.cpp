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

#include <stddef.h>

#include "dromozoa/bind.hpp"

#include <sqlite3.h>

namespace dromozoa {
  using bind::function;
  using bind::push_success;
  using bind::translate_range_i;
  using bind::translate_range_j;

  int new_context(lua_State* L, sqlite3_context* context) {
    *static_cast<sqlite3_context**>(lua_newuserdata(L, sizeof(context))) = context;
    luaL_getmetatable(L, "dromozoa.sqlite3.context");
    lua_setmetatable(L, -2);
    return 1;
  }

  namespace {
    sqlite3_context* get_context(lua_State* L, int n) {
      return *static_cast<sqlite3_context**>(luaL_checkudata(L, n, "dromozoa.sqlite3.context"));
    }

    int impl_result_int64(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      lua_Integer v = luaL_checkinteger(L, 2);
      sqlite3_result_int64(context, v);
      return push_success(L);
    }

    int impl_result_double(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      lua_Integer v = luaL_checknumber(L, 2);
      sqlite3_result_double(context, v);
      return push_success(L);
    }

    int impl_result_text(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      size_t size = 0;
      const char* text = luaL_checklstring(L, 2, &size);
      size_t i = translate_range_i(L, 3, size);
      size_t j = translate_range_i(L, 4, size);
      if (i < j) {
        sqlite3_result_text(context, text + i, j - i, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_text(context, "", 0, SQLITE_STATIC);
      }
      return push_success(L);
    }

    int impl_result_blob(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      size_t size = 0;
      const char* blob = luaL_checklstring(L, 2, &size);
      size_t i = translate_range_i(L, 3, size);
      size_t j = translate_range_j(L, 4, size);
      if (i < j) {
        sqlite3_result_blob(context, blob + i, j - i, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_zeroblob(context, 0);
      }
      return push_success(L);
    }
  }

  int open_context(lua_State* L) {
    lua_newtable(L);
    function<impl_result_int64>::set_field(L, "result_int64");
    function<impl_result_double>::set_field(L, "result_double");
    function<impl_result_text>::set_field(L, "result_text");
    function<impl_result_blob>::set_field(L, "result_blob");
    luaL_newmetatable(L, "dromozoa.sqlite3.context");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
    return 1;
  }
}
