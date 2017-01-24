#include <lua.h>
#include <lauxlib.h>
#include <string.h>

struct profiler_log {
	int linedefined;
	char source[LUA_IDSIZE];
};

struct profiler_count {
	int total;
	int index;
};

static void
profiler_hook(lua_State *L, lua_Debug *ar) {
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, L) != LUA_TUSERDATA) {
		lua_pop(L, 1);
		return;
	}
	struct profiler_count * p = lua_touserdata(L, -1);
	lua_pop(L, 1);
	struct profiler_log * log = (struct profiler_log *)(p+1);
	int index = p->index++;
	while(index >= p->total) {
		index -= p->total;
	}
	if (lua_getinfo(L, "S", ar) != 0) {
		log[index].linedefined = ar->linedefined;
		strcpy(log[index].source, ar->short_src);
	} else {
		log[index].linedefined = 1;
		strcpy(log[index].source, "[unknown]");
	}
}

static int
lstart(lua_State *L) {
	lua_State *cL = L;
	int args = 0;
	if (lua_isthread(L, 1)) {
		cL = lua_tothread(L, 1);
		args = 1;
	}
	int count = luaL_optinteger(L, args+1, 1000);
	int interval = luaL_optinteger(L, args+2, 100);
	struct profiler_count * p = lua_newuserdata(L, sizeof(struct profiler_count) + count * sizeof(struct profiler_log));
	p->total = count;
	p->index = 0;
	lua_pushvalue(L, -1);
	lua_rawsetp(L, LUA_REGISTRYINDEX, cL);
	lua_sethook(cL, profiler_hook, LUA_MASKCOUNT, interval);
	
	return 0;
}

static int
lstop(lua_State *L) {
	lua_State *cL = L;
	if (lua_isthread(L, 1)) {
		cL = lua_tothread(L, 1);
	}
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, cL) != LUA_TNIL) {
		lua_pushnil(L);
		lua_rawsetp(L, LUA_REGISTRYINDEX, cL);
		lua_sethook(cL, NULL, 0, 0);
	} else {
		return luaL_error(L, "thread profiler not begin");
	}

	return 0;
}

static int
linfo(lua_State *L) {
	lua_State *cL = L;
	if (lua_isthread(L, 1)) {
		cL = lua_tothread(L, 1);
	}
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, cL) != LUA_TUSERDATA) {
		return luaL_error(L, "thread profiler not begin");
	}
	struct profiler_count * p = lua_touserdata(L, -1);
	struct profiler_log * log = (struct profiler_log *)(p+1);
	lua_newtable(L);
	int n = (p->index > p->total) ? p->total : p->index;
	int i;
	for (i=0;i<n;i++) {
		luaL_getsubtable(L, -1, log[i].source);
		lua_rawgeti(L, -1, log[i].linedefined);
		int c = lua_tointeger(L, -1);
		lua_pushinteger(L, c + 1);
		// subtbl, c, c + 1
		lua_rawseti(L, -3, log[i].linedefined);
		lua_pop(L, 2);
	}
	lua_pushinteger(L, p->index);
	return 2;
}

LUAMOD_API int
luaopen_profiler(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "start", lstart },
		{ "stop", lstop },
		{ "info", linfo },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
