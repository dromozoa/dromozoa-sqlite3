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
  blob_handle::blob_handle(sqlite3_blob* blob) : blob_(blob) {}

  blob_handle::~blob_handle() {
    if (blob_) {
      int result = close();
      if (result != SQLITE_OK) {
        DROMOZOA_UNEXPECTED(error_to_string(result));
      }
    }
  }

  sqlite3_blob* blob_handle::get() const {
    return blob_;
  }

  int blob_handle::close() {
    sqlite3_blob* blob = blob_;
    blob_ = 0;
    return sqlite3_blob_close(blob);
  }
}
