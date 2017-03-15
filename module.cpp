// Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <stdexcept>
#include <iostream>

#include "dromozoa/bind.hpp"

namespace dromozoa {
  enum enum_t {
    ENUM42 = 42,
    ENUM69 = 69,
  };

  namespace {
    void impl_throw(lua_State*) {
      throw std::runtime_error("runtime_error");
    }

    int impl_result_int(lua_State* L) {
      luaX_push(L, true);
      luaX_push(L, 42);
      luaX_push(L, "foo");
      return 3;
    }

    void impl_result_void(lua_State* L) {
      luaX_push(L, true);
      luaX_push(L, 42);
      luaX_push(L, "foo");
    }

    void impl_push_nil(lua_State* L) {
      luaX_push(L, luaX_nil);
    }

    enum {
      ENUM1 = 42,
      ENUM2 = -42,
    };

    void impl_push_enum(lua_State* L) {
      luaX_push<int>(L, ENUM1);
      luaX_push<int>(L, ENUM2);
    }

    void impl_push_string(lua_State* L) {
      char data[] = { 'f', 'o', 'o', 0 };
      luaX_push(L, "foo");
      luaX_push(L, data);
      luaX_push(L, static_cast<char*>(data));
      luaX_push(L, static_cast<const char*>(data));
      luaX_push(L, std::string("foo"));
    }

    void impl_push_success(lua_State* L) {
      luaX_push_success(L);
    }

    void impl_set_field(lua_State* L) {
      luaX_set_field(L, -1, 1, 17);
      luaX_set_field(L, -1, 2, 23);
      luaX_set_field(L, -1, 3, 37);
      luaX_set_field(L, -1, 4, 42);
      luaX_set_field(L, -1, 5, luaX_nil);
      luaX_push(L, "foo");
      luaX_set_field(L, -2, "s");
    }

    void impl_opt_range(lua_State* L) {
      size_t size = luaX_check_integer<size_t>(L, 1);
      size_t i = luaX_opt_range_i(L, 2, size);
      size_t j = luaX_opt_range_j(L, 3, size);
      luaX_push(L, i);
      luaX_push(L, j);
    }

    void impl_check_integer(lua_State* L) {
      luaX_check_integer<int16_t>(L, 1);
      luaX_check_integer<uint16_t>(L, 2);
      luaX_check_integer<size_t>(L, 3);
      luaX_check_integer<int>(L, 4, 0, 255);
    }

    void impl_opt_integer(lua_State* L) {
      luaX_opt_integer<int16_t>(L, 1, 0);
      luaX_opt_integer<uint16_t>(L, 2, 0);
      luaX_opt_integer<size_t>(L, 3, 0);
      luaX_opt_integer<int>(L, 4, 0, 0, 255);
    }

    void impl_check_enum(lua_State* L) {
      luaX_check_enum<enum_t>(L, 1);
    }

    void impl_opt_enum(lua_State* L) {
      enum_t value = luaX_opt_enum(L, 1, ENUM42);
      luaX_push<int>(L, value);
    }

    void impl_check_integer_field(lua_State* L) {
      luaX_push(L, luaX_check_integer_field<uint16_t>(L, 1, "foo"));
      luaX_push(L, luaX_check_integer_field<uint16_t>(L, 1, 42));
    }

    void impl_check_integer_field_range(lua_State* L) {
      luaX_push(L, luaX_check_integer_field<int>(L, 1, "nice", -20, 19));
    }

    void impl_opt_integer_field(lua_State* L) {
      luaX_push(L, luaX_opt_integer_field<uint16_t>(L, 1, "foo", 0));
      luaX_push(L, luaX_opt_integer_field<uint16_t>(L, 1, 42, 0));
    }

    void impl_opt_integer_field_range(lua_State* L) {
      luaX_push(L, luaX_opt_integer_field<int32_t>(L, 1, "tv_usec", 0, 0, 999999));
    }

    void impl_field_error1(lua_State* L) {
      luaX_field_error(L, luaX_nil, "what");
    }

    void impl_field_error2(lua_State* L) {
      luaX_field_error(L, impl_field_error2, "what");
    }

    void impl_field_error3(lua_State* L) {
      luaX_field_error(L, "foo\"bar\\baz", "what");
    }

    void impl_set_metafield(lua_State* L) {
      lua_newtable(L);
      luaX_set_metafield(L, -1, "a", "a");
      luaX_push(L, "b");
      luaX_set_metafield(L, -2, "b");
    }

    void impl_is_integer(lua_State* L) {
      luaX_push(L, luaX_is_integer(L, 1));
    }

