#include "firesteel/common.hpp"
#include "../include/lua_script.hpp"
#include "lua/lua.hpp"

namespace firesteel {

	lua_State* lua = nullptr;
	int local_i = 0;
	const char* local_str = "";

	void Lua::initialize() {
		//Initialize the Lua state.
		lua = luaL_newstate();
		//Load the standard Lua libraries.
		luaL_openlibs(lua);
		//Tell that Lua is loaded.
		LOG_INFO("Lua state created.");
	}

	bool Lua::line(const char* t_code) {
		if(luaL_dostring(lua, t_code) != 0) {
			LOG_WARN("Error executing Lua script: ", lua_tostring(lua, -1));
			return false;
		}
		return true;
	}

	bool Lua::run(const char* t_path) {
		if(luaL_dofile(lua, t_path) != 0) {
			LOG_WARN("Error executing Lua script: ", lua_tostring(lua, -1));
			return false;
		}
		return true;
	}

	void Lua::destroy() {
		LOG_INFO("Lua state destroyed.");
		// Close the Lua state
		lua_close(lua);
	}

	int Lua::get_i(const char* t_var_name) {
		lua_getglobal(lua, t_var_name);
		local_i = (int)lua_tointeger(lua, -1);
		lua_pop(lua, 1);
		return local_i;
	}
	const char* Lua::get_s(const char* t_var_name) {
		lua_getglobal(lua, t_var_name);
		local_str = (const char*)lua_tostring(lua, -1);
		lua_pop(lua, 1);
		return local_str;
	}

}