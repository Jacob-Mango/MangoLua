#pragma once

#include <string>
#include <assert.h> 

#ifndef LUA_H
#define LUA_H
#include <lua.hpp>
#include "luawrapper.h"
#endif

#include "marshal.h"
#include "unmarshal.h"
#include "luafunction.h"
#include "luaconstructor.h"
#include "luacall.h"

#define SCRIPT_METHOD(S) WrapMethod<decltype(&S), &S>()