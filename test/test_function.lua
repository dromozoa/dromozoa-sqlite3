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

local registry = debug.getregistry()
print(registry["dromozoa.sqlite3.function"])

local dbh = sqlite3.open(":memory:")
print("--")
for k, v in pairs(registry["dromozoa.sqlite3.function"]) do
  print(k, v)
end

print("--")
print(dbh:create_function("f", 1, function (a)
  print("f")
end))
print(dbh:create_function("g", -1, function () end))
print(dbh:create_function("h", -1, function () end))
-- print(dbh:create_function())

for k, v in pairs(registry["dromozoa.sqlite3.function"]) do
  print(v[1])
end

dbh:close()
print("--")
for k, v in pairs(registry["dromozoa.sqlite3.function"]) do
  print(k, v)
end

