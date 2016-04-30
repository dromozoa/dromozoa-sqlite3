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
    function_handle* get_function_handle(sqlite3_context* context) {
      return static_cast<function_handle*>(sqlite3_user_data(context));
    }

    void cb_func(sqlite3_context* context, int argc, sqlite3_value** argv) {
      get_function_handle(context)->call_func(context, argc, argv);
    }

    void cb_step(sqlite3_context* context, int argc, sqlite3_value** argv) {
      get_function_handle(context)->call_func(context, argc, argv);
    }

    void cb_final(sqlite3_context* context) {
      get_function_handle(context)->call_final(context);
    }

    void impl_create_function(lua_State* L) {
      database_handle* self = check_database_handle(L, 1);
      sqlite3* dbh = self->get();
      const char* name = luaL_checkstring(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      function_handle* function = self->new_function(L, 4);
      if (sqlite3_create_function(dbh, name, narg, SQLITE_UTF8, function, cb_func, 0, 0) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, dbh);
      }
    }

    void impl_create_aggregate(lua_State* L) {
      database_handle* self = check_database_handle(L, 1);
      sqlite3* dbh = self->get();
      const char* name = luaL_checkstring(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      function_handle* function = self->new_aggregate(L, 4, 5);
      if (sqlite3_create_function(dbh, name, narg, SQLITE_UTF8, function, 0, cb_step, cb_final) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, dbh);
      }
    }
  }

  void initialize_dbh_function(lua_State* L) {
    luaX_set_field(L, -1, "create_function", impl_create_function);
    luaX_set_field(L, -1, "create_aggregate", impl_create_aggregate);
  }
}
