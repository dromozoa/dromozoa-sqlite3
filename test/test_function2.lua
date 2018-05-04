-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-sqlite3.
--
-- dromozoa-sqlite3 is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-sqlite3 is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-sqlite3.  If not, see <http://www.gnu.org/licenses/>.

local sqlite3 = require "dromozoa.sqlite3"

local dbh = assert(sqlite3.open ":memory:")

assert(dbh:create_function("dromozoa_function", -1, function (context)
  context:result(17)
end))
assert(dbh:create_function("dromozoa_function", 0, function (context)
  context:result(23)
end))
assert(dbh:create_function("dromozoa_function", 1, function (context)
  context:result(42)
end))

assert(dbh:exec("select dromozoa_function() as v", function (columns)
  assert(columns.v == "23")
end))
assert(dbh:exec("select dromozoa_function('foo') as v", function (columns)
  assert(columns.v == "42")
end))
assert(dbh:exec("select dromozoa_function('foo', 'bar') as v", function (columns)
  assert(columns.v == "17")
end))

assert(dbh:create_function("dromozoa_function", 1, function (context)
  context:result(69)
end))

assert(dbh:exec("select dromozoa_function() as v", function (columns)
  assert(columns.v == "23")
end))
assert(dbh:exec("select dromozoa_function('foo') as v", function (columns)
  assert(columns.v == "69")
end))
assert(dbh:exec("select dromozoa_function('foo', 'bar') as v", function (columns)
  assert(columns.v == "17")
end))
