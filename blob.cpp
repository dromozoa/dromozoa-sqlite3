// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <limits>
#include <vector>

#include "common.hpp"

namespace dromozoa {
  blob_handle* check_blob_handle(lua_State* L, int arg) {
    return luaX_check_udata<blob_handle>(L, arg, "dromozoa.sqlite3.blob");
  }

  sqlite3_blob* check_blob(lua_State* L, int arg) {
    return check_blob_handle(L, arg)->get();
  }

  namespace {
    void impl_gc(lua_State* L) {
      check_blob_handle(L, 1)->~blob_handle();
    }

    void impl_close(lua_State* L) {
      int result = check_blob_handle(L, 1)->close();
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }

#if SQLITE_VERSION_NUMBER >= 3007004
    void impl_reopen(lua_State* L) {
      sqlite3_blob* blob = check_blob(L, 1);
      sqlite3_int64 row = luaX_check_integer<sqlite3_int64>(L, 2);
      int result = sqlite3_blob_reopen(blob, row);
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }
#endif

    void impl_bytes(lua_State* L) {
      luaX_push(L, sqlite3_blob_bytes(check_blob(L, 1)));
    }

    void impl_read(lua_State* L) {
      sqlite3_blob* blob = check_blob(L, 1);
      int n = luaX_check_integer<int>(L, 2, 0, std::numeric_limits<int>::max());
      int offset = luaX_opt_integer<int>(L, 3, 0, 0, std::numeric_limits<int>::max());
      std::vector<char> buffer(n);
      int result = sqlite3_blob_read(blob, buffer.data(), n, offset);
      if (result == SQLITE_OK) {
        luaX_push(L, luaX_string_reference(&buffer[0], buffer.size()));
      } else {
        push_error(L, result);
      }
    }

    void impl_write(lua_State* L) {
      sqlite3_blob* blob = check_blob(L, 1);
      luaX_string_reference buffer = luaX_check_string(L, 2);
      int offset = luaX_opt_integer<int>(L, 3, 0, 0, std::numeric_limits<int>::max());
      size_t i = luaX_opt_range_i(L, 4, buffer.size());
      size_t j = luaX_opt_range_j(L, 5, buffer.size());
      int result = sqlite3_blob_write(blob, buffer.data() + i, j - i, offset);
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }
  }

  void initialize_blob(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.sqlite3.blob");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "close", impl_close);
#if SQLITE_VERSION_NUMBER >= 3007004
      luaX_set_field(L, -1, "reopen", impl_reopen);
#endif
      luaX_set_field(L, -1, "bytes", impl_bytes);
      luaX_set_field(L, -1, "read", impl_read);
      luaX_set_field(L, -1, "write", impl_write);
    }
    luaX_set_field(L, -2, "blob");
  }
}
