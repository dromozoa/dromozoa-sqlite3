-- Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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

sqlite3.set_log_level(3)
assert(sqlite3.initialize())

local dbh = assert(sqlite3.open("test.db", sqlite3.SQLITE_OPEN_READWRITE + sqlite3.SQLITE_OPEN_CREATE))
dbh:busy_timeout(60000)

assert(dbh:exec("CREATE TABLE IF NOT EXISTS t (id INTEGER PRIMARY KEY AUTOINCREMENT, k TEXT UNIQUE, v TEXT, n FLOAT)"))
assert(dbh:exec("INSERT INTO t (k, v, n) VALUES('foo', 'bar', 0.125)"))
assert(not dbh:exec("INSERT INTO t (k, v, n) VALUES('foo', 'bar', 0.125)"))
assert(dbh:exec("INSERT INTO t (k, v, n) VALUES('bar', 'baz', 17)"))
assert(dbh:exec("INSERT INTO t (k, v, n) VALUES('baz', 'qux', 23)"))

local sth = assert(dbh:prepare("SELECT * FROM t"))
assert(sth:step())
-- print(sth:column_count())
assert(sth:column_count() == 4)
assert(sth:column_type(1) == sqlite3.SQLITE_INTEGER)
assert(sth:column_type(2) == sqlite3.SQLITE_TEXT)
assert(sth:column_type(3) == sqlite3.SQLITE_TEXT)
assert(sth:column_type(4) == sqlite3.SQLITE_FLOAT)
assert(sth:column_int64(1) == 1)
assert(sth:column_text(2) == "foo")
assert(sth:column_text(3) == "bar")
assert(sth:column_double(4) == 0.125)
for i = 1, sth:column_count() do
  print(sth:column_type(i), sth:column_name(i))
  assert(sth:column_name(i))
end

assert(sth:finalize())
assert(dbh:close())

assert(sqlite3.shutdown())
