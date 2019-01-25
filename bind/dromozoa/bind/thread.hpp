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

#ifndef DROMOZOA_BIND_THREAD_HPP
#define DROMOZOA_BIND_THREAD_HPP

#include <pthread.h>

#include <exception>
#include <iostream>
#include <utility>

#include "system_error.hpp"

namespace dromozoa {
  namespace bind {
    class thread {
    public:
      class id {
      public:
        id() : thread_() {}

        explicit id(pthread_t thread) : thread_(thread) {}

        friend std::ostream& operator<<(std::ostream& out, const id& self) {
          return out << self.thread_;
        }

      private:
        pthread_t thread_;
      };

      thread(void* (*start_routine)(void*), void* arg) : thread_(), joinable_() {
        if (int result = pthread_create(&thread_, 0, start_routine, arg)) {
          throw system_error(result);
        }
        joinable_ = true;
      }

      ~thread() {
        if (joinable_) {
          std::terminate();
        }
      }

      bool joinable() const {
        return joinable_;
      }

      void join() {
        if (int result = pthread_join(thread_, 0)) {
          throw system_error(result);
        }
        joinable_ = false;
      }

      void detach() {
        if (int result = pthread_detach(thread_)) {
          throw system_error(result);
        }
        joinable_ = false;
      }

      id get_id() const {
        return id(thread_);
      }

      pthread_t native_handle() {
        return thread_;
      }

      void swap(thread& that) {
        std::swap(thread_, that.thread_);
        std::swap(joinable_, that.joinable_);
      }

    private:
      pthread_t thread_;
      bool joinable_;
      thread(const thread&);
      thread& operator=(const thread&);
    };

    namespace this_thread {
      inline thread::id get_id() {
        return thread::id(pthread_self());
      }
    }
  }

  using bind::thread;
  namespace this_thread = bind::this_thread;
}

#endif
