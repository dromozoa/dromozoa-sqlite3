// Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_BIND_TRANSLATE_RANGE_HPP
#define DROMOZOA_BIND_TRANSLATE_RANGE_HPP

extern "C" {
#include <lua.h>
}

#include <stddef.h>

namespace dromozoa {
  namespace bind {
    size_t translate_range_i(lua_State* L, int n, size_t size);
    size_t translate_range_j(lua_State* L, int n, size_t size);
  }
}

#endif
