-- Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local verbose = os.getenv "VERBOSE" == "1"

local dbh = assert(sqlite3.open ":memory:")

assert(dbh:exec [[
CREATE TABLE t (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  t BLOB);
]])

local sth = assert(dbh:prepare [[
INSERT INTO t (t) VALUES (:t);
]])

local data = [[
foo
bar
baz
qux
]]

assert(sth:bind(":t", data))
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:reset())

local id = dbh:last_insert_rowid()
assert(id == 1)

local blob = assert(dbh:blob_open("main", "t", "t", id))
if verbose then
  print(blob)
end

assert(blob:bytes() == #data)

assert(blob:close())

assert(dbh:close())
