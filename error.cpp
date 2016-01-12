// Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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
}

#include <sqlite3.h>

#include "error.hpp"

namespace dromozoa {
  int push_error(lua_State* L, int code) {
    lua_pushnil(L);
#if SQLITE_VERSION_NUMBER >= 3007015
    if (const char* what = sqlite3_errstr(code)) {
      lua_pushstring(L, what);
    } else
#endif
    {
      lua_pushfstring(L, "error number %d", code);
    }
    lua_pushinteger(L, code);
    return 3;
  }

  int push_error(lua_State* L, sqlite3* dbh) {
    int code = sqlite3_extended_errcode(dbh);
    lua_pushnil(L);
    if (const char* what = sqlite3_errmsg(dbh)) {
      lua_pushstring(L, what);
    } else {
      lua_pushfstring(L, "error number %d", code);
    }
    lua_pushinteger(L, code);
    return 3;
  }

  int push_error(lua_State* L, sqlite3_stmt* sth) {
    return push_error(L, sqlite3_db_handle(sth));
  }
}
