/*
 * Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
 *
 * This file is part of dromozoa-bind.
 *
 * dromozoa-bind is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dromozoa-bind is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void error(lua_State* L) {
  const char* what = lua_tostring(L, -1);
  if (!what) {
    what = "(null)";
  }
  fprintf(stderr, "%s\n", what);
  exit(1);
}

void initialize(lua_State* L, const char* filename, int argc, char* argv[]) {
  int i;
  luaL_openlibs(L);
  lua_pushboolean(L, 1);
  lua_setglobal(L, "dromozoa.bind.driver");
  lua_newtable(L);
  for (i = 0; i < argc; ++i) {
    lua_pushinteger(L, i - 1);
    lua_pushstring(L, argv[i]);
    lua_settable(L, -3);
  }
  lua_setglobal(L, "arg");
  if (luaL_loadfile(L, filename) != 0) {
    error(L);
  }
  for (i = 2; i < argc; ++i) {
    lua_pushstring(L, argv[i]);
  }
  if (lua_pcall(L, argc - 2, 1, 0) != 0) {
    error(L);
  }
  lua_pushstring(L, "dromozoa.bind.hook");
  lua_pushvalue(L, -2);
  lua_settable(L, LUA_REGISTRYINDEX);
  lua_pop(L, 1);
}

void run(lua_State* L) {
  lua_pushstring(L, "dromozoa.bind.hook");
  lua_gettable(L, LUA_REGISTRYINDEX);
  if (!lua_isnil(L, -1)) {
    if (lua_pcall(L, 0, 0, 0) != 0) {
      error(L);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 1) {
    return 1;
  }

  const char* filename = argv[1];

  lua_State* L1 = luaL_newstate();
  initialize(L1, filename, argc, argv);
  run(L1);

  lua_State* L2 = luaL_newstate();
  initialize(L2, filename, argc, argv);
  run(L2);

  run(L1);
  lua_close(L1);

  run(L2);
  lua_close(L2);
  return 0;
}
