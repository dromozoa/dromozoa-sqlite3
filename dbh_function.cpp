// Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-sqlite3.
//
// dromozoa-sqlite3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-sqlite3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-sqlite3.  If not, see <http://www.gnu.org/licenses/>.

#include "common.hpp"

namespace dromozoa {
  class database_handle_access {
#if SQLITE_VERSION_NUMBER < 3007003
  public:
    static int create_function(database_handle_impl* self, const char* name, int narg, lua_State* L, int index_func) {
      scoped_ptr<luaX_binder> reference(new luaX_reference<>(L, index_func));
      create(self->references_, name, narg, reference.get());
      return sqlite3_create_function(self->get(), name, narg, SQLITE_UTF8, reference.release(), func_callback, 0, 0);
    }

    static int create_aggregate(database_handle_impl* self, const char* name, int narg, lua_State* L, int index_step, int index_final) {
      scoped_ptr<luaX_binder> reference(new luaX_reference<2>(L, index_step, index_final));
      create(self->references_, name, narg, reference.get());
      return sqlite3_create_function(self->get(), name, narg, SQLITE_UTF8, reference.release(), 0, step_callback, final_callback);
    }

    static int delete_function(database_handle_impl* self, const char* name, int narg) {
      int result = sqlite3_create_function(self->get(), name, narg, SQLITE_UTF8, 0, 0, 0, 0);
      if (result == SQLITE_OK) {
        std::map<std::pair<std::string, int>, luaX_binder*>& references = self->references_;
        std::map<std::pair<std::string, int>, luaX_binder*>::iterator i = references.find(std::make_pair(name, narg));
        if (i != references.end()) {
          scoped_ptr<luaX_binder> deleter(i->second);
          references.erase(i);
        }
      }
      return result;
    }

  private:
    static void create(std::map<std::pair<std::string, int>, luaX_binder*>& references, const char* name, int narg, luaX_binder* reference) {
      std::pair<std::string, int> key(name, narg);
      std::map<std::pair<std::string, int>, luaX_binder*>::iterator i = references.find(key);
      if (i == references.end()) {
        references.insert(std::make_pair(key, reference));
      } else {
        scoped_ptr<luaX_binder> deleter(i->second);
        i->second = reference;
      }
    }
#else
  public:
    static int create_function(database_handle_impl* self, const char* name, int narg, lua_State* L, int index_func) {
      scoped_ptr<luaX_binder> reference(new luaX_reference<>(L, index_func));
      return sqlite3_create_function_v2(self->get(), name, narg, SQLITE_UTF8, reference.release(), func_callback, 0, 0, destroy_callback);
    }

    static int create_aggregate(database_handle_impl* self, const char* name, int narg, lua_State* L, int index_step, int index_final) {
      scoped_ptr<luaX_binder> reference(new luaX_reference<2>(L, index_step, index_final));
      return sqlite3_create_function_v2(self->get(), name, narg, SQLITE_UTF8, reference.release(), 0, step_callback, final_callback, destroy_callback);
    }

    static int delete_function(database_handle_impl* self, const char* name, int narg) {
      return sqlite3_create_function_v2(self->get(), name, narg, SQLITE_UTF8, 0, 0, 0, 0, 0);
    }

  private:
    static void destroy_callback(void* data) {
      scoped_ptr<luaX_binder> deleter(static_cast<luaX_binder*>(data));
    }
#endif

  private:
    static bool push_value(lua_State* L, sqlite3_value* value) {
      switch (sqlite3_value_type(value)) {
        case SQLITE_INTEGER:
          luaX_push(L, sqlite3_value_int64(value));
          return true;
        case SQLITE_FLOAT:
          luaX_push(L, sqlite3_value_double(value));
          return true;
        case SQLITE_TEXT:
          if (const unsigned char* text = sqlite3_value_text(value)) {
            luaX_push(L, luaX_string_reference(text, sqlite3_value_bytes(value)));
            return true;
          } else {
            return false;
          }
        case SQLITE_BLOB:
          if (const char* blob = static_cast<const char*>(sqlite3_value_blob(value))) {
            luaX_push(L, luaX_string_reference(blob, sqlite3_value_bytes(value)));
            return true;
          } else {
            return false;
          }
        case SQLITE_NULL:
          push_null(L);
          return true;
        default:
          return false;
      }
    }

    template <size_t T_i, class T>
    static void callback(T* ref, sqlite3_context* context, int argc, sqlite3_value** argv) {
      lua_State* L = ref->state();
      luaX_top_saver save_top(L);
      {
        ref->get_field(L, T_i);
        luaX_new<sqlite3_context*>(L, context);
        luaX_set_metatable(L, "dromozoa.sqlite3.context");
        for (int i = 0; i < argc; ++i) {
          if (!push_value(L, argv[i])) {
            luaX_push(L, luaX_nil);
          }
        }
        if (lua_pcall(L, argc + 1, 0, 0) != 0) {
          sqlite3_result_error(context, lua_tostring(L, -1), -1);
        }
      }
    }

    static void func_callback(sqlite3_context* context, int argc, sqlite3_value** argv) {
      callback<0>(static_cast<luaX_reference<>*>(sqlite3_user_data(context)), context, argc, argv);
    }

    static void step_callback(sqlite3_context* context, int argc, sqlite3_value** argv) {
      callback<0>(static_cast<luaX_reference<2>*>(sqlite3_user_data(context)), context, argc, argv);
    }

    static void final_callback(sqlite3_context* context) {
      callback<1>(static_cast<luaX_reference<2>*>(sqlite3_user_data(context)), context, 0, 0);
    }
  };

  namespace {
    void impl_create_function(lua_State* L) {
      database_handle_impl* self = check_database_handle_impl(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      luaL_checkany(L, 4);
      if (database_handle_access::create_function(self, name.data(), narg, L, 4) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, self->get());
      }
    }

    void impl_create_aggregate(lua_State* L) {
      database_handle_impl* self = check_database_handle_impl(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      luaL_checkany(L, 4);
      luaL_checkany(L, 5);
      if (database_handle_access::create_aggregate(self, name.data(), narg, L, 4, 5) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, self->get());
      }
    }

    void impl_delete_function(lua_State* L) {
      database_handle_impl* self = check_database_handle_impl(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      int narg = luaX_check_integer<int>(L, 3);
      if (database_handle_access::delete_function(self, name.data(), narg) == SQLITE_OK) {
        luaX_push_success(L);
      } else {
        push_error(L, self->get());
      }
    }
  }

  void initialize_dbh_function(lua_State* L) {
    luaX_set_field(L, -1, "create_function", impl_create_function);
    luaX_set_field(L, -1, "create_aggregate", impl_create_aggregate);
    luaX_set_field(L, -1, "delete_function", impl_delete_function);
  }
}
