// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

    template <class T>
    void callback(T* ref, size_t i, sqlite3_context* context, int argc, sqlite3_value** argv) {
      lua_State* L = ref->state();
      int top = lua_gettop(L);
      {
        ref->get_field(L, i);
        new_context(L, context);
        for (int i = 0; i < argc; ++i) {
          if (!push_value(L, argv[i])) {
            luaX_push(L, luaX_nil);
          }
        }
        if (lua_pcall(L, argc + 1, 0, 0) != 0) {
          sqlite3_result_error(context, lua_tostring(L, -1), -1);
        }
      }
      lua_settop(L, top);
    }

    void func_callback(sqlite3_context* context, int argc, sqlite3_value** argv) {
      callback(static_cast<luaX_reference<>*>(sqlite3_user_data(context)), 0, context, argc, argv);
    }

    void step_callback(sqlite3_context* context, int argc, sqlite3_value** argv) {
      callback(static_cast<luaX_reference<2>*>(sqlite3_user_data(context)), 0, context, argc, argv);
    }

    void final_callback(sqlite3_context* context) {
      callback(static_cast<luaX_reference<2>*>(sqlite3_user_data(context)), 1, context, 0, 0);
    }

    int exec_callback(void* data, int count, char** columns, char** names) {
      luaX_reference<>* ref = static_cast<luaX_reference<>*>(data);
      lua_State* L = ref->state();
      int result = 0;
      int top = lua_gettop(L);
      {
        ref->get_field(L);
        lua_newtable(L);
        for (int i = 0; i < count; ++i) {
          if (columns[i]) {
            luaX_push(L, columns[i]);
          } else {
            push_null(L);
          }
          lua_pushvalue(L, -1);
          luaX_set_field(L, -3, i + 1);
          luaX_set_field(L, -2, names[i]);
        }
        if (lua_pcall(L, 1, 1, 0) == 0) {
          if (luaX_is_integer(L, -1)) {
            result = lua_tointeger(L, -1);
          }
        } else {
          DROMOZOA_UNEXPECTED(lua_tostring(L, -1));
          result = 1;
        }
      }
      lua_settop(L, top);
      return result;
    }

    void impl_create_function(lua_State* L) {
      database_handle* self = check_database_handle(L, 1);
      const char* name = luaL_checkstring(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      luaL_checkany(L, 4);
      luaX_reference<>* function = self->new_function(name, narg, L, 4);
      if (sqlite3_create_function(self->get(), name, narg, SQLITE_UTF8, function, func_callback, 0, 0) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, self->get());
      }
    }

    void impl_create_aggregate(lua_State* L) {
      database_handle* self = check_database_handle(L, 1);
      const char* name = luaL_checkstring(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      luaL_checkany(L, 4);
      luaL_checkany(L, 5);
      luaX_reference<2>* aggregate = self->new_aggregate(name, narg, L, 4, 5);
      if (sqlite3_create_function(self->get(), name, narg, SQLITE_UTF8, aggregate, 0, step_callback, final_callback) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, self->get());
      }
    }

    void impl_delete_function(lua_State* L) {
      database_handle* self = check_database_handle(L, 1);
      const char* name = luaL_checkstring(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      if (sqlite3_create_function(self->get(), name, narg, SQLITE_UTF8, 0, 0, 0, 0) == SQLITE_OK) {
        self->delete_function(name, narg);
        luaX_push_success(L);
      } else {
        push_error(L, self->get());
      }
    }

    void impl_exec(lua_State* L) {
      sqlite3* dbh = check_dbh(L, 1);
      const char* sql = luaL_checkstring(L, 2);
      int code = SQLITE_ERROR;
      if (lua_isnoneornil(L, 3)) {
        code = sqlite3_exec(dbh, sql, 0, 0, 0);
      } else {
        luaX_reference<> reference(L, 3);
        code = sqlite3_exec(dbh, sql, exec_callback, &reference, 0);
      }
      if (code == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, dbh);
      }
    }
  }

  void initialize_dbh_function(lua_State* L) {
    luaX_set_field(L, -1, "create_function", impl_create_function);
    luaX_set_field(L, -1, "create_aggregate", impl_create_aggregate);
    luaX_set_field(L, -1, "delete_function", impl_delete_function);
    luaX_set_field(L, -1, "exec", impl_exec);
  }
}
