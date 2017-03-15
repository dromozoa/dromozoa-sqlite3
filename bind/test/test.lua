-- Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local result, message = pcall(bind.throw)
assert(not result)
assert(message:find("exception caught:") == 1)
assert(message:find("runtime_error"))

local a, b, c = bind.result_int()
assert(a == true)
assert(b == 42)
assert(c == "foo")

local a, b, c = bind.result_void()
assert(a == true)
assert(b == 42)
assert(c == "foo")

assert(bind.push_nil() == nil)

local a, b = bind.push_enum()
assert(a == 42)
assert(b == -42)

local a, b, c, d, e = bind.push_string()
assert(a == "foo")
assert(b == "foo")
assert(c == "foo")
assert(d == "foo")
assert(e == "foo")

assert(bind.push_success() == true)
assert(bind.push_success(42) == 42)

local t = {}
bind.set_field(t)
assert(t[1] == 17)
assert(t[2] == 23)
assert(t[3] == 37)
assert(t[4] == 42)
assert(t[5] == nil)
assert(t.s == "foo")

local t = { 42, 42, 42, 42, 42, s = "bar" }
bind.set_field(t)
assert(t[1] == 17)
assert(t[2] == 23)
assert(t[3] == 37)
assert(t[4] == 42)
assert(t[5] == nil)
assert(t.s == "foo")

local function test_opt_range(a, b, i, j)
  assert(a == i)
  assert(b == j)
end

test_opt_range(0, 3, bind.opt_range(3))
test_opt_range(3, 3, bind.opt_range(3, 4))
test_opt_range(2, 3, bind.opt_range(3, 3))
test_opt_range(1, 3, bind.opt_range(3, 2))
test_opt_range(0, 3, bind.opt_range(3, 1))
test_opt_range(0, 3, bind.opt_range(3, 0))
test_opt_range(2, 3, bind.opt_range(3, -1))
test_opt_range(1, 3, bind.opt_range(3, -2))
test_opt_range(0, 3, bind.opt_range(3, -3))
test_opt_range(0, 3, bind.opt_range(3, -4))
test_opt_range(0, 3, bind.opt_range(3, 1, 4))
test_opt_range(0, 3, bind.opt_range(3, 1, 3))
test_opt_range(0, 2, bind.opt_range(3, 1, 2))
test_opt_range(0, 1, bind.opt_range(3, 1, 1))
test_opt_range(0, 0, bind.opt_range(3, 1, 0))
test_opt_range(0, 3, bind.opt_range(3, 1, -1))
test_opt_range(0, 2, bind.opt_range(3, 1, -2))
test_opt_range(0, 1, bind.opt_range(3, 1, -3))
test_opt_range(0, 0, bind.opt_range(3, 1, -4))

bind.check_integer(0, 0, 0, 0)
assert(not pcall(bind.check_integer))
assert(not pcall(bind.check_integer, 0))
assert(not pcall(bind.check_integer, 0, 0))
assert(not pcall(bind.check_integer, 0, 0, 0))
assert(not pcall(bind.check_integer, 32768, 0, 0, 0));
assert(not pcall(bind.check_integer, -32769, 0, 0, 0));
assert(not pcall(bind.check_integer, 0, -1, 0, 0));
assert(not pcall(bind.check_integer, 0, 65536, 0, 0));
assert(not pcall(bind.check_integer, 0, 0, -1, 0));
assert(not pcall(bind.check_integer, 0, 0, 0, -1));
assert(not pcall(bind.check_integer, 0, 0, 0, 256));

bind.opt_integer()
bind.opt_integer(0)
bind.opt_integer(0, 0)
bind.opt_integer(0, 0, 0)
bind.opt_integer(0, 0, 0, 0)
assert(not pcall(bind.opt_integer, 32768))
assert(not pcall(bind.opt_integer, -32769))
assert(not pcall(bind.opt_integer, 0, -1))
assert(not pcall(bind.opt_integer, 0, 65536))
assert(not pcall(bind.opt_integer, 0, 0, -1))
assert(not pcall(bind.opt_integer, 0, 0, 0, -1))
assert(not pcall(bind.opt_integer, 0, 0, 0, 256))

bind.check_enum(bind.ENUM42)
bind.check_enum(bind.ENUM69)
assert(not pcall(bind.check_enum, "foo"))

