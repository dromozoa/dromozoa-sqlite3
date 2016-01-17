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

#include "dromozoa/bind.hpp"

#include "dbh.hpp"
#include "close.hpp"
#include "context.hpp"
#include "error.hpp"
#include "function.hpp"
#include "sth.hpp"

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  namespace {
    int impl_initialize(lua_State* L) {
      int code = sqlite3_initialize();
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, code);
      }
    }

    int impl_shutdown(lua_State* L) {
      int code = sqlite3_shutdown();
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, code);
      }
    }

    int impl_open(lua_State* L) {
      const char* filename = luaL_checkstring(L, 1);
      int flags = luaL_optinteger(L, 2, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
      const char* vfs = lua_tostring(L, 3);
      sqlite3* dbh = 0;
      int code = sqlite3_open_v2(filename, &dbh, flags, vfs);
      if (code == SQLITE_OK) {
        return new_dbh(L, dbh);
      } else {
        wrap_close(dbh);
        return push_error(L, code);
      }
    }

    void initialize(lua_State* L) {
      function<impl_initialize>::set_field(L, "initialize");
      function<impl_shutdown>::set_field(L, "shutdown");
      function<impl_open>::set_field(L, "open");

      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_READONLY);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_READWRITE);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_CREATE);
#ifdef SQLITE_OPEN_URI
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_URI);
#endif
#ifdef SQLITE_OPEN_MEMORY
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_MEMORY);
#endif
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_NOMUTEX);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_FULLMUTEX);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_SHAREDCACHE);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OPEN_PRIVATECACHE);

      DROMOZOA_BIND_SET_FIELD(L, SQLITE_OK);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_ROW);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_DONE);

      DROMOZOA_BIND_SET_FIELD(L, SQLITE_INTEGER);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_FLOAT);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_TEXT);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_BLOB);
      DROMOZOA_BIND_SET_FIELD(L, SQLITE_NULL);
    }
  }

  int open(lua_State* L) {
    lua_newtable(L);

    open_context(L);
    lua_setfield(L, -2, "context");

    open_dbh(L);
    initialize_function(L);
    lua_setfield(L, -2, "dbh");

    open_sth(L);
    lua_setfield(L, -2, "sth");

    dromozoa::bind::initialize(L);
    initialize(L);

    return 1;
  }
}

extern "C" int luaopen_dromozoa_sqlite3(lua_State* L) {
  return dromozoa::open(L);
}
