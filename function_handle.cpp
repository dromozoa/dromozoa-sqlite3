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
  function_handle::function_handle(lua_State* L, int arg) : L_(L), ref_(LUA_NOREF), ref_final_(LUA_NOREF) {
    lua_pushvalue(L, arg);
    ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
  }

  function_handle::function_handle(lua_State* L, int arg, int arg_final) : L_(L), ref_(LUA_NOREF), ref_final_(LUA_NOREF) {
    lua_pushvalue(L, arg);
    ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, arg_final);
    ref_final_ = luaL_ref(L, LUA_REGISTRYINDEX);
  }

  function_handle::~function_handle() {
    luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
    luaL_unref(L_, LUA_REGISTRYINDEX, ref_final_);
  }

  namespace {
    void push_value(lua_State* L, sqlite3_value* value) {
      switch (sqlite3_value_type(value)) {
        case SQLITE_INTEGER:
          luaX_push(L, sqlite3_value_int64(value));
          break;
        case SQLITE_FLOAT:
          luaX_push(L, sqlite3_value_double(value));
          break;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_value_text(value))) {
            lua_pushlstring(L, text, sqlite3_value_bytes(value));
          } else {
            luaX_push(L, luaX_nil);
          }
          break;
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_value_blob(value))) {
            lua_pushlstring(L, blob, sqlite3_value_bytes(value));
          } else {
            luaX_push(L, luaX_nil);
          }
          break;
        case SQLITE_NULL:
          push_null(L);
          break;
        default:
          luaX_push(L, luaX_nil);
          break;
      }
    }

    void call(lua_State* L, int ref, sqlite3_context* context, int argc, sqlite3_value** argv) {
      int top = lua_gettop(L);
      luaX_push(L, ref);
      lua_gettable(L, LUA_REGISTRYINDEX);
      new_context(L, context);
      for (int i = 0; i < argc; ++i) {
        push_value(L, argv[i]);
      }
      if (lua_pcall(L, argc + 1, 0, 0) != 0) {
        sqlite3_result_error(context, lua_tostring(L, -1), -1);
      }
      lua_settop(L, top);
    }
  }

  void function_handle::call_func(sqlite3_context* context, int argc, sqlite3_value** argv) const {
    call(L_, ref_, context, argc, argv);
  }

  void function_handle::call_final(sqlite3_context* context) const {
    call(L_, ref_final_, context, 0, 0);
  }

  int function_handle::call_exec(int count, char** columns, char** names) const {
    lua_State* L = L_;
    int top = lua_gettop(L);
    luaX_push(L, ref_);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_newtable(L);
    for (int i = 0; i < count; ++i) {
      if (columns[i]) {
        luaX_push(L, columns[i]);
      } else {
        push_null(L);
      }
      lua_pushvalue(L, -2); // [TODO] fix?
      luaX_set_field(L, -3, i + 1);
      luaX_set_field(L, -2, names[i]);
    }
    int result = 0;
    if (lua_pcall(L, 1, 1, 0) != 0) {
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
