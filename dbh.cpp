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

#include "dbh.hpp"
#include "error.hpp"
#include "function.hpp"
#include "log_level.hpp"
#include "sth.hpp"
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
#if SQLITE_VERSION_NUMBER >= 3007014
        int code = sqlite3_close_v2(dbh);
#else
        int code = sqlite3_close(dbh);
#endif
        if (code == SQLITE_OK) {
          if (get_log_level() > 2) {
            std::cerr << "[dromozoa-sqlite3] close dbh " << dbh << std::endl;
          }
        } else {
          if (get_log_level() > 0) {
#if SQLITE_VERSION_NUMBER >= 3007015
            if (const char* what = sqlite3_errstr(code)) {
              std::cerr << "[dromozoa-sqlite3] cannot close dbh " << dbh << ": " << what << std::endl;
            } else
#endif
            {
              std::cerr << "[dromozoa-sqlite3] cannot close dbh " << dbh << ": error number " << code << std::endl;
            }
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

    int impl_prepare(lua_State* L) {
      size_t size = 0;
      const char* sql = luaL_checklstring(L, 2, &size);
      ssize_t i = luaL_optinteger(L, 3, 0);
      if (i < 0) {
        i += size;
        if (i < 0) {
          i = 0;
        }
      } else if (i > 0) {
        --i;
      }
      ssize_t j = luaL_optinteger(L, 4, size);
      if (j < 0) {
        j += size + 1;
      } else if (j > static_cast<ssize_t>(size)) {
        j = size;
      }
      if (i > j) {
        i = j;
      }
      sqlite3_stmt* sth = 0;
      const char* tail = 0;
      int code = sqlite3_prepare_v2(get_dbh(L, 1), sql + i, j - i, &sth, &tail);
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
        return push_error(L, code);
      }
    }

    int impl_last_insert_rowid(lua_State* L) {
      lua_pushinteger(L, sqlite3_last_insert_rowid(get_dbh(L, 1)));
      return 1;
    }
  }

  int open_dbh(lua_State* L) {
    lua_newtable(L);
    function<impl_close>::set_field(L, "close");
    function<impl_exec>::set_field(L, "exec");
    function<impl_busy_timeout>::set_field(L, "busy_timeout");
    function<impl_prepare>::set_field(L, "prepare");
    function<impl_last_insert_rowid>::set_field(L, "last_insert_rowid");
    luaL_newmetatable(L, "dromozoa.sqlite3.dbh");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    return 1;
  }
}
