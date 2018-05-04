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

#include <algorithm>
#include <sstream>
#include <string>
#include <stdexcept>

#include "dromozoa/bind.hpp"

namespace dromozoa {
  namespace {
    enum { _42 = 42 };

    int impl_result_int(lua_State* L) {
      luaX_push(L, luaX_nil, 0.25, 42, true, "foo");
      return 5;
    }

    void impl_result_void(lua_State* L) {
      luaX_push(L, luaX_nil, 0.25, 42, true, "foo");
    }

    void impl_push_none(lua_State*) {}

    void impl_push_nil(lua_State* L) {
      luaX_push(L, luaX_nil);
    }

    void impl_push_enum(lua_State* L) {
      luaX_push<int>(L, _42);
    }

    void impl_push_string(lua_State* L) {
      std::string s = "あいうえお";
      char b[256] = { 0 };
      std::copy(s.begin(), s.end(), b);
      luaX_push(L, "あいうえお", b, static_cast<char*>(b), static_cast<const char*>(b), s);
    }

    void impl_push_success(lua_State* L) {
      luaX_push_success(L);
    }

    void impl_unexpected(lua_State*) {
      DROMOZOA_UNEXPECTED("error");
    }

    void impl_throw(lua_State*) {
      throw std::runtime_error("runtime_error");
    }

    void impl_field_error1(lua_State* L) {
      luaX_field_error(L, luaX_nil, "not an integer");
    }

    void impl_field_error2(lua_State* L) {
      luaX_field_error(L, impl_field_error2, "not an integer");
    }

    void impl_field_error3(lua_State* L) {
      char b[256] = { 0 };
      {
        std::ostringstream out;
        for (int i = 1; i <= 127; ++i) {
          out << static_cast<char>(i);
        }
        out << "あいうえお";
        std::string s = out.str();
        std::copy(s.begin(), s.end(), b);
      }
      luaX_field_error(L, b, "not an integer");
    }

    void impl_set_field(lua_State* L) {
      int n = luaX_check_integer<int>(L, 1);
      lua_newtable(L);
      for (int i = 1; i <= n; ++i) {
        luaX_set_field(L, -1, i, i);
      }
      luaX_set_field(L, -1, "foo", "bar");
      luaX_push(L, "qux");
      luaX_set_field(L, -2, "baz");
    }

    void impl_set_metafield(lua_State* L) {
      lua_newtable(L);
      luaX_set_metafield(L, -1, "dromozoa.bind.a", 42);
      luaX_push(L, "あいうえお");
      luaX_set_metafield(L, -2, "dromozoa.bind.b");
    }
  }

  void initialize_core(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "result_int", impl_result_int);
      luaX_set_field(L, -1, "result_void", impl_result_void);
      luaX_set_field(L, -1, "push_none", impl_push_none);
      luaX_set_field(L, -1, "push_nil", impl_push_nil);
      luaX_set_field(L, -1, "push_enum", impl_push_enum);
      luaX_set_field(L, -1, "push_string", impl_push_string);
      luaX_set_field(L, -1, "push_success", impl_push_success);
      luaX_set_field(L, -1, "unexpected", impl_unexpected);
      luaX_set_field(L, -1, "throw", impl_throw);
      luaX_set_field(L, -1, "field_error1", impl_field_error1);
      luaX_set_field(L, -1, "field_error2", impl_field_error2);
      luaX_set_field(L, -1, "field_error3", impl_field_error3);
      luaX_set_field(L, -1, "set_field", impl_set_field);
      luaX_set_field(L, -1, "set_metafield", impl_set_metafield);
    }
    luaX_set_field(L, -2, "core");
  }
}
