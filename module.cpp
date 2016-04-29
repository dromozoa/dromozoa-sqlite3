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
    void initialize_entity(lua_State* L) {
      lua_getglobal(L, "require");
      luaX_push(L, "dromozoa.sqlite3.entity");
      lua_call(L, 1, 1);
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "super");
      luaX_set_field(L, -2, "entity");
    }
  }

  void initialize_context(lua_State* L);
  void initialize_dbh(lua_State* L);
  void initialize_main(lua_State* L);
  void initialize_sth(lua_State* L);

  void initialize(lua_State* L) {
    initialize_context(L);
    initialize_dbh(L);
    initialize_main(L);
    initialize_sth(L);
    initialize_entity(L);
  }
}

extern "C" int luaopen_dromozoa_sqlite3(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
