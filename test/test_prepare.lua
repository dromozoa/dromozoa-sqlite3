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

-- sqlite3.set_log_level(3)
assert(sqlite3.initialize())

local dbh = assert(sqlite3.open(":memory:"))

local sql = "SELECT 1; SELECT 2"
local sth1, i = assert(dbh:prepare(sql))
assert(i == 10)
local sth2, i = assert(dbh:prepare(sql, i))
assert(i == 19)

assert(sth2:step() == sqlite3.SQLITE_ROW)
assert(sth2:column(1) == 2)
assert(sth2:step() == sqlite3.SQLITE_DONE)

assert(sth1:step() == sqlite3.SQLITE_ROW)
assert(sth1:column(1) == 1)
assert(sth1:step() == sqlite3.SQLITE_DONE)

local sth = dbh:prepare(" ")
-- print(sth:step())
print(sth:sql())
