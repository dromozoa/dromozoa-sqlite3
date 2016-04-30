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
  t TEXT UNIQUE);
]]))

local sth = assert(dbh:prepare([[
INSERT INTO t (f, i, t) VALUES (:f, :i, :t);
]]))

assert(sth:bind(":f", 0.25))
assert(sth:bind(":i", 42))
assert(sth:bind(":t", "foobarbaz"))
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:reset())

assert(sth:bind(":f", 17))
assert(sth:bind(":i", true))
assert(sth:bind(":t", "foobarbaz", 4, 6))
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:finalize())

local sth = assert(dbh:prepare([[
SELECT * FROM t ORDER BY id;
]]))

assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 1)
assert(sth:column(2) == 0.25)
assert(sth:column(3) == 42)
assert(sth:column(4) == "foobarbaz")

assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == 2)
assert(sth:column(2) == 17)
assert(sth:column(3) == 1)
assert(sth:column(4) == "bar")

assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:finalize())

assert(dbh:close())
