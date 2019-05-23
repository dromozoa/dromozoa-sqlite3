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

local multi = require "dromozoa.multi"
local unix = require "dromozoa.unix"
local sqlite3 = require "dromozoa.sqlite3"

local verbose = os.getenv "VERBOSE" == "1"

os.remove "test.db"
local flags = unix.bor(
    sqlite3.SQLITE_OPEN_READWRITE,
    sqlite3.SQLITE_OPEN_CREATE,
    sqlite3.SQLITE_OPEN_FULLMUTEX)
local dbh = assert(sqlite3.open_sharable("test.db", flags))
dbh:busy_timeout(5 * 1000)
dbh:exec [[
CREATE TABLE t(id INTEGER PRIMARY KEY AUTOINCREMENT, v INTEGER);
]]

local s = multi.state():load [====[
local unix = require "dromozoa.unix"
local sqlite3 = require "dromozoa.sqlite3"

local verbose = os.getenv "VERBOSE" == "1"

local ptr = ...
local dbh = assert(sqlite3.open_sharable(ptr))

-- local flags = unix.bor(
--     sqlite3.SQLITE_OPEN_READWRITE,
--     sqlite3.SQLITE_OPEN_CREATE,
--     sqlite3.SQLITE_OPEN_FULLMUTEX)
-- local dbh = assert(sqlite3.open_sharable("test.db", flags))

dbh:busy_timeout(1000)

local select_sth = assert(dbh:prepare [[
SELECT COUNT(*) AS n FROM t;
]])

local insert_sth = assert(dbh:prepare [[
INSERT INTO t (v) VALUES (:v);
]])

local m = 0

while true do
  assert(dbh:exec "BEGIN IMMEDIATE TRANSACTION;")

  unix.nanosleep(0.05)

  assert(select_sth:step() == sqlite3.SQLITE_ROW)

  local r = assert(select_sth:columns())
  assert(select_sth:reset())
  if verbose then
    print("> SELECT " .. r.n)
  end

  m = m + 1
  assert(insert_sth:bind(":v", m))
  assert(insert_sth:step() == sqlite3.SQLITE_DONE)
  assert(insert_sth:reset())

  if verbose then
    print("> INSERT " .. m)
  end

  assert(dbh:exec "COMMIT TRANSACTION;")

  unix.nanosleep(0.05)

  if r.n >= 10 then
    break
  end
end
]====]

local t = multi.thread(s, dbh:share())

local select_sth = assert(dbh:prepare [[
SELECT COUNT(*) AS n FROM t;
]])

while true do
  local r, message = dbh:exec "BEGIN IMMEDIATE TRANSACTION;"
  if r then
    assert(select_sth:step() == sqlite3.SQLITE_ROW)
    local r = assert(select_sth:columns())
    print("< SELECT " .. r.n)
    assert(select_sth:reset())
    assert(dbh:exec "COMMIT TRANSACTION;")
    if r.n >= 10 then
      break
    end
  else
    if verbose then
      print("MESSAGE " .. message)
    end
  end
  unix.nanosleep(0.05)
end

t:join()
