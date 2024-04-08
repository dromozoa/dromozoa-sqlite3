#! /bin/sh -e

# Copyright (C) 2024 Tomoyuki Fujimori <moyu@dromozoa.com>
#
# This file is part of dromozoa-sqlite3.
#
# dromozoa-sqlite3 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dromozoa-sqlite3 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dromozoa-sqlite3.  If not, see <http://www.gnu.org/licenses/>.

name=".config-sqlite3_enable_load_extension"
cat <<EOH >"$name.c"
#include <sqlite3.h>

int main(int ac, char* av[]) {
  sqlite3_enable_load_extension(0, 0);
  return 0;
}
EOH

trap "rm -f '$name' '$name.c'" 0

if cc $CPPFLAGS $LDFLAGS "$name.c" -lsqlite3 -o "$name" >/dev/null 2>&1
then
  echo '#define HAVE_SQLITE3_ENABLE_LOAD_EXTENSION 1'
else
  echo '/* #undef HAVE_SQLITE3_ENABLE_LOAD_EXTENSION */'
fi
