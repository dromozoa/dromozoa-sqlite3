package = "dromozoa-bind"
version = "1.1-1"
source = {
  url = "https://github.com/dromozoa/dromozoa-bind/archive/v1.1.tar.gz";
  file = "dromozoa-bind-1.1.tar.gz";
}
description = {
  summary = "Helper library for creating bindings between C++ and Lua";
  license = "GPL-3";
  homepage = "https://github.com/dromozoa/dromozoa-lua/";
  maintainer = "Tomoyuki Fujimori <moyu@dromozoa.com>";
}
build = {
  type = "make";
  build_variables = {
    CFLAGS = "$(CFLAGS)";
    LIBFLAG = "$(LIBFLAG)";
    LUA_INCDIR = "$(LUA_INCDIR)";
    LUA_LIBDIR = "$(LUA_LIBDIR)";
  };
  install_variables = {
    LIBDIR = "$(LIBDIR)";
  };
}
