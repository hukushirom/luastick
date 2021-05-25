// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8401)
// Generated by LuaStick, May 25 2021.

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8409)
#include "..\..\LuaStick\files\Sticklib.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8409)
#include "main.h"
// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8417)
#include "Stick.h"

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4095)
/// <summary>
/// ::myspace::Add(std::string& v1, std::string& v2)
/// </summary>
static int lm__myspace__Add__2(lua_State* L)
{
	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4127)
	try
	{
		// Check the count of arguments.
		if (lua_gettop(L) != 2)
			throw std::invalid_argument("Count of arguments is not correct.");
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		std::string v1;
		Sticklib::check_lvalue<std::string>(v1, L, 1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		std::string v2;
		Sticklib::check_lvalue<std::string>(v2, L, 2);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4431)
		// Call the c++ function.
		auto __lstickvar_ret = (std::string)::myspace::Add((const std::string&)v1, (std::string const&)v2);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4613)
		Sticklib::push_lvalue<std::string>(L, __lstickvar_ret);
	}
	catch (std::exception & e)
	{
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4695)
		luaL_error(L, (std::string("C function error:::myspace::Add:") + e.what()).c_str());
	}
	catch (...)
	{
		luaL_error(L, "C function error:::myspace::Add");
	}
	return 1;
}

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4095)
/// <summary>
/// ::myspace::ArrayAdd(std::vector<std::wstring>& v1, std::wstring& v2)
/// </summary>
static int lm__myspace__ArrayAdd__2(lua_State* L)
{
	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4127)
	try
	{
		// Check the count of arguments.
		if (lua_gettop(L) != 2)
			throw std::invalid_argument("Count of arguments is not correct.");
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		std::vector<std::string> _argin_1_1;
		Sticklib::check_array<std::string>(_argin_1_1, L, 1);
		std::vector<std::wstring> v1;
		Sticklib::vector_to_vector<std::wstring,std::string>(v1, _argin_1_1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		std::string _argin_2_1;
		Sticklib::check_lvalue<std::string>(_argin_2_1, L, 2);
		std::wstring v2;
		Sticklib::astring_to_wstring(v2, _argin_2_1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4417)
		// Call the c++ function.
		::myspace::ArrayAdd((std::vector<std::wstring>&)v1, (const std::wstring&)v2);
		std::vector<std::string> _argout_1_1;
		Sticklib::vector_to_vector<std::string,std::wstring>(_argout_1_1, v1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4613)
		Sticklib::push_array<std::string>(L, _argout_1_1);
	}
	catch (std::exception & e)
	{
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4695)
		luaL_error(L, (std::string("C function error:::myspace::ArrayAdd:") + e.what()).c_str());
	}
	catch (...)
	{
		luaL_error(L, "C function error:::myspace::ArrayAdd");
	}
	return 1;
}

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4095)
/// <summary>
/// ::myspace::HashAdd(std::map<std::wstring,int>& v1, int v2)
/// </summary>
static int lm__myspace__HashAdd__2(lua_State* L)
{
	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4127)
	try
	{
		// Check the count of arguments.
		if (lua_gettop(L) != 2)
			throw std::invalid_argument("Count of arguments is not correct.");
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		std::unordered_map<std::string,__int64> _argin_1_1;
		Sticklib::check_hash(_argin_1_1, L, 1);
		std::map<std::string,__int64> _argin_1_2;
		myconv::uomap_to_map(_argin_1_2, _argin_1_1);
		std::map<std::wstring,int> v1;
		myconv::map_to_map<std::wstring,int,std::string,__int64>(v1, _argin_1_2);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		__int64 _argin_2_1;
		Sticklib::check_lvalue<__int64>(_argin_2_1, L, 2);
		int v2;
		Sticklib::T_to_U(v2, _argin_2_1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4417)
		// Call the c++ function.
		::myspace::HashAdd((std::map<std::wstring,int>&)v1, (int)v2);
		std::map<std::string,__int64> _argout_1_1;
		myconv::map_to_map<std::string,__int64,std::wstring,int>(_argout_1_1, v1);
		std::unordered_map<std::string,__int64> _argout_1_2;
		myconv::map_to_uomap(_argout_1_2, _argout_1_1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4613)
		Sticklib::push_hash<std::string,__int64>(L, _argout_1_2);
	}
	catch (std::exception & e)
	{
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4695)
		luaL_error(L, (std::string("C function error:::myspace::HashAdd:") + e.what()).c_str());
	}
	catch (...)
	{
		luaL_error(L, "C function error:::myspace::HashAdd");
	}
	return 1;
}

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4095)
/// <summary>
/// ::myspace::Print(char* message)
/// </summary>
static int lm__myspace__Print__1(lua_State* L)
{
	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4127)
	try
	{
		// Check the count of arguments.
		if (lua_gettop(L) != 1)
			throw std::invalid_argument("Count of arguments is not correct.");
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4293)
		std::string _argin_1_1;
		Sticklib::check_lvalue<std::string>(_argin_1_1, L, 1);
		char* message;
		Sticklib::astring_to_atext(message, _argin_1_1);
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4417)
		// Call the c++ function.
		::myspace::Print((char const*)message);
	}
	catch (std::exception & e)
	{
		// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4695)
		luaL_error(L, (std::string("C function error:::myspace::Print:") + e.what()).c_str());
	}
	catch (...)
	{
		luaL_error(L, "C function error:::myspace::Print");
	}
	return 0;
}

// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(8538)
/// <summary>
/// LuaStick initializing function.
/// luastick_init must be called to register the classes and its member functions.
/// </summary>
void luastick_init(lua_State* L)
{
	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7437)
	// Register the global functions.
	Sticklib::push_table(L, "");
	{
		static struct luaL_Reg lm__Static[] =
		{
			// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7467)
			{ nullptr, nullptr },
		};
		Sticklib::set_functions(L, lm__Static);
	}

	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7437)
	// Register the static class '::myspace' and its static member functions.
	Sticklib::push_table(L, "myspace");
	{
		static struct luaL_Reg lm__myspace__Static[] =
		{
			// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7456)
			{ "Add", lm__myspace__Add__2 },
			// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7456)
			{ "ArrayAdd", lm__myspace__ArrayAdd__2 },
			// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7456)
			{ "HashAdd", lm__myspace__HashAdd__2 },
			// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7456)
			{ "Print", lm__myspace__Print__1 },
			// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7467)
			{ nullptr, nullptr },
		};
		Sticklib::set_functions(L, lm__myspace__Static);
	}

	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7742)
	Sticklib::pop(L);

	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(7742)
	Sticklib::pop(L);

}

