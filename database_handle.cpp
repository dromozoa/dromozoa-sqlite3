// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
  database_handle::database_handle(sqlite3* dbh) : dbh_(dbh) {}

  database_handle::~database_handle() {
    if (dbh_) {
      int code = close();
      if (code != SQLITE_OK) {
        DROMOZOA_UNEXPECTED(error_to_string(code));
      }
    }
  }

  int database_handle::close() {
    sqlite3* dbh = dbh_;
    dbh_ = 0;
#if SQLITE_VERSION_NUMBER >= 3007014
    int result = sqlite3_close_v2(dbh);
#else
    int result = sqlite3_close(dbh);
#endif

    std::set<luaX_binder*>::iterator i = references_.begin();
    std::set<luaX_binder*>::iterator end = references_.end();
    for (; i != end; ++i) {
      scoped_ptr<luaX_binder> deleter(*i);
    }
    references_.clear();

    return result;
  }

  sqlite3* database_handle::get() const {
    return dbh_;
  }

  luaX_reference<>* database_handle::new_function(lua_State* L, int index_func) {
    scoped_ptr<luaX_reference<> > reference(new luaX_reference<>(L, index_func));
    references_.insert(reference.get());
    return reference.release();
  }

  luaX_reference<2>* database_handle::new_aggregate(lua_State* L, int index_step, int index_final) {
    scoped_ptr<luaX_reference<2> > reference(new luaX_reference<2>(L, index_step, index_final));
    references_.insert(reference.get());
    return reference.release();
  }
}
