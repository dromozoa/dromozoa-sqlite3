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

sqlite3.set_log_level(2)
sqlite3.set_raise_error(true)

local dbh = sqlite3.open(":memory:")

dbh:exec([[
CREATE TABLE t1 (
  id INTEGER PRIMARY KEY,
  k TEXT UNIQUE,
  v INTEGER);
INSERT INTO t1 (k, v) VALUES (0, 0);
CREATE TABLE t2 (
  id INTEGER PRIMARY KEY,
  k TEXT UNIQUE,
  v TEXT);
ALTER TABLE t1 ADD COLUMN "select" DOUBLE;
SELECT * from SQLITE_MASTER WHERE type = 'table';
]], function (columns)
  print(columns.name)
end)

local t1 = sqlite3.entity(dbh, "t1")
print(json.encode(t1))

local data = {
  k = "foo";
  v = 42;
  select = 3.14;
}
local sql = t1:insert_sql(data)
print(sql)
local sth = dbh:prepare(sql)
local i = t1:bind(sth, 1, data)
assert(i == 4)
assert(sth:step() == sqlite3.SQLITE_DONE)
sth:finalize()

local sth = dbh:prepare([[
SELECT rowid AS rowid, * FROM t1;
]])
while sth:step() == sqlite3.SQLITE_ROW do
  local r = sth:columns()
  print(r.rowid, r.id, r.k, r.v, r.select)
  r[1] = nil
  print(json.encode(r))
end
sth:finalize()

dbh:close()

assert(sqlite3.null == sqlite3.entity.null)