    void impl_new(lua_State* L) {
      if (lua_isnoneornil(L, 2)) {
        luaX_new<int>(L);
      } else {
        luaX_new<int>(L, luaX_check_integer<int>(L, 2));
      }
      luaX_set_metatable(L, "dromozoa.bind.int");
    }

    void impl_set(lua_State* L) {
      *luaX_check_udata<int>(L, 1, "dromozoa.bind.int") = luaX_check_integer<int>(L, 2);
      luaX_push_success(L);
    }

    void impl_get(lua_State* L) {
      luaX_push(L, *luaX_check_udata<int>(L, 1, "dromozoa.bind.int"));
    }

    void impl_to(lua_State* L) {
      int* data = luaX_to_udata<int>(L, 1,
          luaL_optstring(L, 2, "dromozoa.bind.none"),
          luaL_optstring(L, 3, "dromozoa.bind.none"),
          luaL_optstring(L, 4, "dromozoa.bind.none"),
          luaL_optstring(L, 5, "dromozoa.bind.none"));
      if (data) {
        luaX_push(L, *data);
      } else {
        luaX_push(L, luaX_nil);
      }
    }

    int chain_gc_count = 0;

    void impl_chain_get(lua_State* L) {
      luaX_push(L, *luaX_check_udata<int>(L, 1, "dromozoa.bind.chain_b", "dromozoa.bind.chain_a"));
    }

    void impl_chain_gc(lua_State*) {
      ++chain_gc_count;
    }

    void impl_chain_gc_count(lua_State* L) {
      luaX_push(L, chain_gc_count);
    }

    void impl_chain_new_a(lua_State* L) {
      luaX_new<int>(L, luaX_opt_integer<int>(L, 1, 0));
      luaX_set_metatable(L, "dromozoa.bind.chain_a");
    }

    void impl_chain_new_b(lua_State* L) {
      luaX_new<int>(L, luaX_opt_integer<int>(L, 1, 0));
      luaX_set_metatable(L, "dromozoa.bind.chain_b");
    }

    void impl_unexpected(lua_State*) {
      DROMOZOA_UNEXPECTED("error");
    }

    typedef void (*api_callback_i_type)(int v, void* userdata);
    typedef void (*api_callback_s_type)(const std::string& v, void* userdata);
    typedef void (*api_destructor_type)(void* userdata);
    api_callback_i_type api_callback_i = 0;
    api_callback_s_type api_callback_s = 0;
    api_destructor_type api_destructor = 0;
    void* api_userdata = 0;

    void api_set_callback(api_callback_i_type callback_i, api_callback_s_type callback_s, api_destructor_type destructor, void* userdata) {
      api_callback_i = callback_i;
      api_callback_s = callback_s;
      api_destructor = destructor;
      api_userdata = userdata;
    }

    void api_run_callback_i(int v) {
      if (api_callback_i) {
        api_callback_i(v, api_userdata);
      }
    }

    void api_run_callback_s(const std::string& v) {
      if (api_callback_s) {
        api_callback_s(v, api_userdata);
      }
    }

    void api_run_destructor() {
      if (api_destructor) {
        api_destructor(api_userdata);
        api_set_callback(0, 0, 0, 0);
      }
    }

    void callback_i(int v, void* userdata) {
      luaX_reference<2>* ref = static_cast<luaX_reference<2>*>(userdata);
      lua_State* L = ref->state();
      int top = lua_gettop(L);
      ref->get_field();
      luaX_push(L, v);
      lua_pcall(L, 1, 1, 0);
      lua_settop(L, top);
    }

    void callback_s(const std::string& v, void* userdata) {
      luaX_reference<2>* ref = static_cast<luaX_reference<2>*>(userdata);
      lua_State* L = ref->state();
      int top = lua_gettop(L);
      ref->get_field(1);
      luaX_push(L, v);
      lua_pcall(L, 1, 1, 0);
      lua_settop(L, top);
    }

    void destructor(void* userdata) {
      delete static_cast<luaX_reference<2>*>(userdata);
    }

    void impl_set_callback(lua_State* L) {
      luaX_reference<2>* ref = new luaX_reference<2>(L, 1, 2);
      api_set_callback(&callback_i, &callback_s, &destructor, ref);
    }

    void impl_run_callback_i(lua_State* L) {
      api_run_callback_i(luaX_check_integer<int>(L, 1));
    }

    void impl_run_callback_s(lua_State* L) {
      size_t size = 0;
      const char* data = luaL_checklstring(L, 1, &size);
      api_run_callback_s(std::string(data, size));
    }

