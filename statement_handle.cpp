// Copyright (C) 2016,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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
  statement_handle::statement_handle(sqlite3_stmt* sth) : sth_(sth) {}

  statement_handle::~statement_handle() {
    if (sth_) {
      finalize();
    }
  }

  sqlite3_stmt* statement_handle::get() const {
    return sth_;
  }

  void statement_handle::finalize() {
    sqlite3_stmt* sth = sth_;
    sth_ = 0;
    sqlite3_finalize(sth);
  }
}
