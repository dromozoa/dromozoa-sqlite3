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

local sequence = require "dromozoa.commons.sequence"
local sequence_writer = require "dromozoa.commons.sequence_writer"

local function type_to_affinity(t)
  t = t:upper()
  if t:find("INT") then
    return "INTEGER"
  elseif t:find("CHAR") or t:find("CLOB") or t:find("TEXT") then
    return "TEXT"
  elseif t:find("BLOB") then
    return "BLOB"
  elseif t:find("REAL") or t:find("FLOA") or t:find("DOUB") then
    return "REAL"
  else
    return "NUMERIC"
  end
end

local function quote(value)
  return "'" .. value:gsub("'", "''") .. "'"
end

local function columns(dbh, name)
  local columns = sequence()
  dbh:exec("PRAGMA table_info(" .. quote(name) .. ")", function (r)
    columns:push({
      name = r.name;
      type = r.type;
      affinity = type_to_affinity(r.type);
      primary_key = r.pk ~= "0";
      not_null = r.notnull ~= "0";
    })
  end)
  return columns;
end

local class = {}

function class.new(dbh, name)
  return {
    dbh = dbh;
    name = name;
    columns = columns(dbh, name);
  }
end

function class:insert_sql()
  local out = sequence_writer()
  out:write("INSERT INTO ", quote(self.name), " (")
  local first = true
  for column in self.columns:each() do
    if first then
      first = false
    else
      out:write(",")
    end
    out:write(quote(column.name))
  end
  out:write(") VALUES (")
  local first = true
  for column in self.columns:each() do
    if first then
      first = false
    else
      out:write(",")
    end
    out:write("?")
  end
  return out:write(")"):concat()
end

function class:update_sql()
end

local metatable = {
  __index = class;
}

return setmetatable(class, {
  __call = function (_, dbh, name)
    return setmetatable(class.new(dbh, name), metatable)
  end;
})
