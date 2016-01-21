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
  i INTEGER,
  f FLOAT,
  t TEXT,
  b BLOB,
  e TEXT);
INSERT INTO t (e) VALUES('end');
]])

local sth = dbh:prepare([[
SELECT * FROM t;
]])
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 1)
assert(sth:column(2) == sqlite3.null)
assert(sth:column(3) == sqlite3.null)
assert(sth:column(4) == sqlite3.null)
assert(sth:column(5) == sqlite3.null)
assert(sth:column(6) == "end")

local r = sth:columns()
assert(#r == 6)
assert(r[1] == 1)
assert(r[2] == sqlite3.null)
assert(r[3] == sqlite3.null)
assert(r[4] == sqlite3.null)
assert(r[5] == sqlite3.null)
assert(r[6] == "end")
assert(r.id == 1)
assert(r.i == sqlite3.null)
assert(r.f == sqlite3.null)
assert(r.t == sqlite3.null)
assert(r.b == sqlite3.null)
assert(r.e == "end")

assert(sth:step() == sqlite3.SQLITE_DONE)
sth:finalize()

dbh:exec([[
SELECT * FROM t;
]], function (r)
  assert(#r == 6)
  assert(r[1] == "1")
  assert(r[2] == sqlite3.null)
  assert(r[3] == sqlite3.null)
  assert(r[4] == sqlite3.null)
  assert(r[5] == sqlite3.null)
  assert(r[6] == "end")
  assert(r.id == "1")
  assert(r.i == sqlite3.null)
  assert(r.f == sqlite3.null)
  assert(r.t == sqlite3.null)
  assert(r.b == sqlite3.null)
  assert(r.e == "end")
end)

dbh:create_aggregate("dromozoa_test", 6, function (context, id, i, f, t, b, e)
  assert(id == 1)
  assert(i == sqlite3.null)
  assert(f == sqlite3.null)
  assert(t == sqlite3.null)
  assert(b == sqlite3.null)
  assert(e == "end")
end, function (context)
  context:result_null()
end)

dbh:exec([[
SELECT dromozoa_test(id, i, f, t, b, e) FROM t;
]])

dbh:close()
