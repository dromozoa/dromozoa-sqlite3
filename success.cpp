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

#include "success.hpp"

namespace dromozoa {
  int push_success(lua_State* L) {
    if (lua_isuserdata(L, 1)) {
      lua_pushvalue(L, 1);
      return 1;
    } else {
      lua_pushinteger(L, SQLITE_OK);
      return 1;
    }
  }
}
