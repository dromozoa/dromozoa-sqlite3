package = "dromozoa-sqlite3"
version = "1.0-1"
source = {
  url = "https://github.com/dromozoa/dromozoa-sqlite3/archive/v1.0.tar.gz";
  file = "dromozoa-sqlite3-1.0.tar.gz";
}
description = {
  summary = "Lua bindings for SQLite3";
  license = "GPL-3";
  homepage = "https://github.com/dromozoa/dromozoa-sqlite3/";
  maintainer = "Tomoyuki Fujimori <moyu@dromozoa.com>";
}
build = {
  type = "make";
  build_variables = {
    CFLAGS = "$(CFLAGS)";
    LIBFLAG = "$(LIBFLAG)";
    LUA_INCDIR = "$(LUA_INCDIR)";
    LUA_LIBDIR = "$(LUA_LIBDIR)";
    LUA = "$(LUA)";
  };
  install_variables = {
    BINDIR = "$(BINDIR)";
    LIBDIR = "$(LIBDIR)";
  };
}
