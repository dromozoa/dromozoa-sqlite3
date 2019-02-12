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

#ifndef DROMOZOA_BIND_ATOMIC_HPP
#define DROMOZOA_BIND_ATOMIC_HPP

#include "mutex.hpp"

namespace dromozoa {
  namespace bind {
    template <class T>
    class atomic_count {
    public:
      atomic_count() : count_() {}

      T operator++() {
        lock_guard<> lock(mutex_);
        return ++count_;
      }

      T operator--() {
        lock_guard<> lock(mutex_);
        return --count_;
      }

    private:
      mutex mutex_;
      T count_;
      atomic_count(const atomic_count&);
      atomic_count& operator=(const atomic_count&);
    };
  }

  using bind::atomic_count;
}

#endif
