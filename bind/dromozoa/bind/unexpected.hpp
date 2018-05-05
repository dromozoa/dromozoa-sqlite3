// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-bind.
//
// dromozoa-bind is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-bind is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DROMOZOA_BIND_UNEXPECTED_HPP
#define DROMOZOA_BIND_UNEXPECTED_HPP

#include <stdio.h>

#include <string>

namespace dromozoa {
  namespace bind {
    typedef void (*unexpected_handler)(const char* what, const char* file, int line, const char* function);

    inline void unexpected_noop(const char*, const char*, int, const char*) {}

    inline void unexpected_cerr(const char* what, const char* file, int line, const char* function) {
      if (!what) {
        what = "(null)";
      }
      if (!file) {
        file = "(null)";
      }
      if (!function) {
        function = "(null)";
      }
      fprintf(stderr, "unexpected: %s at %s:%d in %s\n", what, file, line, function);
      fflush(stderr);
    }

    inline unexpected_handler access_unexpected(bool set, unexpected_handler new_handler) {
      static unexpected_handler handler = unexpected_cerr;
      if (set) {
        handler = new_handler;
        return 0;
      } else {
        return handler;
      }
    }

    inline void set_unexpected(unexpected_handler handler) {
      access_unexpected(true, handler);
    }

    inline void unexpected(const char* what, const char* file, int line, const char* function) {
      if (unexpected_handler handler = access_unexpected(false, 0)) {
        handler(what, file, line, function);
      }
    }

    inline void unexpected(const std::string& what, const char* file, int line, const char* function) {
      unexpected(what.c_str(), file, line, function);
    }
  }

  using bind::set_unexpected;
  using bind::unexpected;
  using bind::unexpected_cerr;
  using bind::unexpected_handler;
  using bind::unexpected_noop;
}

#define DROMOZOA_UNEXPECTED(what) \
  dromozoa::bind::unexpected((what), __FILE__, __LINE__, __func__)

#endif
