-- Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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
local test = bind.test

assert(bind.get_log_level() == 0)
assert(not bind.get_raise_error())

local result, message = pcall(test.throw)
assert(not result)
assert(message:find("exception caught"))
assert(message:find("test"))

local result, message = pcall(test.throw_int)
assert(not result)
assert(message:find("exception caught"))

local result, message, code = test.raise3()
assert(not result)
assert(message == "test")
assert(code == 42)

assert(bind.set_raise_error(true))

local result, message = pcall(test.raise0)
-- print(result, message)
assert(not result)
assert(message:find("error raised"))

local result, message = pcall(test.raise1)
assert(not result)
assert(message:find("error raised"))

local result, message = pcall(test.raise2)
assert(not result)
assert(message:find("test"))

local result, message = pcall(test.raise3)
assert(not result)
assert(message:find("test"))

local t = test.new()
assert(t:get() == 0)
assert(t:set(42):get() == 42)

assert(test.zero == 0)
assert(test.one == 1)

assert(bind.band(0x11, 0x33, 0x77, 0xff) == 0x11)
assert(bind.band(0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80) == 0x00)
assert(bind.bor(0x11, 0x33, 0x77, 0xff) == 0xff)
assert(bind.bor(0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80) == 0xff)
