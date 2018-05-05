// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
    bool push_column(lua_State* L, sqlite3_stmt* sth, int column) {
      switch (sqlite3_column_type(sth, column)) {
        case SQLITE_INTEGER:
          luaX_push(L, sqlite3_column_int64(sth, column));
          return true;
        case SQLITE_FLOAT:
          luaX_push(L, sqlite3_column_double(sth, column));
          return true;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_column_text(sth, column))) {
            lua_pushlstring(L, text, sqlite3_column_bytes(sth, column));
            return true;
          } else {
            return false;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_column_blob(sth, column))) {
            lua_pushlstring(L, blob, sqlite3_column_bytes(sth, column));
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

    void impl_column_count(lua_State* L) {
      luaX_push(L, sqlite3_column_count(check_sth(L, 1)));
    }

    void impl_column_name(lua_State* L) {
      luaX_push(L, sqlite3_column_name(check_sth(L, 1), luaX_check_integer<int>(L, 2) - 1));
    }

    void impl_column_type(lua_State* L) {
      luaX_push(L, sqlite3_column_type(check_sth(L, 1), luaX_check_integer<int>(L, 2) - 1));
    }

    void impl_column_decltype(lua_State* L) {
      luaX_push(L, sqlite3_column_decltype(check_sth(L, 1), luaX_check_integer<int>(L, 2) - 1));
    }

    void impl_column(lua_State* L) {
      push_column(L, check_sth(L, 1), luaX_check_integer<int>(L, 2) - 1);
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
  }

  void initialize_sth_column(lua_State* L) {
    luaX_set_field(L, -1, "column_count", impl_column_count);
    luaX_set_field(L, -1, "column_name", impl_column_name);
    luaX_set_field(L, -1, "column_type", impl_column_type);
    luaX_set_field(L, -1, "column_decltype", impl_column_decltype);
    luaX_set_field(L, -1, "column", impl_column);
    luaX_set_field(L, -1, "columns", impl_columns);
  }
}