    void impl_run_destructor(lua_State*) {
      api_run_destructor();
    }

    luaX_reference<1> reference;

    void impl_ref(lua_State* L) {
      luaX_reference<1>(L, 1).swap(reference);
    }

    void impl_get_field_with_state(lua_State* L) {
      std::cout << reference.state() << " " << L << "\n";
      reference.get_field(L);
    }

    void impl_get_field_without_state(lua_State*) {
      reference.get_field();
    }

    void impl_unref(lua_State*) {
      luaX_reference<1>().swap(reference);
    }
  }

  void initialize(lua_State* L) {
    luaX_set_field(L, -1, "throw", impl_throw);
    luaX_set_field(L, -1, "result_int", impl_result_int);
    luaX_set_field(L, -1, "result_void", impl_result_void);
    luaX_set_field(L, -1, "push_nil", impl_push_nil);
    luaX_set_field(L, -1, "push_enum", impl_push_enum);
    luaX_set_field(L, -1, "push_string", impl_push_string);
    luaX_set_field(L, -1, "push_success", impl_push_success);
    luaX_set_field(L, -1, "set_field", impl_set_field);
    luaX_set_field(L, -1, "opt_range", impl_opt_range);
    luaX_set_field(L, -1, "check_integer", impl_check_integer);
    luaX_set_field(L, -1, "opt_integer", impl_opt_integer);
    luaX_set_field(L, -1, "check_enum", impl_check_enum);
    luaX_set_field(L, -1, "opt_enum", impl_opt_enum);
    luaX_set_field(L, -1, "check_integer_field", impl_check_integer_field);
    luaX_set_field(L, -1, "check_integer_field_range", impl_check_integer_field_range);
    luaX_set_field(L, -1, "opt_integer_field", impl_opt_integer_field);
    luaX_set_field(L, -1, "opt_integer_field_range", impl_opt_integer_field_range);
    luaX_set_field(L, -1, "field_error1", impl_field_error1);
    luaX_set_field(L, -1, "field_error2", impl_field_error2);
    luaX_set_field(L, -1, "field_error3", impl_field_error3);
    luaX_set_field(L, -1, "set_metafield", impl_set_metafield);
    luaX_set_field(L, -1, "is_integer", impl_is_integer);

    luaX_set_field<int>(L, -1, "ENUM42", ENUM42);
    luaX_set_field<int>(L, -1, "ENUM69", ENUM69);

    luaX_set_metafield(L, -1, "__call", impl_new);

    luaL_newmetatable(L, "dromozoa.bind.int");
    lua_newtable(L);
    luaX_set_field(L, -1, "set", impl_set);
    luaX_set_field(L, -1, "get", impl_get);
    luaX_set_field(L, -1, "to", impl_to);
    luaX_set_field(L, -2, "__index");
    lua_pop(L, 1);

    luaL_newmetatable(L, "dromozoa.bind.chain_a");
    lua_newtable(L);
    luaX_set_field(L, -1, "get", impl_chain_get);
    luaX_set_field(L, -2, "__index");
    luaX_set_field(L, -1, "__gc", impl_chain_gc);
    lua_pop(L, 1);

    luaL_newmetatable(L, "dromozoa.bind.chain_b");
    luaL_getmetatable(L, "dromozoa.bind.chain_a");
    luaX_get_field(L, -1, "__index");
    luaX_set_field(L, -3, "__index");
    luaX_get_field(L, -1, "__gc");
    luaX_set_field(L, -3, "__gc");
    lua_pop(L, 2);

    luaX_set_field(L, -1, "chain_new_a", impl_chain_new_a);
    luaX_set_field(L, -1, "chain_new_b", impl_chain_new_b);
    luaX_set_field(L, -1, "chain_gc_count", impl_chain_gc_count);

    luaX_set_field(L, -1, "unexpected", impl_unexpected);

    luaX_set_field(L, -1, "set_callback", impl_set_callback);
    luaX_set_field(L, -1, "run_callback_i", impl_run_callback_i);
    luaX_set_field(L, -1, "run_callback_s", impl_run_callback_s);
    luaX_set_field(L, -1, "run_destructor", impl_run_destructor);

    luaX_set_field(L, -1, "ref", impl_ref);
    luaX_set_field(L, -1, "get_field_with_state", impl_get_field_with_state);
    luaX_set_field(L, -1, "get_field_without_state", impl_get_field_without_state);
    luaX_set_field(L, -1, "unref", impl_unref);

    luaX_set_field(L, -1, "sizeof_lua_integer", sizeof(lua_Integer));
  }
}

extern "C" int luaopen_dromozoa_bind(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
