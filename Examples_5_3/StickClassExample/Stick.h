// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8098)
// Generated by LuaStick, Aug 17 2020.

#pragma once

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8106)
#include "stdafx.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8106)
#include "..\..\LuaStick\files\Sticklib.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8106)
#include "TestClass.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8106)
#include "StickClassExample.h"

struct lua_State;
extern void luastick_init(lua_State* L);

template<>
void Sticklib::push_lvalue<::TestStruct0>(lua_State * L, ::TestStruct0 const & value);

/// <summary>
/// Push the struct data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7072)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestStruct0&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the struct data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7072)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ::TestStruct0&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass0&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__TestClass0__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass0*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__TestClass0__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7110)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass0::EnumB&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7110)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ::TestClass0::EnumB&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7110)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass0::EnumA&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7110)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ::TestClass0::EnumA&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<>
void Sticklib::push_lvalue<::TestStruct1>(lua_State * L, ::TestStruct1 const & value);

/// <summary>
/// Push the struct data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7072)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestStruct1&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the struct data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7072)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ::TestStruct1&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass1&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__TestClass1__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass1*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__TestClass1__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass2&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__TestClass2__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::TestClass2*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__TestClass2__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::CStickClassExampleApp&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__CStickClassExampleApp__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7030)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::CStickClassExampleApp*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__CStickClassExampleApp__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}


#include "..\..\LuaStick\files\Stickrun.h"
