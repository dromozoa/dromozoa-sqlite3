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

namespace dromozoa {
  class database_handle;

  class function_handle {
    friend class database_handle;
  public:
    lua_State* get() const;
    int ref() const;
    int ref_step() const;
    int ref_final() const;
  private:
    lua_State* L_;
    int ref_;
    int ref_step_;
    int ref_final_;
    function_handle(lua_State* L, int ref);
    function_handle(lua_State* L, int ref_step, int ref_final);
    ~function_handle();
    function_handle(const function_handle&);
    function_handle& operator=(const function_handle&);
  };
}

#endif
