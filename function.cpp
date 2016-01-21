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
  using bind::get_log_level;
  using bind::push_success;

  namespace {
    void cb_func(sqlite3_context* context, int argc, sqlite3_value** argv) {
      function_handle& f = *static_cast<function_handle*>(sqlite3_user_data(context));
      f.call_func(context, argc, argv);
    }

    void cb_step(sqlite3_context* context, int argc, sqlite3_value** argv) {
      function_handle& f = *static_cast<function_handle*>(sqlite3_user_data(context));
      f.call_func(context, argc, argv);
    }

    void cb_final(sqlite3_context* context) {
      function_handle& f = *static_cast<function_handle*>(sqlite3_user_data(context));
      f.call_final(context);
    }

    int impl_create_function(lua_State* L) {
      database_handle& d = get_database_handle(L, 1);
      sqlite3* dbh = d.get();
      const char* name = luaL_checkstring(L, 2);
      int narg = luaL_checkinteger(L, 3);
      function_handle* f = d.new_function(L, 4);
      int code = sqlite3_create_function(dbh, name, narg, SQLITE_UTF8, f, cb_func, 0, 0);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, dbh);
      }
    }

    int impl_create_aggregate(lua_State* L) {
      database_handle& d = get_database_handle(L, 1);
      sqlite3* dbh = d.get();
      const char* name = luaL_checkstring(L, 2);
      int narg = luaL_checkinteger(L, 3);
      function_handle* f = d.new_aggregate(L, 4, 5);
      int code = sqlite3_create_function(dbh, name, narg, SQLITE_UTF8, f, 0, cb_step, cb_final);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, dbh);
      }
    }
  }

  void initialize_function(lua_State* L) {
    function<impl_create_function>::set_field(L, "create_function");
    function<impl_create_aggregate>::set_field(L, "create_aggregate");
  }
}
