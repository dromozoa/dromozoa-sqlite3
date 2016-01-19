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
  print(columns.name, columns.sql)
end)

local sth = dbh:prepare([[
PRAGMA table_info('t1');
]])
while sth:step() == sqlite3.SQLITE_ROW do
  local c = sth:columns()
  c[1] = nil
  print(json.encode(c))
end
sth:finalize()

local sth = dbh:prepare([[
SELECT * FROM t1 WHERE 'select' = :select;
]])
print(sth:bind_parameter_name(1))
sth:finalize()

local t = entity(dbh, "t2")
print(json.encode(t))
print(t:insert_sql())

dbh:close()
