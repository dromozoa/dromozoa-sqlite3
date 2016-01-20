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

local function quote(value)
  return "'" .. value:gsub("'", "''") .. "'"
end

local function decltype_to_affinity(decltype)
  decltype = decltype:upper()
  if decltype:find("INT") then
    return "INTEGER"
  elseif decltype:find("CHAR") or decltype:find("CLOB") or decltype:find("TEXT") then
    return "TEXT"
  elseif decltype:find("BLOB") then
    return "BLOB"
  elseif decltype:find("REAL") or decltype:find("FLOA") or decltype:find("DOUB") then
    return "REAL"
  else
    return "NUMERIC"
  end
end

local function load_table_info(dbh, name)
  local columns = sequence()
  dbh:exec("PRAGMA table_info(" .. quote(name) .. ")", function (r)
    local decltype = r.type;
    columns:push({
      name = r.name;
      decltype = decltype;
      affinity = decltype_to_affinity(decltype);
      primary_key = tonumber(r.pk) ~= 0;
      not_null = tonumber(r.notnull) ~= 0;
    })
  end)
  return columns
end

local class = {}

function class.new(dbh, name)
  return {
    dbh = dbh;
    name = name;
    columns = columns;
  }
end

function class:insert(this)
  local dbh = self.dbh
  local name = self.name
  local names = sequence()
  local values = sequence()
  for column in self.columns:each() do
    local name = column.name
    local value = this[name]
    if value ~= nil then
      names:push(name)
      values:push(value)
    end
  end
  local out = sequence_writer()
  out:write("INSERT INTO ", quote(self.name), " (", names:concat(", "), ") VALUES (", ("?"):rep(#name, ","), ")")
  return out:concat()
end

function class:insert_sql()
  local out = sequence_writer()
  out:write("INSERT INTO ", quote(self.name), " (")
  local first = true
  for column in self.columns:each() do
    print(column.primary_key, column.affinity)
    if column.primary_key and column.affinity == "INTEGER" then
      -- assume rowid or auto increment
      print("is rowid or auto increment")
    else
      if first then
        first = false
      else
        out:write(",")
      end
      out:write(quote(column.name))
    end
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

local metatable = {
  __index = class;
}

return setmetatable(class, {
  __call = function (_, dbh, name)
    return setmetatable(class.new(dbh, name), metatable)
  end;
})
