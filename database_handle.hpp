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

#ifndef DROMOZOA_DATABASE_HANDLE_HPP
#define DROMOZOA_DATABASE_HANDLE_HPP

extern "C" {
#include <lua.h>
}

#include <sqlite3.h>

#include <set>

namespace dromozoa {
  class function_handle;

  class database_handle {
  public:
    database_handle(sqlite3* dbh);
    ~database_handle();
    int close();
    sqlite3* get() const;
    function_handle* new_function(lua_State* L, int ref);
    function_handle* new_function(lua_State* L, int ref_step, int ref_final);
    void delete_function(function_handle* function);
  private:
    sqlite3* dbh_;
    std::set<function_handle*> function_;
  };

  database_handle& get_database_handle(lua_State* L, int n);
}

#endif
