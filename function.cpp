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
#include "lua.h"
#include "lauxlib.h"
}

#include <sqlite3.h>

#include "dromozoa/bind.hpp"

#include "dbh.hpp"
#include "function.hpp"

namespace dromozoa {
  using bind::function;

  namespace {
    class Function {
    public:
      lua_State* L;
      int ref;
    };




    int impl_create_function(lua_State* L) {
      sqlite3* dbh = get_dbh(L, 1);
      const char* name = luaL_checkstring(L, 2);
      int nargs = luaL_checkinteger(L, 3);

      lua_pushstring(L, "dromozoa.sqlite3.function");
      lua_gettable(L, LUA_REGISTRYINDEX);
      lua_pushlightuserdata(L, dbh);
      lua_gettable(L, -2);
      lua_pushvalue(L, 4);
      int ref = luaL_ref(L, -2);
      lua_pop(L, 1);

      lua_pushinteger(L, ref);
      return 1;
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
