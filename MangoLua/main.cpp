#include "luaclass.h"
#include <iostream>

#include <string>

class B
{
public:
	B() 
	{

	}

	float GetN()
	{
		return 1.43f;
	}
};

class K
{
private:
	B m_B;
	std::string m_S;
public:
	K(B b, std::string s) : m_B(b), m_S(s)
	{

	}

	void PrintN()
	{
		std::cout << m_S << ": " << m_B.GetN() << std::endl;
	}
};

int main(int argc, char *argv[])
{
	lua_State* L = luaL_newstate();

	luaopen_io(L);
	luaopen_base(L);
	luaopen_table(L);
	luaopen_string(L);
	luaopen_math(L);

	luaL_openlibs(L);

	luaL_dofile(L, "test.lua");
	int errors = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (errors != 0)
	{
		std::cout << "Lua >> " << lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	luaL_Reg D[] =
	{
		{ NULL, NULL }
	};

	luaL_Reg B[] =
	{
		{ "GetN", SCRIPT_METHOD(B::GetN) },
		{ NULL, NULL }
	};

	luaL_Reg K[] =
	{
		{ "PrintN", SCRIPT_METHOD(K::PrintN) },
		{ NULL, NULL }
	};
	luaW_register<B>(L, "B", D, K, WrapConstructor<K>(L));
	luaW_register<K>(L, "K", D, K, WrapConstructor<K, B, std::string>(L));
	return 1;
}