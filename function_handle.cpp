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

#include "context.hpp"
#include "database_handle.hpp"
#include "function_handle.hpp"

namespace dromozoa {
  function_handle::function_handle(lua_State* L, int ref)
    : L_(L), ref_(ref), ref_final_(LUA_NOREF) {}

  function_handle::function_handle(lua_State* L, int ref, int ref_final)
    : L_(L), ref_(ref), ref_final_(ref_final) {}

  function_handle::~function_handle() {
    luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
    luaL_unref(L_, LUA_REGISTRYINDEX, ref_final_);
  }

  namespace {
    int push_value(lua_State* L, sqlite3_value* value) {
      switch (sqlite3_value_type(value)) {
        case SQLITE_INTEGER:
          lua_pushinteger(L, sqlite3_value_int64(value));
          return 1;
        case SQLITE_FLOAT:
          lua_pushnumber(L, sqlite3_value_double(value));
          return 1;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_value_text(value))) {
            lua_pushlstring(L, text, sqlite3_value_bytes(value));
            return 1;
          } else {
            return 0;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_value_blob(value))) {
            lua_pushlstring(L, blob, sqlite3_value_bytes(value));
            return 1;
          } else {
            return 0;
          }
        case SQLITE_NULL:
          return 0;
        default:
          return 0;
      }
    }

    void call(lua_State* L, int ref, sqlite3_context* context, int argc, sqlite3_value** argv) {
      int top = lua_gettop(L);
      lua_pushinteger(L, ref);
      lua_gettable(L, LUA_REGISTRYINDEX);
      new_context(L, context);
      for (int i = 0; i < argc; ++i) {
        if (push_value(L, argv[i]) == 0) {
          lua_pushnil(L);
        }
      }
      if (lua_pcall(L, argc + 1, 0, 0) != LUA_OK) {
        sqlite3_result_error(context, lua_tostring(L, -1), -1);
      }
      lua_settop(L, top);
    }
  }

  void function_handle::call_func(sqlite3_context* context, int argc, sqlite3_value** argv) const {
    call(L_, ref_, context, argc, argv);
  }

  void function_handle::call_step(sqlite3_context* context, int argc, sqlite3_value** argv) const {
    call(L_, ref_, context, argc, argv);
  }

  void function_handle::call_final(sqlite3_context* context) const {
    call(L_, ref_final_, context, 0, 0);
  }

  int function_handle::call_exec(int count, char** columns, char** names) const {
    lua_State* L = L_;
    int top = lua_gettop(L);
    lua_pushinteger(L, ref_);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_newtable(L);
    for (int i = 0; i < count; ++i) {
      if (columns[i]) {
        lua_pushstring(L, columns[i]);
        lua_pushinteger(L, i + 1);
        lua_pushvalue(L, -2);
        lua_settable(L, -4);
        lua_setfield(L, -2, names[i]);
      }
    }
    int result = 0;
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
      result = 1;
    } else {
      if (lua_isnumber(L, -1)) {
        result = lua_tonumber(L, -1);
      }
    }
    lua_settop(L, top);
    return result;
  }
}
