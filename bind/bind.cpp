// Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "dromozoa/bind.hpp"

namespace dromozoa {
  namespace bind {
    namespace {
      int log_level = 0;
      bool raise_error = false;

      int impl_set_log_level(lua_State* L) {
        log_level = luaL_checkinteger(L, 1);
        lua_pushboolean(L, true);
        return 1;
      }

      int impl_get_log_level(lua_State* L) {
        lua_pushinteger(L, log_level);
        return 1;
      }

      int impl_set_raise_error(lua_State* L) {
        raise_error = lua_toboolean(L, 1);
        lua_pushboolean(L, true);
        return 1;
      }

      int impl_get_raise_error(lua_State* L) {
        lua_pushboolean(L, raise_error);
        return 1;
      }
    }

    namespace detail {
      int handle_result(lua_State* L, int result) {
        if (raise_error) {
          if (result <= 0 || !lua_toboolean(L, -result)) {
            if (result <= 1) {
              lua_pushliteral(L, "error raised");
            } else if (result > 2) {
              lua_pop(L, result - 2);
            }
            return lua_error(L);
          }
        }
        return result;
      }

      int handle_exception(lua_State* L, const std::exception& e) {
        return luaL_error(L, "exception caught: %s", e.what());
      }

      int handle_exception(lua_State* L) {
        lua_pushliteral(L, "exception caught");
        return lua_error(L);
      }
    }

    int get_log_level() {
      return log_level;
    }

    int push_success(lua_State* L) {
      if (lua_toboolean(L, 1)) {
        lua_pushvalue(L, 1);
        return 1;
      } else {
        lua_pushboolean(L, true);
        return 1;
      }
    }

    void set_field(lua_State* L, const char* key, lua_Integer value) {
      lua_pushinteger(L, value);
      lua_setfield(L, -2, key);
    }

    void initialize(lua_State* L) {
      function<impl_set_log_level>::set_field(L, "set_log_level");
      function<impl_get_log_level>::set_field(L, "get_log_level");
      function<impl_set_raise_error>::set_field(L, "set_raise_error");
      function<impl_get_raise_error>::set_field(L, "get_raise_error");
    }
  }
}
