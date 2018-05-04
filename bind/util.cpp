// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <stdint.h>

#include "dromozoa/bind.hpp"

namespace dromozoa {
  namespace {
    void impl_is_integer(lua_State* L) {
      luaX_push(L, luaX_is_integer(L, 1));
    }

    void impl_check_int16(lua_State* L) {
      int16_t v = luaX_check_integer<int16_t>(L, 1);
      luaX_push(L, v);
    }

    void impl_check_uint16(lua_State* L) {
      uint16_t v = luaX_check_integer<uint16_t>(L, 1);
      luaX_push(L, v);
    }

    void impl_check_int_range(lua_State* L) {
      int v = luaX_check_integer<int>(L, 1, 0, 255);
      luaX_push(L, v);
    }

    void impl_opt_int16(lua_State* L) {
      int16_t v = luaX_opt_integer<int16_t>(L, 1, 42);
      luaX_push(L, v);
    }

    void impl_opt_uint16(lua_State* L) {
      uint16_t v = luaX_opt_integer<uint16_t>(L, 1, 42);
      luaX_push(L, v);
    }

    void impl_opt_int_range(lua_State* L) {
      int v = luaX_opt_integer<int>(L, 1, 42, 0, 255);
      luaX_push(L, v);
    }

    void impl_check_int_field(lua_State* L) {
      int v = luaX_check_integer_field<int>(L, 1, "nice", -20, 19);
      luaX_push(L, v);
    }

    void impl_opt_int32_field(lua_State* L) {
      int v = luaX_opt_integer_field<int32_t>(L, 1, "tv_nsec", 0, 0, 999999999);
      luaX_push(L, v);
    }

    void impl_opt_range(lua_State* L) {
      size_t size = luaX_check_integer<size_t>(L, 1);
      size_t i = luaX_opt_range_i(L, 2, size);
      size_t j = luaX_opt_range_j(L, 3, size);
      luaX_push(L, i, j);
    }
  }

  void initialize_util(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "is_integer", impl_is_integer);
      luaX_set_field(L, -1, "check_int16", impl_check_int16);
      luaX_set_field(L, -1, "check_uint16", impl_check_uint16);
      luaX_set_field(L, -1, "check_int_range", impl_check_int_range);
      luaX_set_field(L, -1, "opt_int16", impl_opt_int16);
      luaX_set_field(L, -1, "opt_uint16", impl_opt_uint16);
      luaX_set_field(L, -1, "opt_int_range", impl_opt_int_range);
      luaX_set_field(L, -1, "check_int_field", impl_check_int_field);
      luaX_set_field(L, -1, "opt_int32_field", impl_opt_int32_field);
      luaX_set_field(L, -1, "opt_range", impl_opt_range);

      luaX_set_field(L, -1, "sizeof_integer", sizeof(lua_Integer));
    }
    luaX_set_field(L, -2, "util");
  }
}
