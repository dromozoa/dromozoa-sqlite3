// Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <sqlite3.h>

#include <map>
#include <string>
#include <utility>

#include <dromozoa/bind.hpp>

namespace dromozoa {
  class database_handle {
  public:
    explicit database_handle(sqlite3* dbh);
    ~database_handle();
    int close();
    sqlite3* get() const;
  private:
    friend class database_handle_access;
    sqlite3* dbh_;
#if SQLITE_VERSION_NUMBER < 3007003
    std::map<std::pair<std::string, int>, luaX_binder*> references_;
#endif
    database_handle(const database_handle&);
    database_handle& operator=(const database_handle&);
  };

  void new_dbh(lua_State* L, sqlite3* dbh);
  database_handle* check_database_handle(lua_State* L, int arg);
  sqlite3* check_dbh(lua_State* L, int arg);

  class statement_handle {
  public:
    explicit statement_handle(sqlite3_stmt* sth);
    ~statement_handle();
    void finalize();
    sqlite3_stmt* get() const;
  private:
    sqlite3_stmt* sth_;
    statement_handle(const statement_handle&);
    statement_handle& operator=(const statement_handle&);
  };

  void new_sth(lua_State* L, sqlite3_stmt* sth);
  sqlite3_stmt* check_sth(lua_State* L, int arg);

  void new_context(lua_State* L, sqlite3_context* context);

  std::string error_to_string(int code);
  void push_error(lua_State* L, int code);
  void push_error(lua_State* L, sqlite3* dbh);
  void push_error(lua_State* L, sqlite3_stmt* sth);
  void push_null(lua_State* L);
}

#endif
