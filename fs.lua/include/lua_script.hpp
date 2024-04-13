#ifndef FS_LUA_H
#define FS_LUA_H

namespace firesteel {
	class Lua {
	public:
		static void initialize();
		static bool line(const char* t_code);
		static bool run(const char* t_path);
		static void destroy();

		static int get_i(const char* t_var_name);
		static const char* get_s(const char* t_var_name);
	};
}

#endif // FS_LUA_H