// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8025)
// Generated by LuaStick, Jun 12 2020.

#pragma once

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8033)
#include "stdafx.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8033)
#include "..\..\LuaStick\files\Sticklib.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8033)
#include "StickTestDlg.h"

struct lua_State;
extern void luastick_init(lua_State* L);

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__X__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__X__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7042)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X::Enum2&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7042)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ::X::Enum2&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7042)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X::Enum1&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7042)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ::X::Enum1&& head, Args&& ... args)
{
	Sticklib::push_lvalue(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X::A&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__X__A__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X::A*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__X__A__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X::B&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__X__B__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::X::B*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__X__B__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::NM1::NM1_NM2&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__NM1__NM1_NM2__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::NM1::NM1_NM2*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__NM1__NM1_NM2__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::NM1::NM1_NM3&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__NM1__NM1_NM3__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::NM1::NM1_NM3*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__NM1__NM1_NM3__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::C&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__C__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::C*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__C__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::CStickTestDlg&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, &head, "lm__CStickTestDlg__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(6962)
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ::CStickTestDlg*&& head, Args&& ... args)
{
	Sticklib::push_classobject(L, false, head, "lm__CStickTestDlg__");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}


#include "..\..\LuaStick\files\Stickrun.h"
