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

local sqlite3 = require "dromozoa.sqlite3"

sqlite3.set_log_level(3)
sqlite3.set_raise_error(true)

local reg = debug.getregistry()
for i, v in ipairs(reg) do
  print(i, v)
end

local dbh = sqlite3.open(":memory:")

dbh:create_function("f", 3, function (context, a, b, c)
  print("f", context, a, b, c)
  -- context:result_int64(42)
  context:result_blob("foobarbaz")
end)
-- print(dbh:create_function("g", -1, function () end))
-- print(dbh:create_function("h", -1, function () end))
-- print(dbh:create_function())

local sth = dbh:prepare([[
  SELECT f(3.14, 'foo', 42);
]])
sth:step()
print(sth:column_type(1))
print(sth:column_blob(1))
sth:step()
sth:finalize()

for i, v in ipairs(reg) do
  print(i, v)
end

dbh:close()

for i, v in ipairs(reg) do
  print(i, v)
end
