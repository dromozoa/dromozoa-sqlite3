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

#ifndef DROMOZOA_BIND_CONDITION_VARIABLE_HPP
#define DROMOZOA_BIND_CONDITION_VARIABLE_HPP

#include <pthread.h>

#include <exception>

#include "mutex.hpp"
#include "system_error.hpp"

namespace dromozoa {
  namespace bind {
    class condition_variable {
    public:
      condition_variable() {
        if (int result = pthread_cond_init(&cond_, 0)) {
          throw system_error(result);
        }
      }

      ~condition_variable() {
        if (pthread_cond_destroy(&cond_)) {
          std::terminate();
        }
      }

      void notify_one() {
        if (int result = pthread_cond_signal(&cond_)) {
          throw system_error(result);
        }
      }

      void notify_all() {
        if (int result = pthread_cond_broadcast(&cond_)) {
          throw system_error(result);
        }
      }

      void wait(lock_guard<mutex>& lock) {
        if (int result = pthread_cond_wait(&cond_, lock.mutex()->native_handle())) {
          throw system_error(result);
        }
      }

      pthread_cond_t* native_handle() {
        return &cond_;
      }

    private:
      pthread_cond_t cond_;
      condition_variable(const condition_variable&);
      condition_variable& operator=(const condition_variable&);
    };
  }

  using bind::condition_variable;
}

#endif
