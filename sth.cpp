// Copyright (C) 2016,2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
    statement_handle* check_statement_handle(lua_State* L, int arg) {
      return luaX_check_udata<statement_handle>(L, arg, "dromozoa.sqlite3.sth");
    }

    void impl_gc(lua_State* L) {
      check_statement_handle(L, 1)->~statement_handle();
    }

    void impl_finalize(lua_State* L) {
      check_statement_handle(L, 1)->finalize();
      luaX_push_success(L);
    }

    void impl_step(lua_State* L) {
      sqlite3_stmt* sth = check_sth(L, 1);
      int result = sqlite3_step(sth);
      if (result == SQLITE_ROW || result == SQLITE_DONE) {
        luaX_push(L, result);
      } else {
        push_error(L, sth);
      }
    }

    void impl_reset(lua_State* L) {
      sqlite3_reset(check_sth(L, 1));
      luaX_push_success(L);
    }

    void impl_sql(lua_State* L) {
      luaX_push(L, sqlite3_sql(check_sth(L, 1)));
    }

    void impl_data_count(lua_State* L) {
      luaX_push(L, sqlite3_data_count(check_sth(L, 1)));
    }
  }

  sqlite3_stmt* check_sth(lua_State* L, int arg) {
    return check_statement_handle(L, arg)->get();
  }

  void initialize_sth_bind(lua_State* L);
  void initialize_sth_column(lua_State* L);

  void initialize_sth(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.sqlite3.sth");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "finalize", impl_finalize);
      luaX_set_field(L, -1, "step", impl_step);
      luaX_set_field(L, -1, "reset", impl_reset);
      luaX_set_field(L, -1, "sql", impl_sql);
      luaX_set_field(L, -1, "data_count", impl_data_count);

      initialize_sth_bind(L);
      initialize_sth_column(L);
    }
    luaX_set_field(L, -2, "sth");
  }
}
