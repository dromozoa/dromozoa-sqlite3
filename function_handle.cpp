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
    bool push_value(lua_State* L, sqlite3_value* value) {
      switch (sqlite3_value_type(value)) {
        case SQLITE_INTEGER:
          luaX_push(L, sqlite3_value_int64(value));
          return true;
        case SQLITE_FLOAT:
          luaX_push(L, sqlite3_value_double(value));
          return true;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_value_text(value))) {
            lua_pushlstring(L, text, sqlite3_value_bytes(value));
            return true;
          } else {
            return false;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_value_blob(value))) {
            lua_pushlstring(L, blob, sqlite3_value_bytes(value));
            return true;
          } else {
            return false;
          }
        case SQLITE_NULL:
          push_null(L);
          return true;
        default:
          return false;
      }
    }

    void call(lua_State* L, int ref, sqlite3_context* context, int argc, sqlite3_value** argv) {
      int top = lua_gettop(L);
      luaX_get_field(L, LUA_REGISTRYINDEX, ref);
      new_context(L, context);
      for (int i = 0; i < argc; ++i) {
        if (!push_value(L, argv[i])) {
          luaX_push(L, luaX_nil);
        }
      }
      if (lua_pcall(L, argc + 1, 0, 0) != 0) {
        sqlite3_result_error(context, lua_tostring(L, -1), -1);
      }
      lua_settop(L, top);
    }
  }

  function_handle::function_handle(lua_State* L, int arg_func) : L_(L), ref_func_(LUA_NOREF), ref_final_(LUA_NOREF) {
    lua_pushvalue(L, arg_func);
    ref_func_ = luaL_ref(L, LUA_REGISTRYINDEX);
  }

  function_handle::function_handle(lua_State* L, int arg_func, int arg_final) : L_(L), ref_func_(LUA_NOREF), ref_final_(LUA_NOREF) {
    lua_pushvalue(L, arg_func);
    ref_func_ = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, arg_final);
    ref_final_ = luaL_ref(L, LUA_REGISTRYINDEX);
  }

  function_handle::~function_handle() {
    luaL_unref(L_, LUA_REGISTRYINDEX, ref_func_);
    luaL_unref(L_, LUA_REGISTRYINDEX, ref_final_);
  }

  void function_handle::call_func(sqlite3_context* context, int argc, sqlite3_value** argv) const {
    call(L_, ref_func_, context, argc, argv);
  }

  void function_handle::call_final(sqlite3_context* context) const {
    call(L_, ref_final_, context, 0, 0);
  }

  int function_handle::call_exec(int count, char** columns, char** names) const {
    int top = lua_gettop(L_);
    luaX_get_field(L_, LUA_REGISTRYINDEX, ref_func_);
    lua_newtable(L_);
    for (int i = 0; i < count; ++i) {
      if (columns[i]) {
        luaX_push(L_, columns[i]);
      } else {
        push_null(L_);
      }
      lua_pushvalue(L_, -1);
      luaX_set_field(L_, -3, i + 1);
      luaX_set_field(L_, -2, names[i]);
    }
    int result = 0;
    if (lua_pcall(L_, 1, 1, 0) != 0) {
      result = 1;
    } else {
      if (lua_isnumber(L_, -1)) {
        result = lua_tointeger(L_, -1);
      }
    }
    lua_settop(L_, top);
    return result;
  }
}
