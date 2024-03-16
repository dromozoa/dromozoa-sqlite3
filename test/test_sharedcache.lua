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

local unix = require "dromozoa.unix"
local sqlite3 = require "dromozoa.sqlite3"

local multi
local result, message = pcall(function ()
  multi = require "dromozoa.multi"
end)
if not result then
  print "[SKIP] dromozoa.multi not found"
  return
end

local verbose = os.getenv "VERBOSE" == "1"

os.remove "test.db"
local flags = unix.bor(
    sqlite3.SQLITE_OPEN_READWRITE,
    sqlite3.SQLITE_OPEN_CREATE,
    sqlite3.SQLITE_OPEN_FULLMUTEX,
    sqlite3.SQLITE_OPEN_SHAREDCACHE)
local dbh = assert(sqlite3.open("test.db", flags))
dbh:exec [[
CREATE TABLE t(id INTEGER PRIMARY KEY AUTOINCREMENT, v INTEGER);
]]

local s = multi.state():load [====[
local unix = require "dromozoa.unix"
local sqlite3 = require "dromozoa.sqlite3"

local verbose = os.getenv "VERBOSE" == "1"

local flags = unix.bor(
    sqlite3.SQLITE_OPEN_READONLY,
    sqlite3.SQLITE_OPEN_FULLMUTEX,
    sqlite3.SQLITE_OPEN_SHAREDCACHE)
local dbh = assert(sqlite3.open("test.db", flags))
dbh:exec [[
PRAGMA read_uncommitted = true;
]]

local sth = assert(dbh:prepare [[
SELECT COUNT(*) AS n FROM t;
]])

while true do
  if verbose then
    print "SELECT"
  end
  assert(sth:step() == sqlite3.SQLITE_ROW)
  local r = assert(sth:columns())
  assert(sth:reset())
  if verbose then
    print("SELECT done " .. r.n)
  end
  if r.n >= 10 then
    break
  end
end
]====]

local t = multi.thread(s)

local sth = assert(dbh:prepare [[
INSERT INTO t(v) VALUES(:v);
]])

for i = 1, 10 do
  if verbose then
    print("INSERT " .. i)
  end
  while true do
    assert(sth:bind(":v", i))
    local result, message = sth:step()
    assert(sth:reset())
    if result then
      break
    else
      print(message)
    end
  end
  if verbose then
    print "INSERT done"
  end
  unix.nanosleep(0.05)
end

t:join()
