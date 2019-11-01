# Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
#
# This file is part of dromozoa-bind.
#
# dromozoa-bind is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dromozoa-bind is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dromozoa-bind.  If not, see <http://www.gnu.org/licenses/>.

CPPFLAGS += -I$(LUA_INCDIR)
CXXFLAGS += -Wall -W $(CFLAGS)
LDLIBS += -lpthread -ldl

OBJS = \
	atomic.o \
	callback.o \
	common.o \
	core.o \
	module.o \
	mutex.o \
	handle.o \
	scoped_ptr.o \
	system_error.o \
	thread.o \
	util.o
TARGET = bind.so

all: $(TARGET) driver

clean:
	rm -f *.o $(TARGET) driver

check:
	./test.sh

bind.so: $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBFLAG) $^ $(LDLIBS) -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

driver: driver.c
	$(CC) $(CPPFLAGS) -Wall -W $(CFLAGS) $(LDFLAGS) $< -llua -o $@

install:
	mkdir -p $(LIBDIR)/dromozoa
	cp $(TARGET) $(LIBDIR)/dromozoa
