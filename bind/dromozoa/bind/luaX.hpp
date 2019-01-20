// Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_BIND_LUAX_HPP
#define DROMOZOA_BIND_LUAX_HPP

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <exception>
#include <limits>
#include <new>
#include <sstream>
#include <string>
#include <utility>

namespace dromozoa {
  namespace bind {
    struct luaX_nil_impl {};
    typedef int luaX_nil_impl::*luaX_nil_t;
    static const luaX_nil_t luaX_nil = 0;

    class luaX_string_reference {
    public:
      typedef const char* (luaX_string_reference::*unspecified_bool_type)() const;

      luaX_string_reference() : data_(), size_() {}

      luaX_string_reference(const char* data, size_t size) : data_(data), size_(size) {}

      luaX_string_reference(const signed char* data, size_t size) : data_(reinterpret_cast<const char*>(data)), size_(size) {}

      luaX_string_reference(const unsigned char* data, size_t size) : data_(reinterpret_cast<const char*>(data)), size_(size) {}

      const char* data() const {
        return data_;
      }

      size_t size() const {
        return size_;
      }

      operator unspecified_bool_type() const {
        return data_ ? &luaX_string_reference::data : 0;
      }

      bool operator!() const {
        return !data_;
      }

    private:
      const char* data_;
      size_t size_;
    };

    struct luaX_type_nil {};
    struct luaX_type_number {};
    struct luaX_type_numflt : luaX_type_number {};
    struct luaX_type_numint : luaX_type_number {};
    struct luaX_type_boolean {};
    struct luaX_type_string {};
    struct luaX_type_function {};

    template <class T>
    struct luaX_type {};

    template <class T, class T_type>
    struct luaX_type_traits_impl {};

    template <class T>
    struct luaX_type_traits : luaX_type_traits_impl<typename luaX_type<T>::decay, typename luaX_type<T>::type> {};

    template <class T, bool T_is_signed>
    struct luaX_integer_traits_impl {};

    template <class T, class T_type = typename luaX_type<T>::type>
    struct luaX_integer_traits {};

    template <class T>
    struct luaX_integer_traits<T, luaX_type_numint> : luaX_integer_traits_impl<T, std::numeric_limits<T>::is_signed> {};

    template <class T>
    inline void luaX_push(lua_State* L, const T& value) {
      luaX_type_traits<T>::push(L, value);
    }

    template <class T1, class T2>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2) {
      luaX_push(L, v1);
      luaX_push(L, v2);
    }

