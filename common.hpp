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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <sqlite3.h>

#include <set>

#include <dromozoa/bind.hpp>

namespace dromozoa {
  class function_handle;

  class database_handle {
  public:
    explicit database_handle(sqlite3* dbh);
    ~database_handle();
    int close();
    sqlite3* get() const;
    function_handle* new_function(lua_State* L, int arg);
    function_handle* new_aggregate(lua_State* L, int arg, int arg_final);
  private:
    sqlite3* dbh_;
    std::set<function_handle*> function_;
    database_handle(const database_handle&);
    database_handle& operator=(const database_handle&);
  };

  class statement_handle {
  public:
    explicit statement_handle(sqlite3_stmt* sth);
    ~statement_handle();
    int finalize();
    sqlite3_stmt* get() const;
  private:
    sqlite3_stmt* sth_;
    statement_handle(const statement_handle&);
    statement_handle& operator=(const statement_handle&);
  };

  class function_handle {
    friend class database_handle;
  public:
    function_handle(lua_State* L, int arg);
    function_handle(lua_State* L, int arg, int arg_final);
    ~function_handle();
    void call_func(sqlite3_context* context, int argc, sqlite3_value** argv) const;
    void call_step(sqlite3_context* context, int argc, sqlite3_value** argv) const;
    void call_final(sqlite3_context* context) const;
    int call_exec(int count, char** columns, char** names) const;
  private:
    lua_State* L_;
    int ref_;
    int ref_final_;
    function_handle(const function_handle&);
    function_handle& operator=(const function_handle&);
  };

  int push_error(lua_State* L, sqlite3* dbh);
  int push_error(lua_State* L, sqlite3_stmt* sth);
  int push_error(lua_State* L, int code);
  std::string error_to_string(int code);

  void push_null(lua_State* L);

  database_handle* check_database_handle(lua_State* L, int arg);

  void new_sth(lua_State* L, sqlite3_stmt* sth);
  sqlite3_stmt* get_sth(lua_State* L, int n);
  int open_sth(lua_State* L);

  void new_context(lua_State* L, sqlite3_context* context);
  void initialize_context(lua_State* L);
  void new_dbh(lua_State* L, sqlite3* dbh);
  sqlite3* get_dbh(lua_State* L, int n);
  void initialize_dbh(lua_State* L);
}

#endif
