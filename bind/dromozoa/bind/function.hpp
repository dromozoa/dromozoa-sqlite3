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

#ifndef DROMOZOA_BIND_FUNCTION_HPP
#define DROMOZOA_BIND_FUNCTION_HPP

extern "C" {
#include <lua.h>
}

#include <exception>

namespace dromozoa {
  namespace bind {
    namespace detail {
      int handle_result(lua_State* L, int result);
      int handle_exception(lua_State* L, const std::exception& e);
      int handle_exception(lua_State* L);
    }

    template <lua_CFunction T>
    struct function {
      static int value(lua_State* L) {
        int result;
        try {
          result = T(L);
        } catch (const std::exception& e) {
          return detail::handle_exception(L, e);
        } catch (...) {
          return detail::handle_exception(L);
        }
        return detail::handle_result(L, result);
      }

      static void set_field(lua_State* L, const char* key) {
        lua_pushcfunction(L, value);
        lua_setfield(L, -2, key);
      }
    };
  }
}

#endif
