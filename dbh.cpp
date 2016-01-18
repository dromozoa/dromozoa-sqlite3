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

#include <sqlite3.h>

#include <iostream>

#include "dromozoa/bind.hpp"

#include "database_handle.hpp"
#include "dbh.hpp"
#include "close.hpp"
#include "error.hpp"
#include "sth.hpp"

namespace dromozoa {
  using bind::function;
  using bind::get_log_level;
  using bind::push_success;
  using bind::translate_range_i;
  using bind::translate_range_j;

  int new_dbh(lua_State* L, sqlite3* dbh) {
    database_handle* d = static_cast<database_handle*>(lua_newuserdata(L, sizeof(database_handle)));
    new(d) database_handle(dbh);
    luaL_getmetatable(L, "dromozoa.sqlite3.dbh");
    lua_setmetatable(L, -2);
    if (get_log_level() > 2) {
      std::cerr << "[dromozoa-sqlite3] new dbh " << dbh << std::endl;
    }
    return 1;
  }

  sqlite3* get_dbh(lua_State* L, int n) {
    return static_cast<database_handle*>(luaL_checkudata(L, n, "dromozoa.sqlite3.dbh"))->get();
  }

  namespace {
    int impl_close(lua_State* L) {
      database_handle& d = *static_cast<database_handle*>(luaL_checkudata(L, 1, "dromozoa.sqlite3.dbh"));
      if (sqlite3* dbh = d.get()) {
        int code = d.close();
        if (code == SQLITE_OK) {
          if (get_log_level() > 2) {
            std::cerr << "[dromozoa-sqlite3] close dbh " << dbh << std::endl;
          }
          return push_success(L);
        } else {
          return push_error(L, dbh);
        }
      } else {
        return push_success(L);
      }
    }

    int impl_gc(lua_State* L) {
      database_handle& d = *static_cast<database_handle*>(luaL_checkudata(L, 1, "dromozoa.sqlite3.dbh"));
      if (sqlite3* dbh = d.get()) {
        if (get_log_level() > 1) {
          std::cerr << "[dromozoa-sqlite3] dbh " << dbh << " detected" << std::endl;
        }
        int code = d.close();
        if (code == SQLITE_OK) {
          if (get_log_level() > 2) {
            std::cerr << "[dromozoa-sqlite3] close dbh " << dbh << std::endl;
          }
        } else {
          if (get_log_level() > 0) {
            std::cerr << "[dromozoa-sqlite3] cannot close dbh " << dbh << ": ";
            print_error(std::cerr, code);
            std::cerr << std::endl;
          }
        }
      }
      d.~database_handle();
      return 0;
    }

    int impl_busy_timeout(lua_State* L) {
      sqlite3* dbh = get_dbh(L, 1);
      int timeout = luaL_checkinteger(L, 2);
      int code = sqlite3_busy_timeout(dbh, timeout);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, dbh);
      }
    }

    int impl_exec(lua_State* L) {
      sqlite3* dbh = get_dbh(L, 1);
      const char* sql = luaL_checkstring(L, 2);
      int code = sqlite3_exec(dbh, sql, 0, 0, 0);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, dbh);
      }
    }

    int impl_changes(lua_State* L) {
      sqlite3* dbh = get_dbh(L, 1);
      lua_pushinteger(L, sqlite3_changes(dbh));
      return 1;
    }

    int impl_last_insert_rowid(lua_State* L) {
      lua_pushinteger(L, sqlite3_last_insert_rowid(get_dbh(L, 1)));
      return 1;
    }

    int impl_prepare(lua_State* L) {
      sqlite3* dbh = get_dbh(L, 1);
      size_t size = 0;
      const char* sql = luaL_checklstring(L, 2, &size);
      size_t i = translate_range_i(L, 3, size);
      size_t j = translate_range_j(L, 4, size);
      sqlite3_stmt* sth = 0;
      const char* tail = 0;
      if (i < j) {
        int code = sqlite3_prepare_v2(dbh, sql + i, j - i, &sth, &tail);
        if (code == SQLITE_OK) {
          new_sth(L, sth);
          if (tail) {
            lua_pushinteger(L, tail - sql + 1);
            return 2;
          } else {
            return 1;
          }
        } else {
          sqlite3_finalize(sth);
          return push_error(L, dbh);
        }
      } else {
        return 0;
      }
    }
  }

  int open_dbh(lua_State* L) {
    lua_newtable(L);
    function<impl_close>::set_field(L, "close");
    function<impl_busy_timeout>::set_field(L, "busy_timeout");
    function<impl_exec>::set_field(L, "exec");
    function<impl_changes>::set_field(L, "changes");
    function<impl_last_insert_rowid>::set_field(L, "last_insert_rowid");
    function<impl_prepare>::set_field(L, "prepare");
    luaL_newmetatable(L, "dromozoa.sqlite3.dbh");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    return 1;
  }
}
