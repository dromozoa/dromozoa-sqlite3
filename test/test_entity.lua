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
  v INTEGER NOT NULL);
INSERT INTO t1 (k, v) VALUES (0, 0);
CREATE TABLE t2 (
  id INTEGER PRIMARY KEY,
  k TEXT UNIQUE,
  v TEXT);
ALTER TABLE t1 ADD COLUMN "select" DOUBLE;
SELECT * from SQLITE_MASTER WHERE type = 'table';
]], function (columns)
  -- print(columns.name)
end)

local t1 = sqlite3.entity(dbh, "t1")
-- print(json.encode(t1))
assert(t1.columns[1].primary_key)
assert(not t1.columns[2].primary_key)
assert(not t1.columns[2].not_null)
assert(not t1.columns[3].primary_key)
assert(t1.columns[3].not_null)

local data = {
  k = "foo";
  v = 42;
  select = 0.25;
}
local sql = t1:insert_sql(data)
-- print(sql)
local sth = dbh:prepare(sql)
local i = t1:bind(sth, 1, data)
assert(i == 4)
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(dbh:last_insert_rowid() == 2)
sth:finalize()

dbh:exec([[
SELECT * FROM t1 WHERE id = 4;
]], function (r)
  assert(r.k == "foo")
  assert(r.v == "42")
  assert(r.select == "0.25")
end)

local data = {
  k = "bar"
}
local sql = t1:update_sql(data) .. " WHERE id = ?"
local sth = dbh:prepare(sql)
local i = t1:bind(sth, 1, data)
assert(i == 2)
sth:bind_int64(i, 2)
assert(sth:step() == sqlite3.SQLITE_DONE)
sth:finalize()

dbh:exec([[
SELECT * FROM t1 WHERE id = 4;
]], function (r)
  assert(r.k == "bar")
  assert(r.v == "42")
  assert(r.select == "0.25")
end)

local t2 = sqlite3.entity(dbh, "t2")
assert(t2:insert(dbh, { k = "foo", v = "bar" }) == sqlite3.SQLITE_DONE)
assert(dbh:last_insert_rowid() == 1)

dbh:exec([[
SELECT * FROM t2;
]], function (r)
  assert(r.id == "1")
  assert(r.k == "foo")
  assert(r.v == "bar")
end)

assert(t2:update(dbh, 1, { v = "baz" }) == sqlite3.SQLITE_DONE)

dbh:exec([[
SELECT * FROM t2;
]], function (r)
  assert(r.id == "1")
  assert(r.k == "foo")
  assert(r.v == "baz")
end)

assert(t2:insert(dbh, { k = "bar" }, "REPLACE") == sqlite3.SQLITE_DONE)
assert(t2:insert(dbh, { k = "baz" }, "REPLACE") == sqlite3.SQLITE_DONE)
assert(t2:insert(dbh, { k = "qux" }, "REPLACE") == sqlite3.SQLITE_DONE)

dbh:exec([[
SELECT COUNT(*) AS n FROM t2;
]], function (r)
  assert(r.n == "4")
end)

assert(t2:delete(dbh, 1) == sqlite3.SQLITE_DONE)

dbh:exec([[
SELECT COUNT(*) AS n FROM t2;
]], function (r)
  assert(r.n == "3")
end)

dbh:close()

assert(sqlite3.null == sqlite3.entity.null)
