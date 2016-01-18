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

local json = require "dromozoa.commons.json"
local sqlite3 = require "dromozoa.sqlite3"

-- sqlite3.set_log_level(3)
assert(sqlite3.initialize())

local dbh = assert(sqlite3.open("test.db"))
dbh:busy_timeout(60000)

assert(dbh:exec("CREATE TABLE IF NOT EXISTS t (id INTEGER PRIMARY KEY AUTOINCREMENT, k TEXT UNIQUE, v TEXT, n FLOAT)"))
assert(dbh:exec("INSERT INTO t (k, v, n) VALUES('foo', 'bar', 0.125)"))
assert(dbh:last_insert_rowid() == 1)
-- print(dbh:exec("INSERT INTO t (k, v, n) VALUES('foo', 'bar', 0.125)"))
assert(not dbh:exec("INSERT INTO t (k, v, n) VALUES('foo', 'bar', 0.125)"))
assert(dbh:exec("INSERT INTO t (k, v, n) VALUES('bar', 'baz', 17)"))
assert(dbh:last_insert_rowid() == 2)
assert(dbh:exec("INSERT INTO t (k, v, n) VALUES('baz', 'qux', 23)"))
assert(dbh:last_insert_rowid() == 3)

local sth = assert(dbh:prepare("SELECT * FROM t"))
assert(sth:step() == sqlite3.SQLITE_ROW)
-- print(sth:column_count())
assert(sth:column_count() == 4)
assert(sth:column_type(1) == sqlite3.SQLITE_INTEGER)
assert(sth:column_type(2) == sqlite3.SQLITE_TEXT)
assert(sth:column_type(3) == sqlite3.SQLITE_TEXT)
assert(sth:column_type(4) == sqlite3.SQLITE_FLOAT)
-- assert(sth:column_int64(1) == 1)
-- assert(sth:column_text(2) == "foo")
-- assert(sth:column_text(3) == "bar")
-- assert(sth:column_double(4) == 0.125)
assert(sth:column(1) == 1)
assert(sth:column(2) == "foo")
assert(sth:column(3) == "bar")
assert(sth:column(4) == 0.125)
print(json.encode(sth:columns()))
for i = 1, sth:column_count() do
  -- print(sth:column_type(i), sth:column_name(i))
  assert(sth:column_name(i))
end
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:finalize())

local sth = assert(dbh:prepare("INSERT INTO t (k, v, n) VALUES (:k, :v, :n)"))
assert(sth:bind_parameter_count() == 3)
assert(sth:bind_parameter_index(":k") == 1)
assert(sth:bind_parameter_index(":v") == 2)
assert(sth:bind_parameter_index(":n") == 3)
assert(sth:bind_parameter_name(1) == ":k")
assert(sth:bind_parameter_name(2) == ":v")
assert(sth:bind_parameter_name(3) == ":n")
assert(sth:bind_text(1, "qux"))
assert(sth:bind_int64(2, 42))
assert(sth:bind_double(3, 3.14))
assert(dbh:last_insert_rowid() == 3)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(dbh:last_insert_rowid() == 4)
assert(sth:finalize())

assert(dbh:close())

assert(sqlite3.shutdown())
