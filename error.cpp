// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <sstream>

#include "common.hpp"

namespace dromozoa {
  std::string error_to_string(int code) {
#if SQLITE_VERSION_NUMBER >= 3007015
    return sqlite3_errstr(code);
#else
    std::ostringstream out;
    out << "error number " << code;
    return out.str();
#endif
  }

  void push_error(lua_State* L, int code) {
    luaX_push(L, luaX_nil);
    luaX_push(L, error_to_string(code));
    luaX_push(L, code);
  }

  void push_error(lua_State* L, sqlite3* dbh) {
    int code = sqlite3_extended_errcode(dbh);
    luaX_push(L, luaX_nil);
    luaX_push(L, sqlite3_errmsg(dbh));
    luaX_push(L, code);
  }

  void push_error(lua_State* L, sqlite3_stmt* sth) {
    push_error(L, sqlite3_db_handle(sth));
  }
}
