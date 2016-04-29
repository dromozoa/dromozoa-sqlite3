-- Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local dbh = assert(sqlite3.open(":memory:"))

assert(dbh:exec([[
CREATE TABLE t (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  f FLOAT,
  i INTEGER,
  t TEXT);
INSERT INTO t (f, i, t) VALUES(0.25, 17, 'foo');
INSERT INTO t (f, i, t) VALUES(0.50, 23, 'bar');
INSERT INTO t (f, i, t) VALUES(0.75, 37, 'baz');
]]))

assert(dbh:create_function("dromozoa_function", 2, function (context, a, b)
  context:result_int64(a + b)
end))

local sth = assert(dbh:prepare([[
SELECT dromozoa_function(1, 2);
]]))
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 3)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:finalize())

local sum = 0
assert(dbh:create_aggregate("dromozoa_aggregate", 2, function (context, a, b)
  sum = sum + a + b
end, function (context)
  context:result_double(sum)
end))

local sth = dbh:prepare([[
SELECT dromozoa_aggregate(f, i) FROM t;
]])
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 78.5)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:finalize())

assert(dbh:create_function("dromozoa_error", 0, function (context)
  error("error")
end))
local sth = assert(dbh:prepare([[
SELECT dromozoa_error();
]]))
assert(not sth:step())
assert(sth:reset())
assert(sth:finalize())

assert(dbh:close())
