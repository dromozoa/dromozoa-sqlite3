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
    void impl_gc(lua_State* L) {
      check_database_handle(L, 1)->~database_handle();
    }

    void impl_close(lua_State* L) {
      int result = check_database_handle(L, 1)->close();
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }

    void impl_busy_timeout(lua_State* L) {
      sqlite3* dbh = check_dbh(L, 1);
      int timeout = luaX_check_integer<int>(L, 2);
      if (sqlite3_busy_timeout(dbh, timeout) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, dbh);
      }
    }

    void impl_prepare(lua_State* L) {
      sqlite3* dbh = check_dbh(L, 1);
      size_t size = 0;
      const char* sql = luaL_checklstring(L, 2, &size);
      size_t i = luaX_opt_range_i(L, 3, size);
      size_t j = luaX_opt_range_j(L, 4, size);
      sqlite3_stmt* sth = 0;
      const char* tail = 0;
      int result = SQLITE_ERROR;
      if (i < j) {
        result = sqlite3_prepare_v2(dbh, sql + i, j - i, &sth, &tail);
      } else {
        result = sqlite3_prepare_v2(dbh, "", 0, &sth, 0);
      }
      if (result == SQLITE_OK) {
        new_sth(L, sth);
        if (tail) {
          luaX_push(L, tail - sql + 1);
        }
      } else {
        sqlite3_finalize(sth);
        push_error(L, dbh);
      }
    }

    void impl_total_changes(lua_State* L) {
      luaX_push(L, sqlite3_total_changes(check_dbh(L, 1)));
    }

    void impl_changes(lua_State* L) {
      luaX_push(L, sqlite3_changes(check_dbh(L, 1)));
    }

    void impl_last_insert_rowid(lua_State* L) {
      luaX_push(L, sqlite3_last_insert_rowid(check_dbh(L, 1)));
    }
  }

  void new_dbh(lua_State* L, sqlite3* dbh) {
    luaX_new<database_handle>(L, dbh);
    luaX_set_metatable(L, "dromozoa.sqlite3.dbh");
  }

  database_handle* check_database_handle(lua_State* L, int arg) {
    return luaX_check_udata<database_handle>(L, arg, "dromozoa.sqlite3.dbh");
  }

  sqlite3* check_dbh(lua_State* L, int arg) {
    return check_database_handle(L, arg)->get();
  }

  void initialize_dbh_function(lua_State* L);

  void initialize_dbh(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.sqlite3.dbh");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "close", impl_close);
      luaX_set_field(L, -1, "busy_timeout", impl_busy_timeout);
      luaX_set_field(L, -1, "prepare", impl_prepare);
      luaX_set_field(L, -1, "total_changes", impl_total_changes);
      luaX_set_field(L, -1, "changes", impl_changes);
      luaX_set_field(L, -1, "last_insert_rowid", impl_last_insert_rowid);

      initialize_dbh_function(L);
    }
    luaX_set_field(L, -2, "dbh");
  }
}
