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

#include "database_handle.hpp"
#include "function_handle.hpp"

namespace dromozoa {
  database_handle::database_handle(sqlite3* dbh) : dbh_(dbh) {}

  database_handle::~database_handle() {
    close();
  }

  int database_handle::close() {
    std::set<function_handle*>::iterator i = function_.begin();
    std::set<function_handle*>::iterator end = function_.end();
    for (; i != end; ++i) {
      delete *i;
    }
    function_.clear();

    sqlite3* dbh = dbh_;
    dbh_ = 0;
#if SQLITE_VERSION_NUMBER >= 3007014
    return sqlite3_close_v2(dbh);
#else
    return sqlite3_close(dbh);
#endif
  }

  sqlite3* database_handle::get() const {
    return dbh_;
  }

  function_handle* database_handle::new_function(lua_State* L, int ref) {
    function_handle* function = 0;
    try {
      function = new function_handle(L, ref);
      function_.insert(function);
      return function;
    } catch (...) {
      delete function;
      throw;
    }
  }

  function_handle* database_handle::new_function(lua_State* L, int ref_step, int ref_final) {
    function_handle* function = 0;
    try {
      function = new function_handle(L, ref_step, ref_final);
      function_.insert(function);
      return function;
    } catch (...) {
      delete function;
      throw;
    }
  }

  database_handle& get_database_handle(lua_State* L, int n) {
    return *static_cast<database_handle*>(luaL_checkudata(L, n, "dromozoa.sqlite3.dbh"));
  }
}
