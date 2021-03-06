// Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
    void impl_initialize(lua_State* L) {
      int result = sqlite3_initialize();
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }

    void impl_shutdown(lua_State* L) {
      int result = sqlite3_shutdown();
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }

    void impl_open(lua_State* L) {
      luaX_string_reference filename = luaX_check_string(L, 1);
      int flags = luaX_opt_integer<int>(L, 2, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
      luaX_string_reference vfs = luaX_to_string(L, 3);
      sqlite3* dbh = 0;
      int result = sqlite3_open_v2(filename.data(), &dbh, flags, vfs.data());
      if (result == SQLITE_OK) {
        luaX_new<database_handle_impl>(L, dbh);
        luaX_set_metatable(L, "dromozoa.sqlite3.dbh");
      } else {
        sqlite3_close(dbh);
        push_error(L, result);
      }
    }

    void impl_open_sharable(lua_State* L) {
      if (lua_islightuserdata(L, 1)) {
        luaX_new<database_handle_sharable>(L, static_cast<database_handle_sharable_impl*>(lua_touserdata(L, 1)));
        luaX_set_metatable(L, "dromozoa.sqlite3.dbh_sharable");
      } else {
        luaX_string_reference filename = luaX_check_string(L, 1);
        int flags = luaX_opt_integer<int>(L, 2, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        const char* vfs = lua_tostring(L, 3);
        scoped_ptr<database_handle_sharable_impl> impl(new database_handle_sharable_impl(filename.data(), flags, vfs));
        luaX_new<database_handle_sharable>(L, impl.get());
        impl.release();
        luaX_set_metatable(L, "dromozoa.sqlite3.dbh_sharable");
      }
    }

    void impl_libversion(lua_State* L) {
      luaX_push(L, sqlite3_libversion());
    }

    void impl_libversion_number(lua_State* L) {
      luaX_push(L, sqlite3_libversion_number());
    }

    void impl_sourceid(lua_State* L) {
      luaX_push(L, sqlite3_sourceid());
    }

    void impl_threadsafe(lua_State* L) {
      luaX_push(L, sqlite3_threadsafe());
    }

    void impl_backup_init(lua_State* L) {
      sqlite3* dest_dbh = check_dbh(L, 1);
      luaX_string_reference dest_name = luaX_check_string(L, 2);
      sqlite3* source_dbh = check_dbh(L, 3);
      luaX_string_reference source_name = luaX_check_string(L, 4);
      if (sqlite3_backup* backup = sqlite3_backup_init(dest_dbh, dest_name.data(), source_dbh, source_name.data())) {
        luaX_new<backup_handle>(L, backup, L, 1, 3);
        luaX_set_metatable(L, "dromozoa.sqlite3.backup");
      } else {
        push_error(L, dest_dbh);
      }
    }
  }

  void push_null(lua_State* L) {
    lua_pushlightuserdata(L, 0);
  }

  void initialize_main(lua_State* L) {
    luaX_set_field(L, -1, "initialize", impl_initialize);
    luaX_set_field(L, -1, "shutdown", impl_shutdown);
    luaX_set_field(L, -1, "open", impl_open);
    luaX_set_field(L, -1, "open_sharable", impl_open_sharable);
    luaX_set_field(L, -1, "libversion", impl_libversion);
    luaX_set_field(L, -1, "libversion_number", impl_libversion_number);
    luaX_set_field(L, -1, "sourceid", impl_sourceid);
    luaX_set_field(L, -1, "threadsafe", impl_threadsafe);
    luaX_set_field(L, -1, "backup_init", impl_backup_init);

    push_null(L);
    luaX_set_field(L, -2, "null");

    // Fundamental Datatypes
    luaX_set_field(L, -1, "SQLITE_INTEGER", SQLITE_INTEGER);
    luaX_set_field(L, -1, "SQLITE_FLOAT", SQLITE_FLOAT);
    luaX_set_field(L, -1, "SQLITE_TEXT", SQLITE_TEXT);
    luaX_set_field(L, -1, "SQLITE_BLOB", SQLITE_BLOB);
    luaX_set_field(L, -1, "SQLITE_NULL", SQLITE_NULL);

    // Result Codes
    luaX_set_field(L, -1, "SQLITE_OK", SQLITE_OK); // (0)
    luaX_set_field(L, -1, "SQLITE_ROW", SQLITE_ROW); // (100)
    luaX_set_field(L, -1, "SQLITE_DONE", SQLITE_DONE); // (101)

    // Flags For File Open Operations
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
  }
}
