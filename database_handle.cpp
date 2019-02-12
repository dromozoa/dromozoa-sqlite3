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
  namespace {
    int close_impl(sqlite3* dbh) {
#if SQLITE_VERSION_NUMBER >= 3007014
      return sqlite3_close_v2(dbh);
#else
      return sqlite3_close(dbh);
#endif
    }
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
    int result = close_impl(dbh);

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

  database_handle_sharable_impl::database_handle_sharable_impl(const char* filename, int flags, const char* vfs) : dbh_() {
    int result = sqlite3_open_v2(filename, &dbh_, flags, vfs);
    if (result != SQLITE_OK) {
      sqlite3_close(dbh_);
      luaX_throw_failure(error_to_string(result), result);
      return;
    }
  }

  database_handle_sharable_impl::~database_handle_sharable_impl() {
    lock_guard<> lock(dbh_mutex_);
    if (dbh_) {
      sqlite3* dbh = dbh_;
      dbh_ = 0;
      int result = close_impl(dbh);
      if (result != SQLITE_OK) {
        DROMOZOA_UNEXPECTED(error_to_string(result));
      }
    }
  }

  void database_handle_sharable_impl::add_ref() {
    ++count_;
  }

  void database_handle_sharable_impl::release() {
    if (--count_ == 0) {
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
    return close_impl(dbh);
  }

  database_handle_sharable::database_handle_sharable(database_handle_sharable_impl* impl) : impl_(impl) {
    impl_->add_ref();
  }

  database_handle_sharable::~database_handle_sharable() {
    impl_->release();
  }

  sqlite3* database_handle_sharable::get() const {
    return impl_->get();
  }

  database_handle_sharable_impl* database_handle_sharable::share() const {
    return impl_;
  }

  int database_handle_sharable::close() {
    return impl_->close();
  }
}
