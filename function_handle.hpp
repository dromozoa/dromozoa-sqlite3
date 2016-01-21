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

#ifndef DROMOZOA_FUNCTION_HANDLE_HPP
#define DROMOZOA_FUNCTION_HANDLE_HPP

extern "C" {
#include <lua.h>
}

#include <sqlite3.h>

namespace dromozoa {
  class database_handle;

  class function_handle {
    friend class database_handle;
  public:
    function_handle(lua_State* L, int n);
    function_handle(lua_State* L, int n, int n_final);
    ~function_handle();
    void call_func(sqlite3_context* context, int argc, sqlite3_value** argv) const;
    void call_step(sqlite3_context* context, int argc, sqlite3_value** argv) const;
    void call_final(sqlite3_context* context) const;
    int call_exec(int count, char** columns, char** names) const;
  private:
    lua_State* L_;
    int ref_;
    int ref_final_;
    function_handle(const function_handle&);
    function_handle& operator=(const function_handle&);
  };
}

#endif
