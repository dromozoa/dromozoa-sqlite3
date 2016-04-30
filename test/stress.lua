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

local unix = require "dromozoa.unix"
local sqlite3 = require "dromozoa.sqlite3"

local filename, count = assert(...)
local count = tonumber(count)

local dbh = assert(sqlite3.open(filename))
assert(dbh:busy_timeout(60000))

assert(dbh:exec([[
PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

BEGIN IMMEDIATE;
CREATE TABLE IF NOT EXISTS t (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  t TEXT);
COMMIT;
]]))

local sth1 = assert(dbh:prepare("BEGIN IMMEDIATE;"))
local sth2 = assert(dbh:prepare("SELECT count(*) FROM t;"))
local sth3 = assert(dbh:prepare("INSERT INTO t (t) VALUES (:t);"))
local sth4 = assert(dbh:prepare("COMMIT;"))

for i = 1, count do
  if i % 100 == 0 then
    print(i)
    unix.nanosleep(0.2)
  end

  assert(sth1:reset())
  assert(assert(sth1:step()) == sqlite3.SQLITE_DONE)

  assert(sth2:reset())
  assert(assert(sth2:step()) == sqlite3.SQLITE_ROW)
  local count1 = assert(sth2:column(1))
  assert(assert(sth2:step()) == sqlite3.SQLITE_DONE)

  assert(sth3:reset())
  assert(sth3:bind(":t", "foo"))
  assert(assert(sth3:step()) == sqlite3.SQLITE_DONE)

  assert(sth2:reset())
  assert(assert(sth2:step()) == sqlite3.SQLITE_ROW)
  local count2 = assert(sth2:column(1))
  assert(assert(sth2:step()) == sqlite3.SQLITE_DONE)

  assert(count2 - count1 == 1)

  assert(sth4:reset())
  assert(assert(sth4:step()) == sqlite3.SQLITE_DONE)
end
