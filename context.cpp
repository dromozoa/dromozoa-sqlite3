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

#include "common.hpp"

namespace dromozoa {
  void new_context(lua_State* L, sqlite3_context* context) {
    luaX_new<sqlite3_context*>(L, context);
    luaX_set_metatable(L, "dromozoa.sqlite3.context");
  }

  namespace {
    sqlite3_context* get_context(lua_State* L, int arg) {
      return *luaX_check_udata<sqlite3_context*>(L, arg, "dromozoa.sqlite3.context");
    }

    void impl_result_int64(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      lua_Integer value = luaL_checkinteger(L, 2);
      sqlite3_result_int64(context, value);
      luaX_push_success(L);
    }

    void impl_result_double(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      lua_Number value = luaL_checknumber(L, 2);
      sqlite3_result_double(context, value);
      luaX_push_success(L);
    }

    void impl_result_text(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      size_t size = 0;
      const char* text = luaL_checklstring(L, 2, &size);
      size_t i = luaX_opt_range_i(L, 3, size);
      size_t j = luaX_opt_range_i(L, 4, size);
      if (i < j) {
        sqlite3_result_text(context, text + i, j - i, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_text(context, "", 0, SQLITE_STATIC);
      }
      luaX_push_success(L);
    }

    void impl_result_blob(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      size_t size = 0;
      const char* blob = luaL_checklstring(L, 2, &size);
      size_t i = luaX_opt_range_i(L, 3, size);
      size_t j = luaX_opt_range_j(L, 4, size);
      if (i < j) {
        sqlite3_result_blob(context, blob + i, j - i, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_zeroblob(context, 0);
      }
      luaX_push_success(L);
    }

    void impl_result_null(lua_State* L) {
      sqlite3_context* context = get_context(L, 1);
      sqlite3_result_null(context);
      luaX_push_success(L);
    }
  }

  void initialize_context(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "result_int64", impl_result_int64);
      luaX_set_field(L, -1, "result_double", impl_result_double);
      luaX_set_field(L, -1, "result_text", impl_result_text);
      luaX_set_field(L, -1, "result_blob", impl_result_blob);
      luaX_set_field(L, -1, "result_null", impl_result_null);
      luaL_newmetatable(L, "dromozoa.sqlite3.context");
      lua_pushvalue(L, -2);
      lua_setfield(L, -2, "__index");
      lua_pop(L, 1);
    }
    luaX_set_field(L, -2, "context");
  }
}
