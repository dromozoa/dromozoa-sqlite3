// Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include "lua.h"
#include "lauxlib.h"
}

#include <sqlite3.h>

#include <iostream>

#include "error.hpp"
#include "function.hpp"
#include "log_level.hpp"
#include "success.hpp"

namespace dromozoa {
  int new_dbh(lua_State* L, sqlite3* dbh) {
    *static_cast<sqlite3**>(lua_newuserdata(L, sizeof(dbh))) = dbh;
    luaL_getmetatable(L, "dromozoa.sqlite3.dbh");
    lua_setmetatable(L, -2);
    if (get_log_level() > 2) {
      std::cerr << "[dromozoa-sqlite3] new dbh " << dbh << std::endl;
    }
    return 1;
  }

  sqlite3* get_dbh(lua_State* L, int n) {
    return *static_cast<sqlite3**>(luaL_checkudata(L, n, "dromozoa.sqlite3.dbh"));
  }

  namespace {
    int impl_close(lua_State* L) {
      sqlite3** data = static_cast<sqlite3**>(luaL_checkudata(L, 1, "dromozoa.sqlite3.dbh"));
      sqlite3* dbh = *data;
      int code = sqlite3_close(dbh);
      if (code == SQLITE_OK) {
        *data = 0;
        if (get_log_level() > 2) {
          std::cerr << "[dromozoa-sqlite3] close dbh " << dbh << std::endl;
        }
        return push_success(L);
      } else {
        return push_error(L, code);
      }
    }

    int impl_gc(lua_State* L) {
      sqlite3** data = static_cast<sqlite3**>(luaL_checkudata(L, 1, "dromozoa.sqlite3.dbh"));
      sqlite3* dbh = *data;
      *data = 0;
      if (dbh) {
        if (get_log_level() > 1) {
          std::cerr << "[dromozoa-sqlite3] dbh " << dbh << " detected" << std::endl;
        }
        int code = sqlite3_close_v2(dbh);
        if (code == SQLITE_OK) {
          if (get_log_level() > 2) {
            std::cerr << "[dromozoa-sqlite3] close dbh " << dbh << std::endl;
          }
        } else {
          if (get_log_level() > 0) {
            std::cerr << "[dromozoa-sqlite3] cannot close dbh " << dbh << ": " << sqlite3_errstr(code) << std::endl;
          }
        }
      }
      return 0;
    }

    int impl_exec(lua_State* L) {
      const char* sql = luaL_checkstring(L, 2);
      int code = sqlite3_exec(get_dbh(L, 1), sql, 0, 0, 0);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, code);
      }
    }

    int impl_busy_timeout(lua_State* L) {
      int timeout = luaL_checkinteger(L, 2);
      int code = sqlite3_busy_timeout(get_dbh(L, 1), timeout);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, code);
      }
    }
  }

  int open_dbh(lua_State* L) {
    lua_newtable(L);
    function<impl_close>::set_field(L, "close");
    function<impl_exec>::set_field(L, "exec");
    function<impl_busy_timeout>::set_field(L, "busy_timeout");
    luaL_newmetatable(L, "dromozoa.sqlite3.dbh");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    return 1;
  }
}
