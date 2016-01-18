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

#include "dbh.hpp"
#include "context.hpp"
#include "error.hpp"
#include "database_handle.hpp"
#include "function.hpp"
#include "function_handle.hpp"

namespace dromozoa {
  using bind::function;
  using bind::get_log_level;
  using bind::push_success;

  namespace {
    int push_value(lua_State* L, sqlite3_value* value) {
      switch (sqlite3_value_type(value)) {
        case SQLITE_INTEGER:
          lua_pushinteger(L, sqlite3_value_int64(value));
          return 1;
        case SQLITE_FLOAT:
          lua_pushnumber(L, sqlite3_value_double(value));
          return 1;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_value_text(value))) {
            lua_pushlstring(L, text, sqlite3_value_bytes(value));
            return 1;
          } else {
            return 0;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_value_blob(value))) {
            lua_pushlstring(L, blob, sqlite3_value_bytes(value));
            return 1;
          } else {
            return 0;
          }
        case SQLITE_NULL:
          return 0;
      }
      return 0;
    }

    void impl_func(sqlite3_context* context, int argc, sqlite3_value** argv) {
      function_handle& f = *static_cast<function_handle*>(sqlite3_user_data(context));
      lua_State* L = f.get();
      int top = lua_gettop(L);

      lua_pushinteger(L, f.ref());
      lua_gettable(L, LUA_REGISTRYINDEX);

      new_context(L, context);

      for (int i = 0; i < argc; ++i) {
        if (push_value(L, argv[i]) == 0) {
          lua_pushnil(L);
        }
      }

      int result = lua_pcall(L, argc + 1, 0, 0);
      if (result != LUA_OK) {
        sqlite3_result_error(context, lua_tostring(L, -1), -1);
      }
      lua_settop(L, top);
    }

    // void impl_step(sqlite3_context* context, int argc, sqlite3_value** argv) {
    // }

    // void impl_final(sqlite3_context* context) {
    // }

    int impl_create_function(lua_State* L) {
      database_handle& d = get_database_handle(L, 1);
      sqlite3* dbh = d.get();

      const char* name = luaL_checkstring(L, 2);
      int narg = luaL_checkinteger(L, 3);
      lua_pushvalue(L, 4);
      int ref = luaL_ref(L, LUA_REGISTRYINDEX);

      function_handle* f = d.new_function(L, ref);
      if (get_log_level() > 2) {
        std::cerr << "[dromozoa-sqlite3] new function " << f << std::endl;
      }

      int code = sqlite3_create_function_v2(dbh, name, narg, SQLITE_UTF8, f, impl_func, 0, 0, 0);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, dbh);
      }
    }

    // int impl_create_aggregate(lua_State* L) {
    //   return 0;
    // }
  }

  void initialize_function(lua_State* L) {
    function<impl_create_function>::set_field(L, "create_function");
  }
}
