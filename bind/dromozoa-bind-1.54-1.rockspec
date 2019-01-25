rockspec_format = "3.0"
package = "dromozoa-bind"
version = "1.54-1"
source = {
  url = "https://github.com/dromozoa/dromozoa-bind/archive/v1.54.tar.gz";
  file = "dromozoa-bind-1.54.tar.gz";
}
description = {
  summary = "Helper library for creating bindings between C++ and Lua";
  license = "GPL-3";
  homepage = "https://github.com/dromozoa/dromozoa-bind/";
  maintainer = "Tomoyuki Fujimori <moyu@dromozoa.com>";
}
test = {
  type = "command";
  command = "./test.sh";
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