assert(bind.opt_enum() == bind.ENUM42)
assert(bind.opt_enum(bind.ENUM42) == bind.ENUM42)
assert(bind.opt_enum(bind.ENUM69) == bind.ENUM69)
assert(not pcall(bind.check_enum, "foo"))

local a, b = assert(bind.check_integer_field({ foo = 42, [42] = "42" }))
assert(a == 42)
assert(b == 42)
assert(not pcall(bind.check_integer_field, {}))
assert(not pcall(bind.check_integer_field, { foo = "bar" }))
assert(not pcall(bind.check_integer_field, { foo = 42 }))
assert(not pcall(bind.check_integer_field, { foo = 42, [42] = "bar" }))

assert(bind.check_integer_field_range({ nice = 0 }) == 0)
assert(not pcall(bind.check_integer_field_range, { nice = 42 }))
assert(not pcall(bind.check_integer_field_range, { nice = "bar" }))

assert(bind.opt_integer_field({}) == 0)
assert(bind.opt_integer_field({ foo = 42 }) == 42)
assert(not pcall(bind.opt_integer_field, { foo = "bar" }))
assert(not pcall(bind.opt_integer_field, { foo = -1 }))
assert(not pcall(bind.opt_integer_field, { [42] = -1 }))

assert(bind.opt_integer_field_range({}) == 0)
assert(bind.opt_integer_field_range({ tv_usec = 42 }) == 42)
assert(not pcall(bind.opt_integer_field_range, { tv_usec = "foo" }))
assert(not pcall(bind.opt_integer_field_range, { tv_usec = -1 }))
assert(not pcall(bind.opt_integer_field_range, { tv_usec = 1000000 }))

assert(not pcall(bind.field_error1))
assert(not pcall(bind.field_error2))
assert(not pcall(bind.field_error3))

local a, b = bind.set_metafield()
assert(getmetatable(a).a == "a")
assert(getmetatable(a).b == "b")
assert(b == nil)

local DBL_MAX = 1.7976931348623157e+308
local DBL_DENORM_MIN = 4.9406564584124654e-324
local DBL_MIN = 2.2250738585072014e-308
local DBL_EPSILON = 2.2204460492503131e-16

assert(bind.is_integer(42))
assert(bind.is_integer(42 / 2))
assert(not bind.is_integer(1.25))
assert(bind.is_integer(1.25e6))
if bind.sizeof_lua_integer == 4 then
  assert(not bind.is_integer(1.25e12))
else
  assert(bind.is_integer(1.25e12))
  assert(bind.is_integer(1.25e18))
  assert(not bind.is_integer(1.25e24))
end

assert(not bind.is_integer(DBL_MAX))
assert(not bind.is_integer(DBL_DENORM_MIN))
assert(not bind.is_integer(DBL_MIN))
assert(not bind.is_integer(DBL_EPSILON))
assert(not bind.is_integer(math.pi))

assert(bind.is_integer(-1 / math.huge)) -- -0
assert(not bind.is_integer(math.huge))  -- inf
assert(not bind.is_integer(-math.huge)) -- -inf
assert(not bind.is_integer(0 / 0))      -- nan

assert(bind():get() == 0)
assert(bind():set(42):get() == 42)
assert(bind(42):get() == 42)
assert(bind(42):to() == nil)
assert(bind(42):to("foo", "bar", "baz", "qux") == nil)
assert(bind(42):to("dromozoa.bind.int", "bar", "baz", "qux") == 42)
assert(bind(42):to("foo", "dromozoa.bind.int", "baz", "qux") == 42)
assert(bind(42):to("foo", "bar", "dromozoa.bind.int", "qux") == 42)
assert(bind(42):to("foo", "bar", "baz", "dromozoa.bind.int") == 42)

bind.unexpected()

local sum = 0
bind.set_callback(function (v) sum = sum + v end, print)

assert(sum == 0)
bind.run_callback_i(17)
bind.run_callback_i(23)
bind.run_callback_i(37)
bind.run_callback_i(42)
assert(sum == 119)

bind.run_callback_s("foo")
bind.run_callback_s("bar")
bind.run_callback_s("baz")

bind.run_destructor()

bind.ref(42)
assert(bind.get_field() == 42)
bind.unref()
local result, message = pcall(bind.get_field)
print(result, message)
assert(not result)
