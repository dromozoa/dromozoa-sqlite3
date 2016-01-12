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
#include "lua.h"
#include "lauxlib.h"
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
      int i = luaL_checkinteger(L, 2);
      lua_pushstring(L, sqlite3_bind_parameter_name(get_sth(L, 1), i));
      return 1;
    }

    int impl_bind_int64(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int i = luaL_checkinteger(L, 2);
      lua_Integer v = luaL_checkinteger(L, 3);
      int code = sqlite3_bind_int64(sth, i, v);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_double(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int i = luaL_checkinteger(L, 2);
      lua_Number v = luaL_checknumber(L, 3);
      int code = sqlite3_bind_double(sth, i, v);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_text(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int i = luaL_checkinteger(L, 2);
      size_t size = 0;
      const char* text = luaL_checklstring(L, 3, &size);
      int code = sqlite3_bind_text(sth, i, text, size, SQLITE_TRANSIENT);
      if (code == SQLITE_OK) {
        return push_success(L);
      } else {
        return push_error(L, sth);
      }
    }

    int impl_bind_null(lua_State* L) {
      sqlite3_stmt* sth = get_sth(L, 1);
      int i = luaL_checkinteger(L, 2);
      int code = sqlite3_bind_null(sth, i);
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
      lua_pushinteger(L, sqlite3_column_type(get_sth(L, 1), i));
      return 1;
    }

    int impl_column_int64(lua_State* L) {
      int i = luaL_checkinteger(L, 2) - 1;
      lua_pushinteger(L, sqlite3_column_int64(get_sth(L, 1), i));
      return 1;
    }

    int impl_column_double(lua_State* L) {
      int i = luaL_checkinteger(L, 2) - 1;
      lua_pushnumber(L, sqlite3_column_double(get_sth(L, 1), i));
      return 1;
    }

    int impl_column_text(lua_State* L) {
      int i = luaL_checkinteger(L, 2) - 1;
      sqlite3_stmt* sth = get_sth(L, 1);
      if (const char* text = reinterpret_cast<const char*>(sqlite3_column_text(sth, i))) {
        lua_pushlstring(L, text, sqlite3_column_bytes(sth, i));
        return 1;
      } else {
        lua_pushnil(L);
        return 1;
      }
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
    function<impl_bind_null>::set_field(L, "bind_null");
    function<impl_clear_bindings>::set_field(L, "clear_bindings");
    function<impl_column_count>::set_field(L, "column_count");
    function<impl_column_name>::set_field(L, "column_name");
    function<impl_column_type>::set_field(L, "column_type");
    function<impl_column_int64>::set_field(L, "column_int64");
    function<impl_column_double>::set_field(L, "column_double");
    function<impl_column_text>::set_field(L, "column_text");
    luaL_newmetatable(L, "dromozoa.sqlite3.sth");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    return 1;
  }
}
