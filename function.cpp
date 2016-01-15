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

#include "dromozoa/bind.hpp"

#include "dbh.hpp"
#include "error.hpp"
#include "database_handle.hpp"
#include "function.hpp"
#include "function_handle.hpp"

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  namespace {
    void impl_func(sqlite3_context* context, int argc, sqlite3_value** argv) {
    }

    void impl_step(sqlite3_context* context, int argc, sqlite3_value** argv) {
    }

    void impl_final(sqlite3_context* context) {
    }

    void impl_destroy(void* data) {
    }

    int impl_create_function(lua_State* L) {
      database_handle& d = get_database_handle(L, 1);
      sqlite3* dbh = d.get();

      const char* name = luaL_checkstring(L, 2);
      int narg = luaL_checkinteger(L, 3);
      lua_pushvalue(L, 4);
      int ref = luaL_ref(L, -2);

      int code = sqlite3_create_function_v2(
          dbh,
          name,
          narg,
          SQLITE_UTF8,
          d.new_function(L, ref),
          impl_func,
          0,
          0,
          impl_destroy);
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
    lua_pushstring(L, "dromozoa.sqlite3.function");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
    function<impl_create_function>::set_field(L, "create_function");
  }
}
