-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-bind.
--
-- dromozoa-bind is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-bind is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.

local bind = require "dromozoa.bind"

local ma
local mb

bind.handle.clear_destructed()

assert(not bind.handle.is_destructed(42))
do
  local a = bind.handle(42)
  assert(not bind.handle.is_destructed(42))
  assert(a:get() == 42)
  assert(bind.handle.get(a) == 42)
  ma = getmetatable(a)
end
collectgarbage()
collectgarbage()
assert(bind.handle.is_destructed(42))

assert(not bind.handle.is_destructed(69))
do
  local b = bind.handle_ref(69)
  assert(not bind.handle.is_destructed(69))
  assert(b:get() == 69)
  assert(bind.handle.get(b) == 69)
  mb = getmetatable(b)
end
collectgarbage()
collectgarbage()
assert(not bind.handle.is_destructed(69))

assert(ma ~= mb)
assert(ma.__index == bind.handle)
assert(mb.__index == bind.handle)
