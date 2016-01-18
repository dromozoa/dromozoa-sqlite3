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

#include <sqlite3.h>

#include <iostream>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "sth.hpp"

namespace dromozoa {
  using bind::function;
  using bind::get_log_level;
  using bind::push_success;
  using bind::translate_range_i;
  using bind::translate_range_j;

  int new_sth(lua_State* L, sqlite3_stmt* sth) {
    *static_cast<sqlite3_stmt**>(lua_newuserdata(L, sizeof(sth))) = sth;
    luaL_getmetatable(L, "dromozoa.sqlite3.sth");
    lua_setmetatable(L, -2);
    if (get_log_level() > 2) {
      std::cerr << "[dromozoa-sqlite3] new sth " << sth << std::endl;
    }
    return 1;
  }

  sqlite3_stmt* get_sth(lua_State* L, int n) {
    return *static_cast<sqlite3_stmt**>(luaL_checkudata(L, n, "dromozoa.sqlite3.sth"));
  }

  namespace {
    int check_bind_parameter_index(sqlite3_stmt* sth, lua_State* L, int n) {
      if (lua_isnumber(L, n)) {
        return luaL_checkinteger(L, n);
      } else {
        return sqlite3_bind_parameter_index(sth, luaL_checkstring(L, n));
      }
    }

