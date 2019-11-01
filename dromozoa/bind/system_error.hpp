// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_BIND_SYSTEM_ERROR_HPP
#define DROMOZOA_BIND_SYSTEM_ERROR_HPP

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <exception>
#include <sstream>
#include <string>

namespace dromozoa {
  namespace bind {
    class errno_saver {
    public:
      errno_saver() : code_(errno) {}

      ~errno_saver() {
        errno = code_;
      }

      int get() const {
        return code_;
      }

    private:
      int code_;
      errno_saver(const errno_saver&);
      errno_saver& operator=(const errno_saver&);
    };

    inline const char* compat_strerror_r_result(const char* result, char*) {
      return result;
    }

    inline const char* compat_strerror_r_result(int result, char* buffer) {
      if (result == 0) {
        return buffer;
      } else {
        if (result != -1) {
          errno = result;
        }
        return 0;
      }
    }

    inline const char* compat_strerror_r(int code, char* buffer, size_t size) {
      return compat_strerror_r_result(strerror_r(code, buffer, size), buffer);
    }

    inline std::string compat_strerror(int code) {
      errno_saver save_errno;
      errno = 0;
      std::string what;

      char* buffer = 0;
      size_t size = 16;
      while (true) {
        size = size * 2;
        if (char* result = static_cast<char*>(realloc(buffer, size))) {
          buffer = result;
          if (const char* result = compat_strerror_r(code, buffer, size)) {
            what = result;
          } else if (errno == ERANGE) {
            continue;
          }
        }
        break;
      }
      free(buffer);

      if (what.empty()) {
        std::ostringstream out;
        out << "error number " << code;
        what = out.str();
      }

      return what;
    }

    class system_error : public std::exception {
    public:
      explicit system_error(int code) : code_(code) {}

      virtual ~system_error() throw() {}

      virtual const char* what() const throw() {
        try {
          if (what_.empty()) {
            what_ = compat_strerror(code_);
          }
          return what_.c_str();
        } catch (const std::exception& e) {
          return e.what();
        }
      }

      int code() const {
        return code_;
      }

    private:
      int code_;
      mutable std::string what_;
    };
  }

  using bind::compat_strerror;
  using bind::errno_saver;
  using bind::system_error;
}

#endif
