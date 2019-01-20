-- Copyright (C) 2017-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local libversion = assert(sqlite3.libversion())
local libversion_number = assert(sqlite3.libversion_number())
local sourceid = assert(sqlite3.sourceid())

if verbose then
  io.stderr:write(libversion, "\n")
  io.stderr:write(libversion_number, "\n")
  io.stderr:write(sourceid, "\n")
end