    template <class T1, class T2, class T3>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2, const T3& v3) {
      luaX_push(L, v1);
      luaX_push(L, v2);
      luaX_push(L, v3);
    }

    template <class T1, class T2, class T3, class T4>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4) {
      luaX_push(L, v1);
      luaX_push(L, v2);
      luaX_push(L, v3);
      luaX_push(L, v4);
    }

    template <class T1, class T2, class T3, class T4, class T5>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5) {
      luaX_push(L, v1);
      luaX_push(L, v2);
      luaX_push(L, v3);
      luaX_push(L, v4);
      luaX_push(L, v5);
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6) {
      luaX_push(L, v1);
      luaX_push(L, v2);
      luaX_push(L, v3);
      luaX_push(L, v4);
      luaX_push(L, v5);
      luaX_push(L, v6);
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6, const T7& v7) {
      luaX_push(L, v1);
      luaX_push(L, v2);
      luaX_push(L, v3);
      luaX_push(L, v4);
      luaX_push(L, v5);
      luaX_push(L, v6);
      luaX_push(L, v7);
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline void luaX_push(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6, const T7& v7, const T8& v8) {
      luaX_push(L, v1);
      luaX_push(L, v2);
      luaX_push(L, v3);
      luaX_push(L, v4);
      luaX_push(L, v5);
      luaX_push(L, v6);
      luaX_push(L, v7);
      luaX_push(L, v8);
    }

    inline void luaX_push_success(lua_State* L) {
      if (lua_toboolean(L, 1)) {
        lua_pushvalue(L, 1);
      } else {
        lua_pushboolean(L, true);
      }
    }

    template <class T = void>
    class luaX_failure : public std::exception {};

    template <>
    class luaX_failure<int> : public std::exception {
    public:
      virtual int code() const = 0;
    };

    template <class T = void>
    class luaX_failure_impl : public luaX_failure<T> {
    public:
      explicit luaX_failure_impl(const char* what) : what_(what) {}

      explicit luaX_failure_impl(const std::string& what) : what_(what) {}

      virtual ~luaX_failure_impl() throw() {}

      virtual const char* what() const throw() {
        return what_.c_str();
      }

    private:
      std::string what_;
    };

    template <>
    class luaX_failure_impl<int> : public luaX_failure<int> {
    public:
      luaX_failure_impl(const char* what, int code) : what_(what), code_(code) {}

      luaX_failure_impl(const std::string& what, int code) : what_(what), code_(code) {}

      virtual ~luaX_failure_impl() throw() {}

      virtual const char* what() const throw() {
        return what_.c_str();
      }

      virtual int code() const {
        return code_;
      }

    private:
      std::string what_;
      int code_;
    };

    inline void luaX_throw_failure(const char* what) {
      throw luaX_failure_impl<>(what);
    }

    inline void luaX_throw_failure(const std::string& what) {
      throw luaX_failure_impl<>(what);
    }

    inline void luaX_throw_failure(const char* what, int code) {
      throw luaX_failure_impl<int>(what, code);
    }

    inline void luaX_throw_failure(const std::string& what, int code) {
      throw luaX_failure_impl<int>(what, code);
    }

    template <class T>
    inline T* luaX_new(lua_State* L) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T();
      return data;
    }

    template <class T, class T1>
    inline T* luaX_new(lua_State* L, const T1& v1) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1);
      return data;
    }

    template <class T, class T1, class T2>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2);
      return data;
    }

    template <class T, class T1, class T2, class T3>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2, const T3& v3) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2, v3);
      return data;
    }

    template <class T, class T1, class T2, class T3, class T4>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2, v3, v4);
      return data;
    }

    template <class T, class T1, class T2, class T3, class T4, class T5>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2, v3, v4, v5);
      return data;
    }

    template <class T, class T1, class T2, class T3, class T4, class T5, class T6>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2, v3, v4, v5, v6);
      return data;
    }

    template <class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6, const T7& v7) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2, v3, v4, v5, v6, v7);
      return data;
    }

    template <class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline T* luaX_new(lua_State* L, const T1& v1, const T2& v2, const T3& v3, const T4& v4, const T5& v5, const T6& v6, const T7& v7, const T8& v8) {
      T* data = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
      new(data) T(v1, v2, v3, v4, v5, v6, v7, v8);
      return data;
    }

    inline bool luaX_is_true(lua_State* L, int index) {
      return lua_isboolean(L, index) && lua_toboolean(L, index);
    }

    inline bool luaX_is_false(lua_State* L, int index) {
      return lua_isboolean(L, index) && !lua_toboolean(L, index);
    }

    inline bool luaX_is_integer(lua_State* L, int index) {
#if LUA_VERSION_NUM+0 >= 503
      if (lua_isinteger(L, index)) {
        return true;
      }
#endif
      if (lua_type(L, index) == LUA_TNUMBER) {
        double u = lua_tonumber(L, index);
        if (isfinite(u)) {
          lua_pushinteger(L, u);
          double v = lua_tonumber(L, -1);
          lua_pop(L, 1);
          return u == v;
        }
      }
      return false;
    }

    template <class T>
    inline T luaX_check_integer(lua_State* L, int arg) {
      T target = 0;
      if (luaX_integer_traits<T>::convert(luaL_checkinteger(L, arg), target)) {
        return target;
      }
      return luaL_argerror(L, arg, "out of bounds");
    }

    template <class T>
    inline T luaX_check_integer(lua_State* L, int arg, T min, T max) {
      T target = 0;
      if (luaX_integer_traits<T>::convert(luaL_checkinteger(L, arg), target, min, max)) {
        return target;
      }
      return luaL_argerror(L, arg, "out of bounds");
    }

    template <class T>
    inline T luaX_opt_integer(lua_State* L, int arg, T d) {
      T target = 0;
      if (luaX_integer_traits<T>::convert(luaL_optinteger(L, arg, d), target)) {
        return target;
      }
      return luaL_argerror(L, arg, "out of bounds");
    }

    template <class T>
    inline T luaX_opt_integer(lua_State* L, int arg, T d, T min, T max) {
      T target = 0;
      if (luaX_integer_traits<T>::convert(luaL_optinteger(L, arg, d), target, min, max)) {
        return target;
      }
      return luaL_argerror(L, arg, "out of bounds");
    }

    template <class T>
    inline T luaX_check_enum(lua_State* L, int arg) {
      return static_cast<T>(luaL_checkinteger(L, arg));
    }

    template <class T>
    inline T luaX_opt_enum(lua_State* L, int arg, T d) {
      return static_cast<T>(luaL_optinteger(L, arg, d));
    }

    inline luaX_string_reference luaX_to_string(lua_State* L, int index) {
      size_t size = 0;
      const char* data = lua_tolstring(L, index, &size);
      return luaX_string_reference(data, size);
    }

    inline luaX_string_reference luaX_check_string(lua_State* L, int arg) {
      size_t size = 0;
      const char* data = luaL_checklstring(L, arg, &size);
      return luaX_string_reference(data, size);
    }

    inline bool luaX_to_udata_impl(lua_State* L, const char* name) {
      luaL_getmetatable(L, name);
      bool result = lua_rawequal(L, -1, -2);
      lua_pop(L, 1);
      return result;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!luaX_to_udata_impl(L, name)) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2, const char* name3) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2) || luaX_to_udata_impl(L, name3))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2, const char* name3, const char* name4) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2) || luaX_to_udata_impl(L, name3) || luaX_to_udata_impl(L, name4))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2) || luaX_to_udata_impl(L, name3) || luaX_to_udata_impl(L, name4) || luaX_to_udata_impl(L, name5))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5, const char* name6) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2) || luaX_to_udata_impl(L, name3) || luaX_to_udata_impl(L, name4) || luaX_to_udata_impl(L, name5) || luaX_to_udata_impl(L, name6))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5, const char* name6, const char* name7) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2) || luaX_to_udata_impl(L, name3) || luaX_to_udata_impl(L, name4) || luaX_to_udata_impl(L, name5) || luaX_to_udata_impl(L, name6) || luaX_to_udata_impl(L, name7))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_to_udata(lua_State* L, int index, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5, const char* name6, const char* name7, const char* name8) {
      if (T* data = static_cast<T*>(lua_touserdata(L, index))) {
        if (lua_getmetatable(L, index)) {
          if (!(luaX_to_udata_impl(L, name1) || luaX_to_udata_impl(L, name2) || luaX_to_udata_impl(L, name3) || luaX_to_udata_impl(L, name4) || luaX_to_udata_impl(L, name5) || luaX_to_udata_impl(L, name6) || luaX_to_udata_impl(L, name7) || luaX_to_udata_impl(L, name8))) {
            data = 0;
          }
          lua_pop(L, 1);
          return data;
        }
      }
      return 0;
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name) {
      return static_cast<T*>(luaL_checkudata(L, arg, name));
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2) {
      if (T* data = luaX_to_udata<T>(L, arg, name1)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name2);
      }
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2, const char* name3) {
      if (T* data = luaX_to_udata<T>(L, arg, name1, name2)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name3);
      }
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2, const char* name3, const char* name4) {
      if (T* data = luaX_to_udata<T>(L, arg, name1, name2, name3)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name4);
      }
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5) {
      if (T* data = luaX_to_udata<T>(L, arg, name1, name2, name3, name4)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name5);
      }
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5, const char* name6) {
      if (T* data = luaX_to_udata<T>(L, arg, name1, name2, name3, name4, name5)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name6);
      }
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5, const char* name6, const char* name7) {
      if (T* data = luaX_to_udata<T>(L, arg, name1, name2, name3, name4, name5, name6)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name7);
      }
    }

    template <class T>
    inline T* luaX_check_udata(lua_State* L, int arg, const char* name1, const char* name2, const char* name3, const char* name4, const char* name5, const char* name6, const char* name7, const char* name8) {
      if (T* data = luaX_to_udata<T>(L, arg, name1, name2, name3, name4, name5, name6, name7)) {
        return data;
      } else {
        return luaX_check_udata<T>(L, arg, name8);
      }
    }

    inline int luaX_abs_index(lua_State* L, int index) {
#if LUA_VERSION_NUM+0 >= 502
      return lua_absindex(L, index);
#else
      if (index < 0) {
        int top = lua_gettop(L);
        if (top >= -index) {
          return top + index + 1;
        }
      }
      return index;
#endif
    }

    template <class T, class T_key>
    inline int luaX_field_error(lua_State* L, const T_key& key, const T& what) {
      luaL_where(L, 1);
      luaX_push(L, "field ");
      luaX_type_traits<T_key>::quote(L, key);
      luaX_push(L, " ");
      luaX_push(L, what);
      lua_concat(L, 5);
      return lua_error(L);
    }

    template <class T_key>
    inline void luaX_set_field(lua_State* L, int index, const T_key& key) {
      index = luaX_abs_index(L, index);
      luaX_push(L, key);
      lua_pushvalue(L, -2);
      lua_settable(L, index);
      lua_pop(L, 1);
    }

    template <class T, class T_key>
    inline void luaX_set_field(lua_State* L, int index, const T_key& key, const T& value) {
      index = luaX_abs_index(L, index);
      luaX_push(L, key, value);
      lua_settable(L, index);
    }

    template <class T_key>
    inline int luaX_get_field(lua_State* L, int index, const T_key& key) {
      index = luaX_abs_index(L, index);
      luaX_push(L, key);
#if LUA_VERSION_NUM+0 >= 503
      return lua_gettable(L, index);
#else
      lua_gettable(L, index);
      return lua_type(L, -1);
#endif
    }

    template <class T_key>
    inline intmax_t luaX_check_integer_field_impl(lua_State* L, int index, const T_key& key) {
      index = luaX_abs_index(L, index);
      luaX_push(L, key);
      lua_gettable(L, index);
      if (lua_isnumber(L, -1)) {
        intmax_t value = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return value;
      } else {
        lua_pop(L, 1);
        return luaX_field_error(L, key, "not an integer");
      }
    }

    template <class T, class T_key>
    inline T luaX_check_integer_field(lua_State* L, int index, const T_key& key) {
      intmax_t source = luaX_check_integer_field_impl(L, index, key);
      T target = 0;
      if (luaX_integer_traits<T>::convert(source, target)) {
        return target;
      }
      return luaX_field_error(L, key, "out of bounds");
    }

    template <class T, class T_key>
    inline T luaX_check_integer_field(lua_State* L, int index, const T_key& key, T min, T max) {
      intmax_t source = luaX_check_integer_field_impl(L, index, key);
      T target = 0;
      if (luaX_integer_traits<T>::convert(source, target, min, max)) {
        return target;
      }
      return luaX_field_error(L, key, "out of bounds");
    }

    template <class T, class T_key>
    inline intmax_t luaX_opt_integer_field_impl(lua_State* L, int index, const T_key& key, T d) {
      index = luaX_abs_index(L, index);
      luaX_push(L, key);
#if LUA_VERSION_NUM+0 >= 503
      bool is_nil = lua_gettable(L, index) == LUA_TNIL;
#else
      lua_gettable(L, index);
      bool is_nil = lua_isnil(L, -1);
#endif
      if (lua_isnumber(L, -1)) {
        intmax_t value = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return value;
      } else {
        lua_pop(L, 1);
        if (is_nil) {
          return d;
        } else {
          return luaX_field_error(L, key, "not an integer");
        }
      }
    }

    template <class T, class T_key>
    inline T luaX_opt_integer_field(lua_State* L, int index, const T_key& key, T d) {
      intmax_t source = luaX_opt_integer_field_impl(L, index, key, d);
      T target = 0;
      if (luaX_integer_traits<T>::convert(source, target)) {
        return target;
      }
      return luaX_field_error(L, key, "out of bounds");
    }

    template <class T, class T_key>
    inline T luaX_opt_integer_field(lua_State* L, int index, const T_key& key, T d, T min, T max) {
      intmax_t source = luaX_opt_integer_field_impl(L, index, key, d);
      T target = 0;
      if (luaX_integer_traits<T>::convert(source, target, min, max)) {
        return target;
      }
      return luaX_field_error(L, key, "out of bounds");
    }

    inline void luaX_set_metatable(lua_State* L, const char* name) {
#if LUA_VERSION_NUM+0 >= 502
      luaL_setmetatable(L, name);
#else
      luaL_getmetatable(L, name);
      lua_setmetatable(L, -2);
#endif
    }

    template <class T_key>
    inline void luaX_set_metafield(lua_State* L, int index, const T_key& key) {
      index = luaX_abs_index(L, index);
      if (!lua_getmetatable(L, index)) {
        lua_newtable(L);
      }
      luaX_push(L, key);
      lua_pushvalue(L, -3);
      lua_settable(L, -3);
      lua_setmetatable(L, index);
      lua_pop(L, 1);
    }

    template <class T, class T_key>
    inline void luaX_set_metafield(lua_State* L, int index, const T_key& key, const T& value) {
      index = luaX_abs_index(L, index);
      if (!lua_getmetatable(L, index)) {
        lua_newtable(L);
      }
      luaX_set_field(L, -1, key, value);
      lua_setmetatable(L, index);
    }

    inline size_t luaX_opt_range_i(lua_State* L, int arg, size_t size) {
      lua_Integer i = luaL_optinteger(L, arg, 0);
      if (i < 0) {
        i += size;
        if (i < 0) {
          i = 0;
        }
      } else if (i > 0) {
        --i;
      }
      return i;
    }

    inline size_t luaX_opt_range_j(lua_State* L, int arg, size_t size) {
      lua_Integer j = luaL_optinteger(L, arg, size);
      if (j < 0) {
        j += size + 1;
      } else if (j > static_cast<lua_Integer>(size)) {
        j = size;
      }
      return j;
    }

