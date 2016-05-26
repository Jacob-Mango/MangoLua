#pragma once

#ifndef LUAMANGO
#define LUAMANGO

#include <string>
#include <assert.h> 
#include <iostream>

#ifndef LUA_H
#define LUA_H
#include <lua.hpp>
#include "luawrapper.h"
#include "luawrapperutils.h"
#endif

#include "marshal.h"
#include "unmarshal.h"
#include "luafunction.h"
#include "luaconstructor.h"
#include "luacall.h"

#define LUAM_METHOD(S) WrapMethod<decltype(&S), &S>()
#define LUAM_CONSTRUCTOR(S, ...) WrapConstructor<S, __VA_ARGS__>

#define LUAM_CLASSREGISTER(L, S, T, MT, ...) luaW_register<S>(L, #S, T, MT, WrapConstructor<S, __VA_ARGS__>)

#define LUAM_FUNCTIONREGISTER(L, S)

#define LUAM_CALLFUNCTION(L, F, ...) CallFunction(L, F, __VA_ARGS__)

void Init(lua_State* L)
{
	luaopen_io(L);
	luaopen_base(L);
	luaopen_table(L);
	luaopen_string(L);
	luaopen_math(L);
	luaL_openlibs(L);
}

void LoadFile(lua_State* L, const char* file)
{
	luaL_dofile(L, file);
	int errors = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (errors != 0)
	{
		std::string s(lua_tostring(L, -1));
		std::cout << "Lua >> " << s << std::endl;
		lua_pop(L, 1);
	}
}

#define LUAM_INIT(L) Init(L)
#define LUAM_LOADFILE(L, F) LoadFile(L, F)

#endif