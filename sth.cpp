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
  namespace {
    statement_handle* check_statement_handle(lua_State* L, int arg) {
      return luaX_check_udata<statement_handle>(L, arg, "dromozoa.sqlite3.sth");
    }

    sqlite3_stmt* check_sth(lua_State* L, int arg) {
      return check_statement_handle(L, arg)->get();
    }

    int check_bind_parameter_index(lua_State* L, int arg, sqlite3_stmt* sth) {
      if (lua_isnumber(L, arg)) {
        return luaX_check_integer<int>(L, arg);
      } else {
        return sqlite3_bind_parameter_index(sth, luaL_checkstring(L, arg));
      }
    }

    bool push_column(lua_State* L, sqlite3_stmt* sth, int i) {
      switch (sqlite3_column_type(sth, i)) {
        case SQLITE_INTEGER:
          luaX_push(L, sqlite3_column_int64(sth, i));
          return true;
        case SQLITE_FLOAT:
          luaX_push(L, sqlite3_column_double(sth, i));
          return true;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_column_text(sth, i))) {
            lua_pushlstring(L, text, sqlite3_column_bytes(sth, i));
            return true;
          } else {
            return false;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_column_blob(sth, i))) {
            lua_pushlstring(L, blob, sqlite3_column_bytes(sth, i));
            return true;
          } else {
            return false;
          }
        case SQLITE_NULL:
          push_null(L);
          return true;
        default:
          return false;
      }
    }

    void impl_gc(lua_State* L) {
      check_statement_handle(L, 1)->~statement_handle();
    }

    void impl_finalize(lua_State* L) {
      int code = check_statement_handle(L, 1)->finalize();
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, code);
      }
    }

    // [TODO] fix interface
    void impl_step(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int code = sqlite3_step(sth);
      if (code == SQLITE_ROW || code == SQLITE_DONE) {
        luaX_push(L, code);
      } else {
        push_error(L, sth);
      }
    }

    // [TODO] fix interface
    void impl_reset(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int code = sqlite3_reset(sth);
      luaX_push(L, code);
    }

    void impl_bind_parameter_count(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      luaX_push(L, sqlite3_bind_parameter_count(sth));
    }

    void impl_bind_parameter_index(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      const char* name = luaL_checkstring(L, 2);
      luaX_push(L, sqlite3_bind_parameter_index(sth, name));
    }

    void impl_bind_parameter_name(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int index = luaX_check_integer<int>(L, 2);
      luaX_push(L, sqlite3_bind_parameter_name(sth, index));
    }

    void impl_bind_int64(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int index = check_bind_parameter_index(L, 2, sth);
      sqlite3_int64 value = luaX_check_integer<sqlite3_int64>(L, 3);
      int code = sqlite3_bind_int64(sth, index, value);
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_double(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int index = check_bind_parameter_index(L, 2, sth);
      double value = luaL_checknumber(L, 3);
      int code = sqlite3_bind_double(sth, index, value);
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_text(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int index = check_bind_parameter_index(L, 2, sth);
      size_t size = 0;
      const char* text = luaL_checklstring(L, 3, &size);
      size_t i = luaX_opt_range_i(L, 4, size);
      size_t j = luaX_opt_range_j(L, 5, size);
      int code = SQLITE_ERROR;
      if (i < j) {
        code = sqlite3_bind_text(sth, index, text, size, SQLITE_TRANSIENT);
      } else {
        code = sqlite3_bind_text(sth, index, "", 0, SQLITE_STATIC);
      }
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_blob(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int index = check_bind_parameter_index(L, 2, sth);
      size_t size = 0;
      const char* blob = luaL_checklstring(L, 3, &size);
      size_t i = luaX_opt_range_i(L, 4, size);
      size_t j = luaX_opt_range_j(L, 5, size);
      int code = SQLITE_ERROR;
      if (i < j) {
        code = sqlite3_bind_blob(sth, index, blob + i, j - i, SQLITE_TRANSIENT);
      } else {
        code = sqlite3_bind_zeroblob(sth, index, 0);
      }
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_bind_null(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int index = check_bind_parameter_index(L, 2, sth);
      int code = sqlite3_bind_null(sth, index);
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_clear_bindings(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int code = sqlite3_clear_bindings(sth);
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, sth);
      }
    }

    void impl_column_count(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      luaX_push(L, sqlite3_column_count(sth));
    }

    void impl_column_name(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int i = luaX_check_integer<int>(L, 2) - 1;
      luaX_push(L, sqlite3_column_name(sth, i));
    }

    void impl_column_type(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int i = luaX_check_integer<int>(L, 2) - 1;
      int type = sqlite3_column_type(sth, i);
      luaX_push(L, type);
    }

    void impl_column(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int i = luaX_check_integer<int>(L, 2) - 1;
      push_column(L, sth, i);
    }

    void impl_columns(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int count = sqlite3_column_count(sth);
      lua_newtable(L);
      for (int i = 0; i < count; ++i) {
        if (push_column(L, sth, i)) {
          lua_pushvalue(L, -1);
          luaX_set_field(L, -3, i + 1);
          luaX_set_field(L, -2, sqlite3_column_name(sth, i));
        }
      }
    }

    void impl_sql(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      luaX_push(L, sqlite3_sql(sth));
    }
  }

  void new_sth(lua_State* L, sqlite3_stmt* sth) {
    luaX_new<statement_handle>(L, sth);
    luaX_set_metatable(L, "dromozoa.sqlite3.sth");
  }

  void initialize_sth(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.sqlite3.sth");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "finalize", impl_finalize);
      luaX_set_field(L, -1, "step", impl_step);
      luaX_set_field(L, -1, "reset", impl_reset);
      luaX_set_field(L, -1, "bind_parameter_count", impl_bind_parameter_count);
      luaX_set_field(L, -1, "bind_parameter_index", impl_bind_parameter_index);
      luaX_set_field(L, -1, "bind_parameter_name", impl_bind_parameter_name);
      luaX_set_field(L, -1, "bind_int64", impl_bind_int64);
      luaX_set_field(L, -1, "bind_double", impl_bind_double);
      luaX_set_field(L, -1, "bind_text", impl_bind_text);
      luaX_set_field(L, -1, "bind_blob", impl_bind_blob);
      luaX_set_field(L, -1, "bind_null", impl_bind_null);
      luaX_set_field(L, -1, "clear_bindings", impl_clear_bindings);
      luaX_set_field(L, -1, "column_count", impl_column_count);
      luaX_set_field(L, -1, "column_name", impl_column_name);
      luaX_set_field(L, -1, "column_type", impl_column_type);
      luaX_set_field(L, -1, "column", impl_column);
      luaX_set_field(L, -1, "columns", impl_columns);
      luaX_set_field(L, -1, "sql", impl_sql);
    }
    luaX_set_field(L, -2, "sth");
  }
}
