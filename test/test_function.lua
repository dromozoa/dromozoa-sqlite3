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

sqlite3.set_log_level(2)
sqlite3.set_raise_error(true)

local dbh = sqlite3.open(":memory:")

dbh:exec([[
CREATE TABLE t (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  f FLOAT,
  i INTEGER,
  t TEXT);
INSERT INTO t (f, i, t) VALUES(0.25, 17, 'foo');
INSERT INTO t (f, i, t) VALUES(0.50, 23, 'bar');
INSERT INTO t (f, i, t) VALUES(0.75, 37, 'baz');
]])

dbh:create_function("dromozoa_function", 2, function (context, a, b)
  context:result_int64(a + b)
end)

local sth = dbh:prepare([[
SELECT dromozoa_function(1, 2);
]])
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 3)
assert(sth:step() == sqlite3.SQLITE_DONE)
sth:finalize()

local sum = 0
dbh:create_aggregate("dromozoa_aggregate", 2, function (context, a, b)
  -- print("a", a, b)
  sum = sum + a + b
end, function (context)
  -- print("f", sum)
  context:result_double(sum)
end)

local sth = dbh:prepare([[
SELECT dromozoa_aggregate(f, i) FROM t;
]])
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 78.5)
assert(sth:step() == sqlite3.SQLITE_DONE)
sth:finalize()

dbh:create_function("dromozoa_error", 0, function (context)
  error("error")
end)
local sth = dbh:prepare([[
SELECT dromozoa_error();
]])
local result, message = pcall(sth.step, sth)
-- print(result, message)
assert(not result)
sth:reset()
sth:finalize()

dbh:close()
