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
dbh:exec [[
CREATE TABLE t (id INTEGER PRIMARY KEY, v INTEGER, t TEXT);
]]

local insert_sth = assert(dbh:prepare [[
INSERT INTO t (v, t) VALUES (:v, :t);
]])

for i = 1, 100 do
  assert(insert_sth:bind(":v", i))
  assert(insert_sth:bind(":t", ("x"):rep(i)))
  assert(insert_sth:step() == sqlite3.SQLITE_DONE)
  assert(insert_sth:reset())
end

os.remove "test.db"
local backup = assert(sqlite3.backup_init(assert(sqlite3.open "test.db"), "main", dbh, "main"))
collectgarbage()
collectgarbage()

while true do
  local result, message, code = backup:step(1)
  if verbose then
    print(result, message, code)
  end
  if result == sqlite3.SQLITE_DONE then
    break
  end
  local remaining = backup:remaining()
  local pagecount = backup:pagecount()
  if verbose then
    print("remaining", remaining)
    print("pagecount", pagecount)
  end
end

assert(backup:finish())
assert(insert_sth:finalize())
assert(dbh:close())

local dbh = assert(sqlite3.open "test.db")
local count_sth = assert(dbh:prepare [[
SELECT COUNT(*) FROM t;
]])

assert(count_sth:step() == sqlite3.SQLITE_ROW)
assert(count_sth:column(1) == 100)
assert(count_sth:step() == sqlite3.SQLITE_DONE)
