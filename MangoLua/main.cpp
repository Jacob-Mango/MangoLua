#include "mangolua.h"
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
		std::string s(lua_tostring(L, -1));
		std::cout << "Lua >> " << s << "\n";
		lua_pop(L, 1);
	}

	luaL_Reg Dc[] =
	{
		{ NULL, NULL }
	};

	luaL_Reg Hc[] =
	{
		{ NULL, NULL }
	};

	luaL_Reg Bc[] =
	{
		{ "GetN", SCRIPT_METHOD(B::GetN) },
		{ NULL, NULL }
	};

	luaL_Reg Kc[] =
	{
		{ "PrintN", SCRIPT_METHOD(K::PrintN) },
		{ NULL, NULL }
	};

	luaW_register<B>(L, "B", Hc, Bc, WrapConstructor<B>);
	luaW_register<K>(L, "K", Dc, Kc, WrapConstructor<K, B, std::string>);

	B* b = new B();

	CallFunction(L, "ch", b, "Hey BB");

	system("pause");
	return 0;
}