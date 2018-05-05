-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-bind.
--
-- dromozoa-bind is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-bind is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.

local bind = require "dromozoa.bind"

local verbose = os.getenv "VERBOSE" == "1"

if verbose then
  print(select("#", ...))
  for k, v in pairs(arg) do
    print(k, v)
  end
end

local bind_count = bind.handle(bind.count)
local hook_count = 0

if verbose then
  print("bind", bind_count, bind_count:get(), hook_count)
end

bind = nil
collectgarbage()
collectgarbage()

return function ()
  hook_count = hook_count + 1
  if verbose then
    print("hook", bind_count, bind_count:get(), hook_count)
  end
end