#define DROMOZOA_BIND_LUAX_TYPE(PP_lua_type, PP_cxx_type) \
    template <> \
    struct luaX_type<PP_cxx_type> { \
      typedef PP_cxx_type decay; \
      typedef PP_lua_type type; \
    };

    DROMOZOA_BIND_LUAX_TYPE(luaX_type_nil, luaX_nil_t)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numflt, float)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numflt, double)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, long double)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, char)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, signed char)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, unsigned char)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, short)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, unsigned short)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, int)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, unsigned int)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, long)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, unsigned long)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, long long)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_numint, unsigned long long)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_boolean, bool)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, char*)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, signed char*)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, unsigned char*)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, const char*)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, const signed char*)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, const unsigned char*)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, std::string)
    DROMOZOA_BIND_LUAX_TYPE(luaX_type_string, luaX_string_reference)

#undef DROMOZOA_BIND_LUAX_TYPE

    template <size_t T>
    struct luaX_type<char[T]> {
      typedef const char* decay;
      typedef luaX_type_string type;
    };

    template <size_t T>
    struct luaX_type<signed char[T]> {
      typedef const signed char* decay;
      typedef luaX_type_string type;
    };

    template <size_t T>
    struct luaX_type<unsigned char[T]> {
      typedef const unsigned char* decay;
      typedef luaX_type_string type;
    };

    template <class T>
    struct luaX_type<T(lua_State* L)> {
      typedef T (*decay)(lua_State* L);
      typedef luaX_type_function type;
    };

    template <class T>
    struct luaX_type_traits_impl<T, luaX_type_nil> {
      static void push(lua_State* L, const T&) {
        lua_pushnil(L);
      }

      static void quote(lua_State* L, const T&) {
        luaX_push(L, "nil");
      }
    };

    template <class T>
    struct luaX_type_traits_impl<T, luaX_type_numflt> {
      static void push(lua_State* L, const T& value) {
        lua_pushnumber(L, value);
      }

      static void quote(lua_State* L, const T& value) {
        luaX_push(L, value);
      }
    };

    template <class T>
    struct luaX_type_traits_impl<T, luaX_type_numint> {
      static void push(lua_State* L, const T& value) {
        lua_pushinteger(L, value);
      }

      static void quote(lua_State* L, const T& value) {
        luaX_push(L, value);
      }
    };

    template <class T>
    struct luaX_type_traits_impl<T, luaX_type_boolean> {
      static void push(lua_State* L, const T& value) {
        lua_pushboolean(L, value);
      }

      static void quote(lua_State* L, const T& value) {
        if (value) {
          luaX_push(L, "true");
        } else {
          luaX_push(L, "false");
        }
      }
    };

    template <class T>
    struct luaX_type_traits_impl<T, luaX_type_string> {
      template <class T_char>
      static void push(lua_State* L, const T_char* value) {
        lua_pushstring(L, reinterpret_cast<const char*>(value));
      }

      static void push(lua_State* L, const std::string& value) {
        lua_pushlstring(L, value.data(), value.size());
      }

      static void push(lua_State* L, const luaX_string_reference& value) {
        lua_pushlstring(L, value.data(), value.size());
      }

      static void quote_impl(lua_State* L, const char* data, size_t size) {
        std::ostringstream out;
        out << "\"";
        for (size_t i = 0; i < size; ++i) {
          char c = data[i];
          switch (c) {
            case '\a': out << "\\a"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            case '\v': out << "\\v"; break;
            case '\\': out << "\\\\"; break;
            case '\"': out << "\\\""; break;
            case '\'': out << "\\'"; break;
            default:
              if ((0x00 <= c && c < 0x20) || c == 0x7F) {
                out << "\\" << static_cast<int>(c);
              } else {
                out << c;
              }
          }
        }
        out << "\"";
        luaX_push(L, out.str());
      }

      template <class T_char>
      static void quote(lua_State* L, const T_char* value) {
        const char* data = reinterpret_cast<const char*>(value);
        quote_impl(L, data, strlen(data));
      }

      static void quote(lua_State* L, const std::string& value) {
        quote_impl(L, value.data(), value.size());
      }

      static void quote(lua_State* L, const luaX_string_reference& value) {
        quote_impl(L, value.data(), value.size());
      }
    };

    template <class T>
    struct luaX_type_traits_impl<T, luaX_type_function> {
      static int call(lua_State* L, int, lua_CFunction function) {
        return function(L);
      }

      static int call(lua_State* L, int top, void (*function)(lua_State*)) {
        function(L);
        return lua_gettop(L) - top;
      }

      static int closure(lua_State* L) {
        int top = lua_gettop(L);
        try {
          return call(L, top, reinterpret_cast<T>(lua_touserdata(L, lua_upvalueindex(1))));
        } catch (const luaX_failure<>& e) {
          lua_settop(L, top);
          luaX_push(L, luaX_nil);
          luaX_push(L, e.what());
          return 2;
        } catch (const luaX_failure<int>& e) {
          lua_settop(L, top);
          luaX_push(L, luaX_nil);
          luaX_push(L, e.what());
          luaX_push(L, e.code());
          return 3;
        } catch (const std::exception& e) {
          lua_settop(L, top);
          luaL_where(L, 1);
          luaX_push(L, "exception caught: ");
          luaX_push(L, e.what());
          lua_concat(L, 3);
        }
        return lua_error(L);
      }

      static void push(lua_State* L, const T& value) {
        lua_pushlightuserdata(L, reinterpret_cast<void*>(value));
        lua_pushcclosure(L, closure, 1);
      }

      static void quote(lua_State* L, const T& value) {
        lua_pushfstring(L, "userdata: %p", value);
      }
    };

    template <class T>
    struct luaX_integer_traits_impl<T, true> {
      static T convert(intmax_t source, T& target) {
        static const intmax_t min = std::numeric_limits<T>::min();
        static const intmax_t max = std::numeric_limits<T>::max();
        return convert(source, target, min, max);
      }

      static T convert(intmax_t source, T& target, intmax_t min, intmax_t max) {
        if (min <= source && source <= max) {
          target = source;
          return true;
        }
        return false;
      }
    };

    template <class T>
    struct luaX_integer_traits_impl<T, false> {
      static T convert(intmax_t source, T& target) {
        static const uintmax_t min = std::numeric_limits<T>::min();
        static const uintmax_t max = std::numeric_limits<T>::max();
        return convert(source, target, min, max);
      }

      static T convert(intmax_t source, T& target, uintmax_t min, uintmax_t max) {
        if (0 <= source) {
          uintmax_t value = source;
          if (min <= value && value <= max) {
            target = value;
            return true;
          }
        }
        return false;
      }
    };

    class luaX_binder {
    public:
      virtual ~luaX_binder() {};
      virtual lua_State* state() const = 0;
    protected:
      luaX_binder() {}
    private:
      luaX_binder(const luaX_binder&);
      luaX_binder& operator=(const luaX_binder&);
    };

    class luaX_binder_impl : public luaX_binder {
    public:
      virtual ~luaX_binder_impl() {
        if (lua_State* L = state()) {
          luaL_unref(L, LUA_REGISTRYINDEX, reference_);
        }
      }

      virtual lua_State* state() const {
        return state_;
      }

    protected:
      explicit luaX_binder_impl(lua_State* L) : state_(), reference_(LUA_NOREF) {
        if (L) {
          state_ = lua_newthread(L);
          reference_ = luaL_ref(L, LUA_REGISTRYINDEX);
        }
      }

      void swap(luaX_binder_impl& that) {
        std::swap(state_, that.state_);
        std::swap(reference_, that.reference_);
      }

    private:
      lua_State* state_;
      int reference_;
    };

    template <size_t T>
    class luaX_reference_impl : public luaX_binder_impl {
    public:
      virtual ~luaX_reference_impl() {
        if (lua_State* L = state()) {
          for (size_t i = 0; i < T; ++i) {
            luaL_unref(L, LUA_REGISTRYINDEX, references_[i]);
          }
        }
      }

      int get_field(lua_State* L, size_t i = 0) const {
        if (i < T) {
          return luaX_get_field(L, LUA_REGISTRYINDEX, references_[i]);
        } else {
          luaX_push(L, luaX_nil);
          return LUA_TNIL;
        }
      }

      void swap(luaX_reference_impl& that) {
        luaX_binder_impl::swap(that);
        for (size_t i = 0; i < T; ++i) {
          std::swap(references_[i], that.references_[i]);
        }
      }

    protected:
      explicit luaX_reference_impl(lua_State* L) : luaX_binder_impl(L) {
        for (size_t i = 0; i < T; ++i) {
          references_[i] = LUA_NOREF;
        }
      }

      void ref(lua_State* L, size_t i, int index) {
        lua_pushvalue(L, index);
        references_[i] = luaL_ref(L, LUA_REGISTRYINDEX);
      }

    private:
      int references_[T];
    };

    template <size_t T = 1>
    class luaX_reference {};

    template <>
    class luaX_reference<1> : public luaX_reference_impl<1> {
    public:
      luaX_reference() : luaX_reference_impl<1>(0) {}

      luaX_reference(lua_State* L, int index0) : luaX_reference_impl<1>(L) {
        ref(L, 0, index0);
      }
    };

    template <>
    class luaX_reference<2> : public luaX_reference_impl<2> {
    public:
      luaX_reference() : luaX_reference_impl<2>(0) {}

      luaX_reference(lua_State* L, int index0, int index1) : luaX_reference_impl<2>(L) {
        ref(L, 0, index0);
        ref(L, 1, index1);
      }
    };

    template <>
    class luaX_reference<3> : public luaX_reference_impl<3> {
    public:
      luaX_reference() : luaX_reference_impl<3>(0) {}

      luaX_reference(lua_State* L, int index0, int index1, int index2) : luaX_reference_impl<3>(L) {
        ref(L, 0, index0);
        ref(L, 1, index1);
        ref(L, 2, index2);
      }
    };

    template <>
    class luaX_reference<4> : public luaX_reference_impl<4> {
    public:
      luaX_reference() : luaX_reference_impl<4>(0) {}

      luaX_reference(lua_State* L, int index0, int index1, int index2, int index3) : luaX_reference_impl<4>(L) {
        ref(L, 0, index0);
        ref(L, 1, index1);
        ref(L, 2, index2);
        ref(L, 3, index3);
      }
    };

    class luaX_top_saver {
    public:
      explicit luaX_top_saver(lua_State* L) : state_(L), top_(lua_gettop(L)) {}

      ~luaX_top_saver() {
        lua_settop(state_, top_);
      }

      int get() const {
        return top_;
      }

    private:
      lua_State* state_;
      int top_;
      luaX_top_saver(const luaX_top_saver&);
      luaX_top_saver& operator=(const luaX_top_saver&);
    };
  }

  using bind::luaX_abs_index;
  using bind::luaX_binder;
  using bind::luaX_check_enum;
  using bind::luaX_check_integer;
  using bind::luaX_check_integer_field;
  using bind::luaX_check_string;
  using bind::luaX_check_udata;
  using bind::luaX_failure;
  using bind::luaX_field_error;
  using bind::luaX_get_field;
  using bind::luaX_is_false;
  using bind::luaX_is_integer;
  using bind::luaX_is_true;
  using bind::luaX_new;
  using bind::luaX_nil;
  using bind::luaX_nil_t;
  using bind::luaX_opt_enum;
  using bind::luaX_opt_integer;
  using bind::luaX_opt_integer_field;
  using bind::luaX_opt_range_i;
  using bind::luaX_opt_range_j;
  using bind::luaX_push;
  using bind::luaX_push_success;
  using bind::luaX_reference;
  using bind::luaX_set_field;
  using bind::luaX_set_metafield;
  using bind::luaX_set_metatable;
  using bind::luaX_string_reference;
  using bind::luaX_throw_failure;
  using bind::luaX_to_string;
  using bind::luaX_to_udata;
  using bind::luaX_top_saver;
}

#endif
