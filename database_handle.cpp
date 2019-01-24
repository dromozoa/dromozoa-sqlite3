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

#include "common.hpp"

namespace dromozoa {
  database_handle_sharable_impl::database_handle_sharable_impl(sqlite3* dbh) : counter_(), dbh_(dbh) {}

  database_handle_sharable_impl::~database_handle_sharable_impl() {
    lock_guard<> lock(dbh_mutex_);
    if (dbh_) {
      sqlite3* dbh = dbh_;
      dbh_ = 0;
#if SQLITE_VERSION_NUMBER >= 3007014
      int result = sqlite3_close_v2(dbh);
#else
      int result = sqlite3_close(dbh);
#endif
      if (result != SQLITE_OK) {
        DROMOZOA_UNEXPECTED(error_to_string(result));
      }
    }
  }

  void database_handle_sharable_impl::add_ref() {
    lock_guard<> lock(counter_mutex_);
    ++counter_;
  }

  void database_handle_sharable_impl::release() {
    lock_guard<> lock(counter_mutex_);
    if (--counter_ == 0) {
      delete this;
    }
  }

  sqlite3* database_handle_sharable_impl::get() {
    lock_guard<> lock(dbh_mutex_);
    return dbh_;
  }

  int database_handle_sharable_impl::close() {
    lock_guard<> lock(dbh_mutex_);
    sqlite3* dbh = dbh_;
    dbh_ = 0;
#if SQLITE_VERSION_NUMBER >= 3007014
    int result = sqlite3_close_v2(dbh);
#else
    int result = sqlite3_close(dbh);
#endif
    return result;
  }

  database_handle::~database_handle() {}

  database_handle_impl::database_handle_impl(sqlite3* dbh) : dbh_(dbh) {}

  database_handle_impl::~database_handle_impl() {
    if (dbh_) {
      int result = close();
      if (result != SQLITE_OK) {
        DROMOZOA_UNEXPECTED(error_to_string(result));
      }
    }
  }

  sqlite3* database_handle_impl::get() const {
    return dbh_;
  }

  int database_handle_impl::close() {
    sqlite3* dbh = dbh_;
    dbh_ = 0;
#if SQLITE_VERSION_NUMBER >= 3007014
    int result = sqlite3_close_v2(dbh);
#else
    int result = sqlite3_close(dbh);
#endif

#if SQLITE_VERSION_NUMBER < 3007003
    std::map<std::pair<std::string, int>, luaX_binder*>::iterator i = references_.begin();
    std::map<std::pair<std::string, int>, luaX_binder*>::iterator end = references_.end();
    for (; i != end; ++i) {
      scoped_ptr<luaX_binder> deleter(i->second);
    }
    references_.clear();
#endif

    return result;
  }
}
