# Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

CPPFLAGS += -Ibind -I$(LUA_INCDIR)
CXXFLAGS += -Wall -W $(CFLAGS) -g
LDLIBS += -lsqlite3 -lpthread -ldl

OBJS = \
	backup.o \
	backup_handle.o \
	blob.o \
	blob_handle.o \
	context.o \
	database_handle.o \
	dbh.o \
	dbh_function.o \
	error.o \
	main.o \
	module.o \
	statement_handle.o \
	sth.o \
	sth_bind.o \
	sth_column.o
TARGET = sqlite3.so

all: $(TARGET)

clean:
	rm -f config.h *.o $(TARGET)

check:
	./test.sh

install:
	mkdir -p $(LIBDIR)/dromozoa
	cp $(TARGET) $(LIBDIR)/dromozoa

sqlite3.so: $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBFLAG) $^ $(LDLIBS) -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

config.h: config.sh
	./config.sh >$@

dbh.o: config.h