    int push_column(lua_State* L, sqlite3_stmt* sth, int i) {
      switch (sqlite3_column_type(sth, i)) {
        case SQLITE_INTEGER:
          lua_pushinteger(L, sqlite3_column_int64(sth, i));
          return 1;
        case SQLITE_FLOAT:
          lua_pushnumber(L, sqlite3_column_double(sth, i));
          return 1;
        case SQLITE_TEXT:
          if (const char* text = reinterpret_cast<const char*>(sqlite3_column_text(sth, i))) {
            lua_pushlstring(L, text, sqlite3_column_bytes(sth, i));
            return 1;
          } else {
            return 0;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_column_blob(sth, i))) {
            lua_pushlstring(L, blob, sqlite3_column_bytes(sth, i));
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

    int impl_finalize(lua_State* L) {
      sqlite3_stmt** data = static_cast<sqlite3_stmt**>(luaL_checkudata(L, 1, "dromozoa.sqlite3.sth"));
      sqlite3_stmt* sth = *data;
      int code = sqlite3_finalize(sth);
      if (code == SQLITE_OK) {
        *data = 0;
        if (get_log_level() > 2) {
          std::cerr << "[dromozoa-sqlite3] finalize sth " << sth << std::endl;
        }
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_gc(lua_State* L) {
      sqlite3_stmt** data = static_cast<sqlite3_stmt**>(luaL_checkudata(L, 1, "dromozoa.sqlite3.sth"));
      sqlite3_stmt* sth = *data;
      *data = 0;
      if (sth) {
        if (get_log_level() > 1) {
          std::cerr << "[dromozoa-sqlite3] sth " << sth << " detected" << std::endl;
        }
        int code = sqlite3_finalize(sth);
        if (code == SQLITE_OK) {
          if (get_log_level() > 2) {
            std::cerr << "[dromozoa-sqlite3] finalize sth " << sth << std::endl;
          }
        } else {
          if (get_log_level() > 0) {
            std::cerr << "[dromozoa-sqlite3] cannot finalize sth " << sth << ": ";
            print_error(std::cerr, code);
            std::cerr << std::endl;
          }
        }
      }
      return 0;
    }

    int impl_step(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int code = sqlite3_step(sth);
      if (code == SQLITE_ROW || code == SQLITE_DONE) {
        lua_pushinteger(L, code);
        return 1;
      } else {
        return push_error(L, sth);
      }
    }

    int impl_reset(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int code = sqlite3_reset(sth);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_parameter_count(lua_State* L) {
      lua_pushinteger(L, sqlite3_bind_parameter_count(get_sth(L, 1)));
      return 1;
    }

    int impl_bind_parameter_index(lua_State* L) {
      const char* name = luaL_checkstring(L, 2);
      lua_pushinteger(L, sqlite3_bind_parameter_index(get_sth(L, 1), name));
      return 1;
    }

    int impl_bind_parameter_name(lua_State* L) {
      int index = luaL_checkinteger(L, 2);
      lua_pushstring(L, sqlite3_bind_parameter_name(get_sth(L, 1), index));
      return 1;
    }

    int impl_bind_int64(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int index = check_bind_parameter_index(sth, L, 2);
      lua_Integer value = luaL_checkinteger(L, 3);
      int code = sqlite3_bind_int64(sth, index, value);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_double(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int index = check_bind_parameter_index(sth, L, 2);
      lua_Number value = luaL_checknumber(L, 3);
      int code = sqlite3_bind_double(sth, index, value);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_text(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int index = check_bind_parameter_index(sth, L, 2);
      size_t size = 0;
      const char* text = luaL_checklstring(L, 3, &size);
      size_t i = translate_range_i(L, 4, size);
      size_t j = translate_range_j(L, 5, size);
      int code;
      if (i < j) {
        code = sqlite3_bind_text(sth, index, text, size, SQLITE_TRANSIENT);
      } else {
        code = sqlite3_bind_text(sth, index, "", 0, SQLITE_STATIC);
      }
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_blob(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int index = check_bind_parameter_index(sth, L, 2);
      size_t size = 0;
      const char* blob = luaL_checklstring(L, 3, &size);
      size_t i = translate_range_i(L, 4, size);
      size_t j = translate_range_j(L, 5, size);
      int code;
      if (i < j) {
        code = sqlite3_bind_blob(sth, index, blob + i, j - i, SQLITE_TRANSIENT);
      } else {
        code = sqlite3_bind_zeroblob(sth, index, 0);
      }
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_null(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int index = check_bind_parameter_index(sth, L, 2);
      int code = sqlite3_bind_null(sth, index);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_clear_bindings(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int code = sqlite3_clear_bindings(sth);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_column_count(lua_State* L) {
      lua_pushinteger(L, sqlite3_column_count(get_sth(L, 1)));
      return 1;
    }

    int impl_column_name(lua_State* L) {
      int i = luaL_checkinteger(L, 2) - 1;
      lua_pushstring(L, sqlite3_column_name(get_sth(L, 1), i));
      return 1;
    }

    int impl_column_type(lua_State* L) {
      int i = luaL_checkinteger(L, 2) - 1;
      int type = sqlite3_column_type(get_sth(L, 1), i);
      lua_pushinteger(L, type);
      return 1;
    }

    int impl_column(lua_State* L) {
      int i = luaL_checkinteger(L, 2) - 1;
      return push_column(L, get_sth(L, 1), i);
    }

    int impl_columns(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int count = sqlite3_column_count(sth);
      lua_newtable(L);
      for (int i = 0; i < count; ++i) {
        if (push_column(L, sth, i) == 1) {
          lua_pushinteger(L, i + 1);
          lua_pushvalue(L, -2);
          lua_settable(L, -4);
          lua_setfield(L, -2, sqlite3_column_name(sth, i));
        }
      }
      return 1;
    }

    int impl_sql(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      lua_pushstring(L, sqlite3_sql(sth));
      return 1;
    }
  }

  int open_sth(lua_State* L) {
    lua_newtable(L);
    function<impl_finalize>::set_field(L, "finalize");
    function<impl_step>::set_field(L, "step");
    function<impl_reset>::set_field(L, "reset");
    function<impl_bind_parameter_count>::set_field(L, "bind_parameter_count");
    function<impl_bind_parameter_index>::set_field(L, "bind_parameter_index");
    function<impl_bind_parameter_name>::set_field(L, "bind_parameter_name");
    function<impl_bind_int64>::set_field(L, "bind_int64");
    function<impl_bind_double>::set_field(L, "bind_double");
    function<impl_bind_text>::set_field(L, "bind_text");
    function<impl_bind_blob>::set_field(L, "bind_blob");
    function<impl_bind_null>::set_field(L, "bind_null");
    function<impl_clear_bindings>::set_field(L, "clear_bindings");
    function<impl_column_count>::set_field(L, "column_count");
    function<impl_column_name>::set_field(L, "column_name");
    function<impl_column_type>::set_field(L, "column_type");
    function<impl_column>::set_field(L, "column");
    function<impl_columns>::set_field(L, "columns");
    function<impl_sql>::set_field(L, "sql");
    luaL_newmetatable(L, "dromozoa.sqlite3.sth");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    return 1;
  }
}
