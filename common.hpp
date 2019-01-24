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
#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  class database_handle {
  public:
    virtual ~database_handle() = 0;
    virtual sqlite3* get() const = 0;
    virtual int close() = 0;
  };

  class database_handle_impl : public database_handle {
  public:
    explicit database_handle_impl(sqlite3*);
    ~database_handle_impl();
    sqlite3* get() const;
    int close();
  private:
    friend class database_handle_access;
    sqlite3* dbh_;
#if SQLITE_VERSION_NUMBER < 3007003
    std::map<std::pair<std::string, int>, luaX_binder*> references_;
#endif
    database_handle_impl(const database_handle_impl&);
    database_handle_impl& operator=(const database_handle_impl&);
  };

  class database_handle_sharable_impl {
  public:
    explicit database_handle_sharable_impl(sqlite3*);
    ~database_handle_sharable_impl();
    void add_ref();
    void release();
    sqlite3* get();
    int close();
  private:
    long counter_;
    mutex counter_mutex_;
    sqlite3* dbh_;
    mutex dbh_mutex_;
    database_handle_sharable_impl(const database_handle_sharable_impl&);
    database_handle_sharable_impl& operator=(const database_handle_sharable_impl&);
  };

  class database_handle_sharable : public database_handle {
  public:
    explicit database_handle_sharable(database_handle_sharable_impl*);
    ~database_handle_sharable();
    sqlite3* get() const;
    database_handle_sharable_impl* share() const;
    int close();
  private:
    database_handle_sharable_impl* impl_;
    database_handle_sharable(const database_handle_sharable&);
    database_handle_sharable& operator=(const database_handle_sharable&);
  };

  void new_dbh(lua_State*, sqlite3*);
  database_handle* check_database_handle(lua_State*, int);
  database_handle_impl* check_database_handle_impl(lua_State*, int);
  database_handle_sharable* check_database_handle_sharable(lua_State*, int);
  sqlite3* check_dbh(lua_State*, int);

  class statement_handle {
  public:
    explicit statement_handle(sqlite3_stmt*);
    ~statement_handle();
    sqlite3_stmt* get() const;
    void finalize();
  private:
    sqlite3_stmt* sth_;
    statement_handle(const statement_handle&);
    statement_handle& operator=(const statement_handle&);
  };

  void new_sth(lua_State*, sqlite3_stmt*);
  sqlite3_stmt* check_sth(lua_State*, int);

  void new_context(lua_State*, sqlite3_context*);

  std::string error_to_string(int);
  void push_error(lua_State*, int);
  void push_error(lua_State*, sqlite3*);
  void push_error(lua_State*, sqlite3_stmt*);
  void push_null(lua_State*);
}

#endif
