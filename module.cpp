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
    int open_entity(lua_State* L) {
      lua_getglobal(L, "require");
      lua_pushliteral(L, "dromozoa.sqlite3.entity");
      lua_call(L, 1, 1);
      lua_pushvalue(L, -2);
      lua_setfield(L, -2, "super");
      return 1;
    }

    void impl_initialize(lua_State* L) {
      int code = sqlite3_initialize();
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, code);
      }
    }

    void impl_shutdown(lua_State* L) {
      int code = sqlite3_shutdown();
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, code);
      }
    }

    void impl_open(lua_State* L) {
      const char* filename = luaL_checkstring(L, 1);
      int flags = luaX_opt_integer<int>(L, 2, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
      const char* vfs = lua_tostring(L, 3);
      sqlite3* dbh = 0;
      int code = sqlite3_open_v2(filename, &dbh, flags, vfs);
      if (code == SQLITE_OK) {
        new_dbh(L, dbh);
      } else {
        sqlite3_close(dbh);
        push_error(L, code);
      }
    }

    void initialize_core(lua_State* L) {
      luaX_set_field(L, -1, "initialize", impl_initialize);
      luaX_set_field(L, -1, "shutdown", impl_shutdown);
      luaX_set_field(L, -1, "open", impl_open);

      push_null(L);
      lua_setfield(L, -2, "null");

      luaX_set_field(L, -1, "SQLITE_OPEN_READONLY", SQLITE_OPEN_READONLY);
      luaX_set_field(L, -1, "SQLITE_OPEN_READWRITE", SQLITE_OPEN_READWRITE);
      luaX_set_field(L, -1, "SQLITE_OPEN_CREATE", SQLITE_OPEN_CREATE);
#ifdef SQLITE_OPEN_URI
      luaX_set_field(L, -1, "SQLITE_OPEN_URI", SQLITE_OPEN_URI);
#endif
#ifdef SQLITE_OPEN_MEMORY
      luaX_set_field(L, -1, "SQLITE_OPEN_MEMORY", SQLITE_OPEN_MEMORY);
#endif
      luaX_set_field(L, -1, "SQLITE_OPEN_NOMUTEX", SQLITE_OPEN_NOMUTEX);
      luaX_set_field(L, -1, "SQLITE_OPEN_FULLMUTEX", SQLITE_OPEN_FULLMUTEX);
      luaX_set_field(L, -1, "SQLITE_OPEN_SHAREDCACHE", SQLITE_OPEN_SHAREDCACHE);
      luaX_set_field(L, -1, "SQLITE_OPEN_PRIVATECACHE", SQLITE_OPEN_PRIVATECACHE);

      luaX_set_field(L, -1, "SQLITE_OK", SQLITE_OK);
      luaX_set_field(L, -1, "SQLITE_ROW", SQLITE_ROW);
      luaX_set_field(L, -1, "SQLITE_DONE", SQLITE_DONE);

      luaX_set_field(L, -1, "SQLITE_INTEGER", SQLITE_INTEGER);
      luaX_set_field(L, -1, "SQLITE_FLOAT", SQLITE_FLOAT);
      luaX_set_field(L, -1, "SQLITE_TEXT", SQLITE_TEXT);
      luaX_set_field(L, -1, "SQLITE_BLOB", SQLITE_BLOB);
      luaX_set_field(L, -1, "SQLITE_NULL", SQLITE_NULL);
    }
  }

  void initialize_dbh(lua_State* L);
  void initialize_sth(lua_State* L);
  void initialize_context(lua_State* L);

  void initialize(lua_State* L) {

    initialize_context(L);

    initialize_dbh(L);

    initialize_sth(L);

    open_entity(L);
    lua_setfield(L, -2, "entity");

    initialize_core(L);
  }
}

extern "C" int luaopen_dromozoa_sqlite3(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
