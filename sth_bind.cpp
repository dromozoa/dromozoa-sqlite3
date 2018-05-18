// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
    int check_bind_parameter_index(lua_State* L, int arg, sqlite3_stmt* sth) {
      if (luaX_is_integer(L, arg)) {
        return luaX_check_integer<int>(L, arg);
      } else {
        return sqlite3_bind_parameter_index(sth, luaL_checkstring(L, arg));
      }
    }

    int bind_text(lua_State* L, sqlite3_stmt* sth, int param) {
      luaX_string_reference text = luaX_check_string(L, 3);
      size_t i = luaX_opt_range_i(L, 4, text.size());
      size_t j = luaX_opt_range_j(L, 5, text.size());
      if (i < j) {
        return sqlite3_bind_text(sth, param, text.data() + i, j - i, SQLITE_TRANSIENT);
      } else {
        return sqlite3_bind_text(sth, param, "", 0, SQLITE_STATIC);
      }
    }

    void impl_bind_parameter_count(lua_State* L) {
      luaX_push(L, sqlite3_bind_parameter_count(check_sth(L, 1)));
    }

    void impl_bind_parameter_index(lua_State* L) {
      luaX_push(L, sqlite3_bind_parameter_index(check_sth(L, 1), luaL_checkstring(L, 2)));
    }

    void impl_bind_parameter_name(lua_State* L) {
      luaX_push(L, sqlite3_bind_parameter_name(check_sth(L, 1), luaX_check_integer<int>(L, 2)));
    }

    void impl_bind_int64(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int param = check_bind_parameter_index(L, 2, sth);
      sqlite3_int64 value = luaX_check_integer<sqlite3_int64>(L, 3);
      if (sqlite3_bind_int64(sth, param, value) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_double(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int param = check_bind_parameter_index(L, 2, sth);
      double value = luaL_checknumber(L, 3);
      if (sqlite3_bind_double(sth, param, value) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_text(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int param = check_bind_parameter_index(L, 2, sth);
      if (bind_text(L, sth, param) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_blob(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int param = check_bind_parameter_index(L, 2, sth);
      luaX_string_reference blob = luaX_check_string(L, 3);
      size_t i = luaX_opt_range_i(L, 4, blob.size());
      size_t j = luaX_opt_range_j(L, 5, blob.size());
      int result = SQLITE_ERROR;
      if (i < j) {
        result = sqlite3_bind_blob(sth, param, blob.data() + i, j - i, SQLITE_TRANSIENT);
      } else {
        result = sqlite3_bind_zeroblob(sth, param, 0);
      }
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_null(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int param = check_bind_parameter_index(L, 2, sth);
      if (sqlite3_bind_null(sth, param) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int param = check_bind_parameter_index(L, 2, sth);
      int code = SQLITE_ERROR;
      switch (lua_type(L, 3)) {
        case LUA_TNUMBER:
          if (luaX_is_integer(L, 3)) {
            code = sqlite3_bind_int64(sth, param, luaX_check_integer<sqlite3_int64>(L, 3));
          } else {
            code = sqlite3_bind_double(sth, param, luaL_checknumber(L, 3));
          }
          break;
        case LUA_TSTRING:
          code = bind_text(L, sth, param);
          break;
        case LUA_TBOOLEAN:
          code = sqlite3_bind_int64(sth, param, lua_toboolean(L, 3));
          break;
        default:
          code = sqlite3_bind_null(sth, param);
      }
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_clear_bindings(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      if (sqlite3_clear_bindings(sth) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }
  }

  void initialize_sth_bind(lua_State* L) {
    luaX_set_field(L, -1, "bind_parameter_count", impl_bind_parameter_count);
    luaX_set_field(L, -1, "bind_parameter_index", impl_bind_parameter_index);
    luaX_set_field(L, -1, "bind_parameter_name", impl_bind_parameter_name);
    luaX_set_field(L, -1, "bind_int64", impl_bind_int64);
    luaX_set_field(L, -1, "bind_double", impl_bind_double);
    luaX_set_field(L, -1, "bind_text", impl_bind_text);
    luaX_set_field(L, -1, "bind_blob", impl_bind_blob);
    luaX_set_field(L, -1, "bind_null", impl_bind_null);
    luaX_set_field(L, -1, "bind", impl_bind);
    luaX_set_field(L, -1, "clear_bindings", impl_clear_bindings);
  }
}
