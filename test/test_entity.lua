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
local entity = sqlite3.entity

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
print(t1:insert_sql())
print(t1:insert({
  k = "foo";
  v = 42;
  select = 3.14;
}))

local t2 = sqlite3.entity(dbh, "t2")
print(json.encode(t2))
print(t2:insert_sql())

dbh:close()

assert(sqlite3.null == entity.null)
