-- Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
local equal = require "equal"

os.remove "test.db"
local dbh = assert(sqlite3.open "test.db")
assert(dbh:busy_timeout(60000))

assert(dbh:changes() == 0)
assert(dbh:total_changes() == 0)

assert(dbh:exec [[
CREATE TABLE t (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  f FLOAT,
  i MY_INTEGER,
  t TEXT UNIQUE);
INSERT INTO t (f, i, t) VALUES (0.25, 17, 'foo');
INSERT INTO t (f, i, t) VALUES (0.50, 23, 'bar');
INSERT INTO t (f, i, t) VALUES (0.75, 37, 'baz');
]])
assert(dbh:last_insert_rowid() == 3)

assert(dbh:changes() == 1)
assert(dbh:total_changes() == 3)

assert(not dbh:exec [[
INSERT INTO t (f, i, t) VALUES (1, 42, 'foo')")
]])

local sth = assert(dbh:prepare [[
SELECT * FROM t
]])

assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column_count() == 4)
assert(sth:column_type(1) == sqlite3.SQLITE_INTEGER)
assert(sth:column_type(2) == sqlite3.SQLITE_FLOAT)
assert(sth:column_type(3) == sqlite3.SQLITE_INTEGER)
assert(sth:column_type(4) == sqlite3.SQLITE_TEXT)
assert(sth:column_decltype(3) == "MY_INTEGER")
assert(sth:column(1) == 1)
assert(sth:column(2) == 0.25)
assert(sth:column(3) == 17)
assert(sth:column(4) == "foo")

assert(sth:step() == sqlite3.SQLITE_ROW)
assert(equal(sth:columns(), {
  [1] = 2;
  [2] = 0.50;
  [3] = 23;
  [4] = "bar";
  id = 2;
  f = 0.50;
  i = 23;
  t = "bar";
}))
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column_count() == 4)
assert(sth:data_count() == 4)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:column_count() == 4)
assert(sth:data_count() == 0)
assert(sth:finalize())

local sth = assert(dbh:prepare [[
INSERT INTO t (f, i, t) VALUES (:f, :i, :t)
]])
assert(sth:bind_parameter_count() == 3)
assert(sth:bind_parameter_index ":f" == 1)
assert(sth:bind_parameter_index ":i" == 2)
assert(sth:bind_parameter_index ":t" == 3)
assert(sth:bind_parameter_name(1) == ":f")
assert(sth:bind_parameter_name(2) == ":i")
assert(sth:bind_parameter_name(3) == ":t")
assert(sth:bind_double(":f", 1))
assert(sth:bind_int64(":i", 42))
assert(sth:bind_text(":t", "qux"))

assert(dbh:last_insert_rowid() == 3)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(dbh:last_insert_rowid() == 4)
assert(sth:finalize())

assert(dbh:close())
assert(os.remove "test.db")
