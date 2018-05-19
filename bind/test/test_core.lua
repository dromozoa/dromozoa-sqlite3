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

local verbose = os.getenv "VERBOSE" == "1"

--
-- result
--

local function check_result(...)
  assert(select("#", ...) == 5)
  if verbose then
    print(...)
  end
  local a, b, c, d, e = ...
  assert(a == nil)
  assert(b == 0.25)
  assert(c == 42)
  assert(d == true)
  assert(e == "foo")
end

check_result(bind.core.result_int())
check_result(bind.core.result_void())

local function check_none_or_nil(n, ...)
  assert(select("#", ...) == n)
  assert(... == nil)
end

--
-- push
--

check_none_or_nil(0, bind.core.push_none())
check_none_or_nil(1, bind.core.push_nil())

assert(bind.core.push_enum() == 42)

local function check_string(...)
  assert(select("#", ...) == 14)
  if verbose then
    print(...)
  end
  for i = 1, 5 do
    assert(select(i, ...) == "あいうえお")
  end
  assert(select(6, ...) == "foo\0bar\0baz")
  for i = 7, 10 do
    assert(select(i, ...) == "foo")
  end
  for i = 11, 14 do
    assert(select(i, ...) == "bar")
  end
end

check_string(bind.core.push_string())

assert(bind.core.push_success() == true)
assert(bind.core.push_success(42) == 42)

--
-- error
--

local function check_error(fn, expect)
  local result, message = pcall(fn)
  if verbose then
    print(result, message)
  end
  assert(not result)
  if expect then
    assert(message:find(expect, 1, true))
  end
end

bind.core.unexpected()
check_error(bind.core.throw, "exception caught: runtime_error")
check_error(bind.core.field_error1, "field nil not an integer")
check_error(bind.core.field_error2, "field userdata:")
check_error(bind.core.field_error3, [[field "\1\2\3\4\5\6\a\b\t\n\v\f\r\14\15\16]])
check_error(bind.core.field_error4, [[field "foo\0bar\0baz"]])

--
-- field
--

local t = bind.core.set_field(16)
assert(#t == 16)
for i = 1, 16 do
  assert(t[i] == i)
end
assert(t.foo == "bar")
assert(t.baz == "qux")

local t = bind.core.set_metafield()
assert(getmetatable(t)["dromozoa.bind.a"] == 42)
assert(getmetatable(t)["dromozoa.bind.b"] == "あいうえお")

--
-- failure
--

local result, message, code = bind.core.failure1()
assert(not result)
assert(message == "failure1")
assert(not code)

local result, message, code = bind.core.failure2()
assert(not result)
assert(message == "failure2")
assert(code == 69)

local result, message, code = bind.core.failure3()
assert(not result)
assert(message == "failure3")
assert(not code)

local result, message, code = bind.core.failure4()
assert(not result)
assert(message == "failure4")
assert(code == 69)

--
-- top_saver
--

local result = bind.core.top_saver()
assert(result == 0)
