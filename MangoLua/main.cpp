#include "mangolua.h"
#include <iostream>

#include <string>

class B
{
private:
	int m_x;
public:
	B() 
	{

	}

	void SetX(int x) {
		m_x = x;
	}

	int GetX()
	{
		return m_x;
	}
};

int main(int argc, char *argv[])
{
	lua_State* L = luaL_newstate();

	LUAM_INIT(L);
	LUAM_LOADFILE(L, "test.lua");

	luaL_Reg Dc[] =
	{
		{ NULL, NULL }
	};

	luaL_Reg Bc[] =
	{
		{ "SetX", LUAM_METHOD(B::SetX) },
		{ "GetX", LUAM_METHOD(B::GetX) },
		{ NULL, NULL }
	};

	LUAM_CLASSREGISTER(L, B, Dc, Bc);


	B* b = new B();
	b->SetX(8);
	LUAM_CALLFUNCTION(L, "ch", b);
	std::cout << "X: " << b->GetX() << std::endl;
	system("pause");
	return 0;
}