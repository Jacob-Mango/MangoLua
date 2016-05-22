#pragma once

#ifndef LUA_H
#define LUA_H
#include <lua.hpp>
#endif

#include "luawrapper.h"

#include <string>
#include <assert.h> 

template<typename R>
struct Marshal
{
	static int Dispatch(lua_State* L, const R& value) { assert(false); return 0; }
};

template<>
struct Marshal<double>
{
	static int Dispatch(lua_State* L, const double& value)
	{
		lua_pushnumber(L, value);
		return 1;
	}
};

class UserdataPtr {

	explicit UserdataPtr(void* const p)
	{
		assert(p != 0);
	}

};

template<class T>
struct Marshal<T>
{
	static int Dispatch(lua_State* L, T& value)
	{
		new (lua_newuserdata(L, sizeof(UserdataPtr))) UserdataPtr(p);
		lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo <T>::getConstKey());
		assert(lua_istable(L, -1));
		lua_setmetatable(L, -2);
		return 1;
	}
};

template<typename R>
struct Unmarshal
{
	static R&& Dispatch(lua_State* L, int index) { assert(false); return 0; }
};

template<>
struct Unmarshal<double>
{
	static double&& Dispatch(lua_State* L, int index)
	{
		return lua_tonumber(L, index);
	}
};

template<class T>
struct Unmarshal<T>
{
	static T&& Dispatch(lua_State* L, int index)
	{
		return luaW_to<T>(L, index);
	}
};

template<>
struct Unmarshal<std::string>
{
	static std::string&& Dispatch(lua_State* L, int index)
	{
		return lua_tostring(L, index);
	}
};

template<typename T>
struct FunctionWrapper
{

};

template<typename T, typename R, typename... Args>
struct FunctionWrapper<R(T::*)(Args...)>
{
	enum { ArgCount = sizeof...(Args) };

	typedef R(T::* FunctionType)(Args...);

	template<FunctionType method, size_t... index>
	static lua_CFunction WrapMethod()
	{
		return [](lua_State *L)
		{
			T* obj = luaW_check<T>(L, 1);
			Marshal<std::remove_const<R>::type>::Dispatch(L, (obj->*method)(Unmarshal<Args>::Dispatch(L, index + 2)...));
			return 1;
		};
	}

	template<FunctionType method, size_t... index>
	static lua_CFunction WrapMethod(std::index_sequence<index...>)
	{
		return WrapMethod<method, index...>();
	}
};

template<typename T, typename R, typename... Args>
struct FunctionWrapper<R(T::*)(Args...) const>
{
	enum { ArgCount = sizeof...(Args) };

	typedef R(T::* FunctionType)(Args...) const;
	typedef R(T::* FunctionTypeNonConst)(Args...);
	template <FunctionType method, size_t... index> static lua_CFunction WrapMethod()
	{
		return FunctionWrapper<FunctionTypeNonConst>::WrapMethod<(FunctionTypeNonConst)method, index...>();
	}
	template <FunctionType method, size_t... index> static lua_CFunction WrapMethodInternal(std::index_sequence<index...>)
	{
		return FunctionWrapper<FunctionTypeNonConst>::WrapMethod<(FunctionTypeNonConst)method, index...>();
	}
};

template<typename T, typename... Args>
struct FunctionWrapper<void(T::*)(Args...)>
{
	enum { ArgCount = sizeof...(Args) };
	typedef void (T::* FunctionType)(Args...);

	template<FunctionType method, size_t... index>
	static lua_CFunction WrapMethod()
	{
		return [](lua_State *L)
		{
			T* obj = luaW_check<T>(L, 1);
			(obj->*method)(Unmarshal<Args>::Dispatch(L, index + 2)...);
			return 0;
		};
	}

	template<FunctionType method, size_t... index>
	static lua_CFunction WrapMethodInternal(std::index_sequence<index...>)
	{
		return WrapMethod<method, index...>();
	}
};

template<typename T, T method, std::size_t N, typename Indices = std::make_index_sequence<N>>
lua_CFunction WrapMethod()
{
	return FunctionWrapper<T>::WrapMethodInternal<method>(Indices());
}

template<typename T, T method>
lua_CFunction WrapMethod()
{
	return WrapMethod<T, method, FunctionWrapper<T>::ArgCount>();
}

template<class T>
T WrapArgument(lua_State* L, int& i)
{
	return Unmarshal<T>().Dispatch(L, i++);
}

template<typename A>
A WrapArgument(lua_State* L, int& i)
{
	return Unmarshal<A>().Dispatch(L, i++);
}

template<class T, typename... Args>
T* WrapConstructor(lua_State* L)
{
	int& i = 1;
	return new T(WrapArgument<std::forward<Args>...>(L, i));
}

#define SCRIPT_METHOD(S) WrapMethod<decltype(&S), &S>()
#define SCRIPT_CONSTRUCTOR(L, T, ...) WrapConstructor<T, __VA_ARGS__>(L)