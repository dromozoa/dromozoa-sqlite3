-- Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
local equal = require "equal"

local dbh = assert(sqlite3.open ":memory:")

assert(dbh:exec [[
CREATE TABLE t (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  f FLOAT,
  i INTEGER,
  t TEXT);
INSERT INTO t (f, i, t) VALUES(0.25, 17, 'foo');
INSERT INTO t (f, i, t) VALUES(0.50, 23, 'bar');
INSERT INTO t (f, i, t) VALUES(0.75, 37, 'baz');
]])

local data = {}
assert(dbh:exec([[
SELECT * FROM t;
UPDATE t SET i = 42 WHERE t = 'foo';
SELECT * FROM t WHERE t = 'foo';
]], function (columns)
  data[#data + 1] = { columns.i, columns.t }
end))

assert(equal(data, {
  { "17", "foo" };
  { "23", "bar" };
  { "37", "baz" };
  { "42", "foo" };
}))

assert(dbh:close())
