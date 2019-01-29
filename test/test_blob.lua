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

local data = [[
foo
bar
baz
qux
]]

local dbh = assert(sqlite3.open ":memory:")

assert(dbh:exec [[
CREATE TABLE t (
  id INTEGER PRIMARY KEY,
  t BLOB);
]])

local sth = assert(dbh:prepare [[
INSERT INTO t (t) VALUES (:t);
]])

assert(sth:bind(":t", data))
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:reset())
assert(dbh:last_insert_rowid() == 1)

assert(sth:bind(":t", "123456"))
assert(sth:step() == sqlite3.SQLITE_DONE)
assert(sth:reset())
assert(dbh:last_insert_rowid() == 2)

assert(sth:finalize())

local sth = assert(dbh:prepare [[
SELECT t FROM t WHERE id = :id;
]])

local function fetch(id)
  assert(sth:reset())
  assert(sth:bind(":id", id))
  assert(sth:step() == sqlite3.SQLITE_ROW)
  local t = sth:column(1)
  assert(sth:step() == sqlite3.SQLITE_DONE)
  return t
end

local blob = assert(dbh:blob_open("main", "t", "t", 1))
if verbose then
  print(blob)
end

assert(blob:bytes() == #data)
assert(blob:read(#data) == data)
assert(blob:read(4) == "foo\n")
assert(blob:read(4, 4) == "bar\n")
assert(blob:read(4, 8) == "baz\n")
assert(blob:read(4, 12) == "qux\n")

local result, message = pcall(blob.read, blob, -1)
if verbose then
  print(message)
end
assert(not result)

local result, message = blob:read(#data + 1)
if verbose then
  print(message)
end
assert(not result)

local result, message = blob:read(1, #data)
if verbose then
  print(message)
end
assert(not result)

if verbose then
  print("reopen", blob.reopen)
end
if blob.reopen then
  assert(blob:reopen(2))
else
  assert(blob:close())
  blob = assert(dbh:blob_open("main", "t", "t", 2))
end
assert(fetch(2) == "123456")
assert(blob:write("ABC"))
assert(fetch(2) == "ABC456")
assert(blob:write("DEF", 3))
assert(fetch(2) == "ABCDEF")
assert(blob:write("abcdef", 3, 2, 4))
assert(fetch(2) == "ABCbcd")
assert(blob:close())
assert(sth:finalize())

assert(dbh:create_function("dromozoa_test", 1, function (context, n)
  context:result_zeroblob(n)
end))

local sth = assert(dbh:prepare [[
SELECT dromozoa_test(:n);
]])

assert(sth:bind(":n", 42))
assert(sth:step() == sqlite3.SQLITE_ROW)
assert(sth:column(1) == ("\0"):rep(42))
assert(sth:step() == sqlite3.SQLITE_DONE)

assert(dbh:close())
