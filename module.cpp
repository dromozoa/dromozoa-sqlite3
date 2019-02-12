// Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-bind.
//
// dromozoa-bind is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-bind is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>

#include "dromozoa/bind.hpp"
#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_gc(lua_State*) {
      if (verbose()) {
        std::cout << "[VERBOSE] close dromozoa.bind\n";
      }
    }
  }

  void initialize_atomic(lua_State*);
  void initialize_callback(lua_State*);
  void initialize_core(lua_State*);
  void initialize_handle(lua_State*);
  void initialize_mutex(lua_State*);
  void initialize_scoped_ptr(lua_State*);
  void initialize_system_error(lua_State*);
  void initialize_thread(lua_State*);
  void initialize_util(lua_State*);

  void initialize(lua_State* L) {
    luaL_newmetatable(L, "dromozoa.bind");
    luaX_set_field(L, -1, "__gc", impl_gc);
    lua_pop(L, 1);
    luaX_set_metatable(L, "dromozoa.bind");

    static int count = 0;
    luaX_set_field(L, -1, "count", ++count);

    initialize_atomic(L);
    initialize_callback(L);
    initialize_core(L);
    initialize_handle(L);
    initialize_mutex(L);
    initialize_scoped_ptr(L);
    initialize_system_error(L);
    initialize_thread(L);
    initialize_util(L);
  }
}

extern "C" int luaopen_dromozoa_bind(lua_State* L) {
  if (dromozoa::verbose()) {
    std::cout << "[VERBOSE] open dromozoa.bind\n";
  }
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
