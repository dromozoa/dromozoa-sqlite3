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

#include "common.hpp"

namespace dromozoa {
  namespace {
    backup_handle* check_backup_handle(lua_State* L, int arg) {
      return luaX_check_udata<backup_handle>(L, arg, "dromozoa.sqlite3.backup");
    }

    sqlite3_backup* check_backup(lua_State* L, int arg) {
      return check_backup_handle(L, arg)->get();
    }

    void impl_gc(lua_State* L) {
      check_backup_handle(L, 1)->~backup_handle();
    }

    void impl_finish(lua_State* L) {
      int result = check_backup_handle(L, 1)->finish();
      if (result == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, result);
      }
    }

    void impl_step(lua_State* L) {
      sqlite3_backup* backup = check_backup(L, 1);
      int page = luaX_check_integer<int>(L, 2);
      int result = sqlite3_backup_step(backup, page);
      if (result == SQLITE_OK || result == SQLITE_DONE) {
        luaX_push(L, result);
      } else {
        push_error(L, result);
      }
    }

    void impl_remaining(lua_State* L) {
      luaX_push(L, sqlite3_backup_remaining(check_backup(L, 1)));
    }

    void impl_pagecount(lua_State* L) {
      luaX_push(L, sqlite3_backup_pagecount(check_backup(L, 1)));
    }
  }

  void initialize_backup(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.sqlite3.backup");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "finish", impl_finish);
      luaX_set_field(L, -1, "step", impl_step);
      luaX_set_field(L, -1, "remaining", impl_remaining);
      luaX_set_field(L, -1, "pagecount", impl_pagecount);
    }
    luaX_set_field(L, -2, "backup");
  }
}
