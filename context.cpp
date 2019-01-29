// Copyright (C) 2016,2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
  namespace {
    sqlite3_context* check_context(lua_State* L, int arg) {
      return *luaX_check_udata<sqlite3_context*>(L, arg, "dromozoa.sqlite3.context");
    }

    void result_text(lua_State* L, sqlite3_context* context) {
      luaX_string_reference text = luaX_check_string(L, 2);
      size_t i = luaX_opt_range_i(L, 3, text.size());
      size_t j = luaX_opt_range_i(L, 4, text.size());
      if (i < j) {
        sqlite3_result_text(context, text.data() + i, j - i, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_text(context, "", 0, SQLITE_STATIC);
      }
    }

    void impl_result_int64(lua_State* L) {
      sqlite3_result_int64(check_context(L, 1), luaX_check_integer<sqlite3_int64>(L, 2));
      luaX_push_success(L);
    }

    void impl_result_double(lua_State* L) {
      sqlite3_result_double(check_context(L, 1), luaL_checknumber(L, 2));
      luaX_push_success(L);
    }

    void impl_result_text(lua_State* L) {
      result_text(L, check_context(L, 1));
      luaX_push_success(L);
    }

    void impl_result_blob(lua_State* L) {
      sqlite3_context* context = check_context(L, 1);
      luaX_string_reference blob = luaX_check_string(L, 2);
      size_t i = luaX_opt_range_i(L, 3, blob.size());
      size_t j = luaX_opt_range_i(L, 4, blob.size());
      if (i < j) {
        sqlite3_result_blob(context, blob.data() + i, j - i, SQLITE_TRANSIENT);
      } else {
        sqlite3_result_zeroblob(context, 0);
      }
      luaX_push_success(L);
    }

    void impl_result_null(lua_State* L) {
      sqlite3_result_null(check_context(L, 1));
      luaX_push_success(L);
    }

    void impl_result_zeroblob(lua_State* L) {
      sqlite3_result_zeroblob(check_context(L, 1), luaX_check_integer<int>(L, 2));
      luaX_push_success(L);
    }

    void impl_result(lua_State* L) {
      sqlite3_context* context = check_context(L, 1);
      switch (lua_type(L, 2)) {
        case LUA_TNUMBER:
          if (luaX_is_integer(L, 2)) {
            sqlite3_result_int64(context, luaX_check_integer<sqlite3_int64>(L, 2));
          } else {
            sqlite3_result_double(context, luaL_checknumber(L, 2));
          }
          break;
        case LUA_TSTRING:
          result_text(L, context);
          break;
        case LUA_TBOOLEAN:
          sqlite3_result_int64(context, lua_toboolean(L, 3));
          break;
        default:
          sqlite3_result_null(context);
      }
      luaX_push_success(L);
    }
  }

  void initialize_context(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.sqlite3.context");
      lua_pushvalue(L, -2);
      lua_setfield(L, -2, "__index");
      lua_pop(L, 1);

      luaX_set_field(L, -1, "result_int64", impl_result_int64);
      luaX_set_field(L, -1, "result_double", impl_result_double);
      luaX_set_field(L, -1, "result_text", impl_result_text);
      luaX_set_field(L, -1, "result_blob", impl_result_blob);
      luaX_set_field(L, -1, "result_null", impl_result_null);
      luaX_set_field(L, -1, "result_zeroblob", impl_result_zeroblob);
      luaX_set_field(L, -1, "result", impl_result);
    }
    luaX_set_field(L, -2, "context");
  }
}
