# Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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

CPPFLAGS = -I$(LUA_INCDIR)
CXXFLAGS = -Wall -W -std=c++11 $(CFLAGS)
LDFLAGS = -L$(LUA_LIBDIR) $(LIBFLAG)
LDLIBS = -lsqlite3 -ldl

TARGET = sqlite3.so

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

sqlite3.so: dbh.o error.o log_level.o set_field.o sqlite3.o sth.o success.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

dbh.o: dbh.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

error.o: error.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

log_level.o: log_level.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

set_field.o: set_field.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

sqlite3.o: sqlite3.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

sth.o: sth.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

success.o: success.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

install:
	mkdir -p $(LIBDIR)/dromozoa
	cp $(TARGET) $(LIBDIR)/dromozoa
