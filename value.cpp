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
}

#include <sqlite3.h>

namespace dromozoa {
  int push_value(lua_State* L, sqlite3_value* value) {
    switch (sqlite3_value_type(value)) {
      case SQLITE_INTEGER:
        lua_pushinteger(L, sqlite3_value_int64(value));
        return 1;
      case SQLITE_FLOAT:
        lua_pushnumber(L, sqlite3_value_double(value));
        return 1;
      case SQLITE3_TEXT:
        if (const char* text = reinterpret_cast<const char*>(sqlite3_value_text(value))) {
          lua_pushlstring(L, text, sqlite3_value_bytes(value));
          return 1;
        } else {
          return 0;
        }
      case SQLITE_BLOB:
        if (const char* blob = static_cast<const char*>(sqlite3_value_blob(value))) {
          lua_pushlstring(L, blob, sqlite3_value_bytes(value));
          return 1;
        } else {
          return 0;
        }
      case SQLITE_NULL:
        return 0;
    }
    return 0;
  }
}
