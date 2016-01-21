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

local function make_quoted_names(columns, object)
  local quoted_names = sequence()
  for column in columns:each() do
    local name = column.name
    if object[name] ~= nil then
      quoted_names:push(quote(name))
    end
  end
  return quoted_names
end

local class = {}

function class.new(dbh, name)
  return {
    name = name;
    columns = load_table_info(dbh, name);
  }
end

function class:insert_sql(object, command)
  if command == nil then
    command = "INSERT"
  end
  local quoted_names = make_quoted_names(self.columns, object)
  local out = sequence_writer():write(command, " INTO ", quote(self.name), " (", quoted_names:concat(", "), ") VALUES (")
  local n = #quoted_names
  if n > 0 then
    out:write("?")
    for i = 2, n do
      out:write(", ?")
    end
  end
  return out:write(")"):concat()
end

function class:update_sql(object, command)
  if command == nil then
    command = "UPDATE"
  end
  local quoted_names = make_quoted_names(self.columns, object)
  return command .. " " .. quote(self.name) .. " SET " .. quoted_names:concat(" = ?, ") .. " = ?"
end

function class:bind(sth, i, object)
  for column in self.columns:each() do
    local value = object[column.name]
    if value ~= nil then
      local t = type(value)
      if t == "number" then
        if column.decltype == "INTEGER" then
          sth:bind_int64(i, value)
        else
          sth:bind_double(i, value)
        end
      elseif t == "string" then
        if column.decltype == "BLOB" then
          sth:bind_blob(i, value)
        else
          sth:bind_text(i, value)
        end
      elseif t == "boolean" then
        if value then
          sth:bind_int64(i, 1)
        else
          sth:bind_int64(i, 0)
        end
      elseif value == class.null then
        sth:bind_null(i)
      else
        sth:bind_text(i, tostring(value))
      end
      i = i + 1
    end
  end
  return i
end

function class:insert(dbh, object, command)
  local sql = self:insert_sql(object, command)
  local sth = dbh:prepare(sql)
  self:bind(sth, 1, object)
  local result = sth:step()
  sth:finalize()
  return result
end

function class:update(dbh, rowid, object, command)
  local sql = self:update_sql(object, command) .. " WHERE rowid = ?"
  local sth = dbh:prepare(sql)
  local i = self:bind(sth, 1, object)
  sth:bind_int64(i, rowid)
  local result = sth:step()
  sth:finalize()
  return result
end

function class:delete(dbh, rowid)
  local sql = "DELETE FROM " .. quote(self.name) .. " WHERE rowid = ?"
  local sth = dbh:prepare(sql)
  sth:bind_int64(1, rowid)
  local result = sth:step()
  sth:finalize()
  return result
end

local metatable = {
  __index = class;
}

return setmetatable(class, {
  __call = function (_, dbh, name)
    return setmetatable(class.new(dbh, name), metatable)
  end;
})
