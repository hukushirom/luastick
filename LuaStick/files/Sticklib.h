#pragma once

#pragma warning(disable:4635)

#include <memory>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <string>
// 22.09.06 Fukushiro M. 1行削除 (追加)
//#include <codecvt>
// 22.09.06 Fukushiro M. 1行変更 ()
//#include <exception>
#include <stdexcept>

#if defined(WIN32)
#include <wtypes.h>	// For BOOL.
#endif//WIN32

#if defined(_CPLUS_LUA_)
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#else
#include "lua.hpp"
#endif
#include "lauxlib.h"
#include "lualib.h"


enum SticklibError : int
{
	STICKERR_SYSTEM = 1000,
	STICKERR_INCORRECT_ARG_TYPE = 1001,
};

constexpr const char* STICKERR_PREFIX = "stick_error:";
constexpr size_t STICKERR_PREFIX_LENGTH = sizeof("stick_error:") - 1;

constexpr const char* STICKERR_SYSTEM_MSG = "System error";
constexpr const char* STICKERR_INCORRECT_ARG_TYPE_MSG = "Incorrect argument type";

#define StickThrowScriptError(msg) throw std::runtime_error(msg)
#define StickThrowRuntimeError(id, ...) throw std::runtime_error(Sticklib::make_error_message(id, id ## _MSG, __VA_ARGS__))
#define StickThrowSystemError(...) throw std::runtime_error(Sticklib::make_error_message(STICKERR_SYSTEM, STICKERR_SYSTEM_MSG, (const char*)__FILE__, __LINE__, __VA_ARGS__))
#define StickSystemErrorMessage(...) Sticklib::make_error_message(STICKERR_SYSTEM, STICKERR_SYSTEM_MSG, (const char*)__FILE__, __LINE__, __VA_ARGS__)

/// <summary>
/// <para> Wrapper for class object.                     </para>
/// <para> Wrap the c++ class object and export to lua.  </para>
/// <para> See Readme.txt                              </para>
/// </summary>
struct StickInstanceWrapper
{
	unsigned __int64 hash;	// Hash value to validate this object.
	bool own;		// true:own class object/false:not own.
	void * ptr;		// class object.
};

// Lua-type definition.

//----- 21.05.25 Fukushiro M. 削除始 ()-----
///// sticktype name="classobject" ctype="Sticklib::classobject" getfunc="Sticklib::check_classobject" setfunc="Sticklib::push_classobject" />
///// sticktype name="array<classobject>" ctype="std::vector<Sticklib::classobject>" getfunc="Sticklib::check_classobjectarray" setfunc="Sticklib::push_classobjectarray" />
//----- 21.05.25 Fukushiro M. 削除終 ()-----

/// <sticktype name="boolean" ctype="bool" getfunc="Sticklib::check_lvalue<bool>" setfunc="Sticklib::push_lvalue<bool>" />
/// <sticktype name="integer" ctype="__int64" getfunc="Sticklib::check_lvalue<__int64>" setfunc="Sticklib::push_lvalue<__int64>" />
/// <sticktype name="number" ctype="double" getfunc="Sticklib::check_lvalue<double>" setfunc="Sticklib::push_lvalue<double>" />
/// <sticktype name="string" ctype="std::string" getfunc="Sticklib::check_lvalue<std::string>" setfunc="Sticklib::push_lvalue<std::string>" />
/// <sticktype name="lightuserdata" ctype="void*" getfunc="Sticklib::check_lvalue<void*>" setfunc="Sticklib::push_lvalue<void*>" />
/// <sticktype name="array<number>" ctype="std::vector<double>" getfunc="Sticklib::check_array<double>" setfunc="Sticklib::push_array<double>" />
/// <sticktype name="array<integer>" ctype="std::vector<__int64>" getfunc="Sticklib::check_array<__int64>" setfunc="Sticklib::push_array<__int64>" />
/// <sticktype name="array<boolean>" ctype="std::vector<bool>" getfunc="Sticklib::check_array<bool>" setfunc="Sticklib::push_array<bool>" />
/// <sticktype name="array<string>" ctype="std::vector<std::string>" getfunc="Sticklib::check_array<std::string>" setfunc="Sticklib::push_array<std::string>" />
/// <sticktype name="array<lightuserdata>" ctype="std::vector<void*>" getfunc="Sticklib::check_array<void*>" setfunc="Sticklib::push_array<void*>" />
/// <sticktype name="hash<integer,boolean>" ctype="std::unordered_map<__int64,bool>" getfunc="Sticklib::check_hash<__int64,bool>" setfunc="Sticklib::push_hash<__int64,bool>" />
/// <sticktype name="hash<integer,integer>" ctype="std::unordered_map<__int64,__int64>" getfunc="Sticklib::check_hash<__int64,__int64>" setfunc="Sticklib::push_hash<__int64,__int64>" />
/// <sticktype name="hash<integer,number>" ctype="std::unordered_map<__int64,double>" getfunc="Sticklib::check_hash<__int64,double>" setfunc="Sticklib::push_hash<__int64,double>" />
/// <sticktype name="hash<integer,string>" ctype="std::unordered_map<__int64,std::string>" getfunc="Sticklib::check_hash<__int64,std::string>" setfunc="Sticklib::push_hash<__int64,std::string>" />
/// <sticktype name="hash<number,boolean>" ctype="std::unordered_map<double,bool>" getfunc="Sticklib::check_hash<double,bool>" setfunc="Sticklib::push_hash<double,bool>" />
/// <sticktype name="hash<number,integer>" ctype="std::unordered_map<double,__int64>" getfunc="Sticklib::check_hash<double,__int64>" setfunc="Sticklib::push_hash<double,__int64>" />
/// <sticktype name="hash<number,number>" ctype="std::unordered_map<double,double>" getfunc="Sticklib::check_hash<double,double>" setfunc="Sticklib::push_hash<double,double>" />
/// <sticktype name="hash<number,string>" ctype="std::unordered_map<double,std::string>" getfunc="Sticklib::check_hash<double,std::string>" setfunc="Sticklib::push_hash<double,std::string>" />
/// <sticktype name="hash<string,boolean>" ctype="std::unordered_map<std::string,bool>" getfunc="Sticklib::check_hash<std::string,bool>" setfunc="Sticklib::push_hash<std::string,bool>" />
/// <sticktype name="hash<string,integer>" ctype="std::unordered_map<std::string,__int64>" getfunc="Sticklib::check_hash<std::string,__int64>" setfunc="Sticklib::push_hash<std::string,__int64>" />
/// <sticktype name="hash<string,number>" ctype="std::unordered_map<std::string,double>" getfunc="Sticklib::check_hash<std::string,double>" setfunc="Sticklib::push_hash<std::string,double>" />
/// <sticktype name="hash<string,string>" ctype="std::unordered_map<std::string,std::string>" getfunc="Sticklib::check_hash<std::string,std::string>" setfunc="Sticklib::push_hash<std::string,std::string>" />
/// <sticktype name="hash<string,any>" ctype="std::unordered_map<std::string,Sticklib::AnyValue>" getfunc="Sticklib::check_hash<std::string,Sticklib::AnyValue>" setfunc="Sticklib::push_hash<std::string,Sticklib::AnyValue>" />

// Converter definition.

/// <stickconv type1="__int8" type2="__int64" type1to2="Sticklib::T_to_U<__int64,__int8>" type2to1="Sticklib::T_to_U<__int8,__int64>" />
/// <stickconv type1="__int16" type2="__int64" type1to2="Sticklib::T_to_U<__int64,__int16>" type2to1="Sticklib::T_to_U<__int16,__int64>" />
/// <stickconv type1="__int32" type2="__int64" type1to2="Sticklib::T_to_U<__int64,__int32>" type2to1="Sticklib::T_to_U<__int32,__int64>" />
/// <stickconv type1="int" type2="__int64" type1to2="Sticklib::T_to_U<__int64,int>" type2to1="Sticklib::T_to_U<int,__int64>" />
/// <stickconv type1="long" type2="__int64" type1to2="Sticklib::T_to_U<__int64,long>" type2to1="Sticklib::T_to_U<long,__int64>" />
/// <stickconv type1="short" type2="__int64" type1to2="Sticklib::T_to_U<__int64,short>" type2to1="Sticklib::T_to_U<short,__int64>" />
/// <stickconv type1="unsigned __int8" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned __int8>" type2to1="Sticklib::T_to_U<unsigned __int8,__int64>" />
/// <stickconv type1="unsigned __int16" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned __int16>" type2to1="Sticklib::T_to_U<unsigned __int16,__int64>" />
/// <stickconv type1="unsigned __int32" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned __int32>" type2to1="Sticklib::T_to_U<unsigned __int32,__int64>" />
/// <stickconv type1="unsigned __int64" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned __int64>" type2to1="Sticklib::T_to_U<unsigned __int64,__int64>" />
/// <stickconv type1="unsigned int" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned int>" type2to1="Sticklib::T_to_U<unsigned int,__int64>" />
/// <stickconv type1="unsigned long" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned long>" type2to1="Sticklib::T_to_U<unsigned long,__int64>" />
/// <stickconv type1="unsigned short" type2="__int64" type1to2="Sticklib::T_to_U<__int64,unsigned short>" type2to1="Sticklib::T_to_U<unsigned short,__int64>" />
/// <stickconv type1="size_t" type2="__int64" type1to2="Sticklib::T_to_U<__int64,size_t>" type2to1="Sticklib::T_to_U<size_t,__int64>" />
/// <stickconv type1="float" type2="double" type1to2="Sticklib::T_to_U<double,float>" type2to1="Sticklib::T_to_U<float,double>" />
/// <stickconv type1="BOOL" type2="bool" type1to2="Sticklib::T_to_U<bool, BOOL>" type2to1="Sticklib::T_to_U<BOOL, bool>" />
/// <stickconv type1="std::wstring" type2="std::string" type1to2="Sticklib::Sticklib::T_to_U<std::string, std::wstring>" type2to1="Sticklib::T_to_U<std::wstring, std::string>" />
/// <stickconv type1="char*" type2="std::string" type1to2="Sticklib::T_to_U<std::string, char *>" type2to1="Sticklib::T_to_U<char *, std::string>" />

/// <stickconv type1="wchar_t*" type2="std::wstring" type1to2="Sticklib::T_to_U<std::wstring,wchar_t*>" type2to1="Sticklib::T_to_U<wchar_t*,std::wstring>" />

/// <stickconv type1="std::vector<__int8>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,__int8>" type2to1="Sticklib::vectorT_to_vectorU<__int8,__int64>" />
/// <stickconv type1="std::vector<__int16>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,__int16>" type2to1="Sticklib::vectorT_to_vectorU<__int16,__int64>" />
/// <stickconv type1="std::vector<__int32>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,__int32>" type2to1="Sticklib::vectorT_to_vectorU<__int32,__int64>" />
/// <stickconv type1="std::vector<int>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,int>" type2to1="Sticklib::vectorT_to_vectorU<int,__int64>" />
/// <stickconv type1="std::vector<long>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,long>" type2to1="Sticklib::vectorT_to_vectorU<long,__int64>" />
/// <stickconv type1="std::vector<short>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,short>" type2to1="Sticklib::vectorT_to_vectorU<short,__int64>" />
/// <stickconv type1="std::vector<unsigned __int8>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned __int8>" type2to1="Sticklib::vectorT_to_vectorU<unsigned __int8,__int64>" />
/// <stickconv type1="std::vector<unsigned __int16>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned __int16>" type2to1="Sticklib::vectorT_to_vectorU<unsigned __int16,__int64>" />
/// <stickconv type1="std::vector<unsigned __int32>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned __int32>" type2to1="Sticklib::vectorT_to_vectorU<unsigned __int32,__int64>" />
/// <stickconv type1="std::vector<unsigned __int64>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned __int64>" type2to1="Sticklib::vectorT_to_vectorU<unsigned __int64,__int64>" />
/// <stickconv type1="std::vector<unsigned int>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned int>" type2to1="Sticklib::vectorT_to_vectorU<unsigned int,__int64>" />
/// <stickconv type1="std::vector<unsigned long>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned long>" type2to1="Sticklib::vectorT_to_vectorU<unsigned long,__int64>" />
/// <stickconv type1="std::vector<unsigned short>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,unsigned short>" type2to1="Sticklib::vectorT_to_vectorU<unsigned short,__int64>" />
/// <stickconv type1="std::vector<size_t>" type2="std::vector<__int64>" type1to2="Sticklib::vectorT_to_vectorU<__int64,size_t>" type2to1="Sticklib::vectorT_to_vectorU<size_t,__int64>" />
/// <stickconv type1="std::vector<float>" type2="std::vector<double>" type1to2="Sticklib::vectorT_to_vectorU<double,float>" type2to1="Sticklib::vectorT_to_vectorU<float,double>" />
/// <stickconv type1="std::vector<BOOL>" type2="std::vector<bool>" type1to2="Sticklib::vectorT_to_vectorU<bool,BOOL>" type2to1="Sticklib::vectorT_to_vectorU<BOOL,bool>" />
/// <stickconv type1="std::vector<std::wstring>" type2="std::vector<std::string>" type1to2="Sticklib::vectorT_to_vectorU<std::string,std::wstring>" type2to1="Sticklib::vectorT_to_vectorU<std::wstring,std::string>" />

/// <stickconv type1="std::unordered_set<__int64>" type2="std::vector<__int64>" type1to2="Sticklib::uset_to_vector<__int64>" type2to1="Sticklib::vector_to_uset<__int64>" />
/// <stickconv type1="std::unordered_set<double>" type2="std::vector<double>" type1to2="Sticklib::uset_to_vector<double>" type2to1="Sticklib::vector_to_uset<double>" />
/// <stickconv type1="std::unordered_set<std::string>" type2="std::vector<std::string>" type1to2="Sticklib::uset_to_vector<std::string>" type2to1="Sticklib::vector_to_uset<std::string>" />
/// <stickconv type1="std::unordered_set<void*>" type2="std::vector<void*>" type1to2="Sticklib::uset_to_vector<void*>" type2to1="Sticklib::vector_to_uset<void*>" />

/// <stickconv type1="std::unordered_set<__int8>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, __int8>" type2to1="Sticklib::vectorT_to_usetU<__int8, __int64>" />
/// <stickconv type1="std::unordered_set<__int16>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, __int16>" type2to1="Sticklib::vectorT_to_usetU<__int16, __int64>" />
/// <stickconv type1="std::unordered_set<__int32>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, __int32>" type2to1="Sticklib::vectorT_to_usetU<__int32, __int64>" />
/// <stickconv type1="std::unordered_set<int>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, int>" type2to1="Sticklib::vectorT_to_usetU<int, __int64>" />
/// <stickconv type1="std::unordered_set<long>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, long>" type2to1="Sticklib::vectorT_to_usetU<long, __int64>" />
/// <stickconv type1="std::unordered_set<short>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, short>" type2to1="Sticklib::vectorT_to_usetU<short, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned __int8>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned __int8>" type2to1="Sticklib::vectorT_to_usetU<unsigned __int8, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned __int16>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned __int16>" type2to1="Sticklib::vectorT_to_usetU<unsigned __int16, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned __int32>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned __int32>" type2to1="Sticklib::vectorT_to_usetU<unsigned __int32, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned __int64>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned __int64>" type2to1="Sticklib::vectorT_to_usetU<unsigned __int64, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned int>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned int>" type2to1="Sticklib::vectorT_to_usetU<unsigned int, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned long>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned long>" type2to1="Sticklib::vectorT_to_usetU<unsigned long, __int64>" />
/// <stickconv type1="std::unordered_set<unsigned short>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, unsigned short>" type2to1="Sticklib::vectorT_to_usetU<unsigned short, __int64>" />
/// <stickconv type1="std::unordered_set<size_t>" type2="std::vector<__int64>" type1to2="Sticklib::usetT_to_vectorU<__int64, size_t>" type2to1="Sticklib::vectorT_to_usetU<size_t, __int64>" />
/// <stickconv type1="std::unordered_set<float>" type2="std::vector<double>" type1to2="Sticklib::usetT_to_vectorU<double, float>" type2to1="Sticklib::vectorT_to_usetU<float, double>" />
/// <stickconv type1="std::unordered_set<std::wstring>" type2="std::vector<std::string>" type1to2="Sticklib::usetT_to_vectorU<std::string, std::wstring>" type2to1="Sticklib::vectorT_to_usetU<std::wstring, std::string>" />

/// <stickconv type1="std::set<__int64>" type2="std::vector<__int64>" type1to2="Sticklib::set_to_vector<__int64>" type2to1="Sticklib::vector_to_set<__int64>" />
/// <stickconv type1="std::set<double>" type2="std::vector<double>" type1to2="Sticklib::set_to_vector<double>" type2to1="Sticklib::vector_to_set<double>" />
/// <stickconv type1="std::set<std::string>" type2="std::vector<std::string>" type1to2="Sticklib::set_to_vector<std::string>" type2to1="Sticklib::vector_to_set<std::string>" />
/// <stickconv type1="std::set<void*>" type2="std::vector<void*>" type1to2="Sticklib::set_to_vector<void*>" type2to1="Sticklib::vector_to_set<void*>" />

/// <stickconv type1="std::set<__int8>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, __int8>" type2to1="Sticklib::vectorT_to_setU<__int8, __int64>" />
/// <stickconv type1="std::set<__int16>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, __int16>" type2to1="Sticklib::vectorT_to_setU<__int16, __int64>" />
/// <stickconv type1="std::set<__int32>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, __int32>" type2to1="Sticklib::vectorT_to_setU<__int32, __int64>" />
/// <stickconv type1="std::set<int>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, int>" type2to1="Sticklib::vectorT_to_setU<int, __int64>" />
/// <stickconv type1="std::set<long>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, long>" type2to1="Sticklib::vectorT_to_setU<long, __int64>" />
/// <stickconv type1="std::set<short>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, short>" type2to1="Sticklib::vectorT_to_setU<short, __int64>" />
/// <stickconv type1="std::set<unsigned __int8>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned __int8>" type2to1="Sticklib::vectorT_to_setU<unsigned __int8, __int64>" />
/// <stickconv type1="std::set<unsigned __int16>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned __int16>" type2to1="Sticklib::vectorT_to_setU<unsigned __int16, __int64>" />
/// <stickconv type1="std::set<unsigned __int32>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned __int32>" type2to1="Sticklib::vectorT_to_setU<unsigned __int32, __int64>" />
/// <stickconv type1="std::set<unsigned __int64>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned __int64>" type2to1="Sticklib::vectorT_to_setU<unsigned __int64, __int64>" />
/// <stickconv type1="std::set<unsigned int>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned int>" type2to1="Sticklib::vectorT_to_setU<unsigned int, __int64>" />
/// <stickconv type1="std::set<unsigned long>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned long>" type2to1="Sticklib::vectorT_to_setU<unsigned long, __int64>" />
/// <stickconv type1="std::set<unsigned short>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, unsigned short>" type2to1="Sticklib::vectorT_to_setU<unsigned short, __int64>" />
/// <stickconv type1="std::set<size_t>" type2="std::vector<__int64>" type1to2="Sticklib::setT_to_vectorU<__int64, size_t>" type2to1="Sticklib::vectorT_to_setU<size_t, __int64>" />
/// <stickconv type1="std::set<float>" type2="std::vector<double>" type1to2="Sticklib::setT_to_vectorU<double, float>" type2to1="Sticklib::vectorT_to_setU<float, double>" />
/// <stickconv type1="std::set<std::wstring>" type2="std::vector<std::string>" type1to2="Sticklib::setT_to_vectorU<std::string, std::wstring>" type2to1="Sticklib::vectorT_to_setU<std::wstring, std::string>" />


//
//   c++           get from lua             c++               c++
// +--------   *B +-------------+ *A   +------------+ *D   +-------+
// |char*  |<-----|   string    |----->|std::wstring|----->|wchar* |
// +-------+      |(std::string)|      +------------+      +-------+
//                |             |
//                +-------------+
//
//
//   c++           get from lua             c++               c++
// +--------   *B +-------------+ *A   +------------+   *D +-------+
// |char*  |----->|   string    |<-----|std::wstring|<-----|wchar* |
// +-------+      |(std::string)|      +------------+      +-------+
//                |             |
//                +-------------+
//

class Sticklib
{
public:
// 21.05.26 Fukushiro M. 1行削除 ()
//	using classobject = void *;

	using charConstP = char const *;
	using charP = char *;
	using voidp = void *;

public:
	class AnyValue
	{
	public:
		enum class Type
		{
			NIL,
			DOUBLE,
			BOOL,
			CHARP,
		};
	public:
		Type type;
		union
		{
			double doubleValue;
			bool boolValue;
			char * charpValue;
		};
		AnyValue () : type(Type::NIL) {}
		AnyValue (const double & value) : type(Type::DOUBLE), doubleValue(value) {}
		AnyValue (const bool & value) : type(Type::BOOL), boolValue(value) {}
		AnyValue (const char * value)
		{
			type = Type::CHARP;
			auto sz = strlen(value) + 1;
			charpValue = new char[sz];
			strcpy_s(charpValue, sz, value);
		}
		AnyValue (const __int16 & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const unsigned __int16 & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const __int32 & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const unsigned __int32 & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const __int64 & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const unsigned __int64 & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const float & value) : type(Type::DOUBLE), doubleValue((double)value) {}
		AnyValue (const wchar_t * value)
		{
			type = Type::CHARP;
			std::string astr;
			T_to_U<std::string, std::wstring>(astr, value);
			charpValue = new char[astr.length() + 1];
			strcpy_s(charpValue, astr.length() + 1, astr.c_str());
		}

		~AnyValue ()
		{
			if (type == Type::CHARP)
				delete[] charpValue;
		}

		AnyValue (const AnyValue & value)
		{
			type = Type::NIL;
			*this = value;
		}

		AnyValue (AnyValue && value)
		{
			type = value.type;
			switch (value.type)
			{
			case AnyValue::Type::NIL:
				break;
			case AnyValue::Type::DOUBLE:
				doubleValue = value.doubleValue;
				break;
			case AnyValue::Type::BOOL:
				boolValue = value.boolValue;
				break;
			case AnyValue::Type::CHARP:
				charpValue = value.charpValue;
				break;
			}
			value.type = Type::NIL;
		}

		AnyValue & operator = (const AnyValue & value)
		{
			if (type == AnyValue::Type::CHARP)
				delete[] charpValue;
			type = value.type;
			switch (value.type)
			{
			case AnyValue::Type::NIL:
				break;
			case AnyValue::Type::DOUBLE:
				doubleValue = value.doubleValue;
				break;
			case AnyValue::Type::BOOL:
				boolValue = value.boolValue;
				break;
			case AnyValue::Type::CHARP:
			{
				auto sz = strlen(value.charpValue) + 1;
				charpValue = new char[sz];
				strcpy_s(charpValue, sz, value.charpValue);
				break;
			}
			default:
				throw std::invalid_argument("The type of argument is not supported");
			}
			return *this;
		}

		double GetDouble() const
		{
			if (type == AnyValue::Type::DOUBLE)
				return doubleValue;
			StickThrowRuntimeError(STICKERR_INCORRECT_ARG_TYPE, (const char *)"Not double");
		}

		bool GetBool() const
		{
			if (type == AnyValue::Type::BOOL)
				return boolValue;
			StickThrowRuntimeError(STICKERR_INCORRECT_ARG_TYPE, (const char *)"Not bool");
		}

		const char * GetCharp() const
		{
			if (type == AnyValue::Type::CHARP)
				return charpValue;
			StickThrowRuntimeError(STICKERR_INCORRECT_ARG_TYPE, (const char *)"Not char *");
		}
	};

private:
	static void join_string(std::string& message, const std::string& separator) {}

	template<typename ... Args>
	static void join_string(std::string& message, const std::string& separator, const char *&& head, Args&& ... args)
	{
		message += separator + head;
		join_string(message, ":", std::move(args)...);
	}

	template<typename T, typename ... Args>
	static void join_string(std::string& message, const std::string& separator, T&& head, Args&& ... args)
	{
		message += separator + std::to_string(head);
		join_string(message, ":", std::move(args)...);
	}

public:
	template<typename ... Args>
	static std::string make_error_message(int errorCode, const char * errorMessage, Args&& ... args)
	{
		std::string message = std::string(STICKERR_PREFIX) + std::to_string(errorCode) + ":" + errorMessage;
		join_string(message, ":", std::move(args)...);
		return message;
	}

public:
	//------------------------------------------------------------------
	// Converters. Called from Lua thread and LuaStickInit.

	template<typename U, typename T>
	static void T_to_U(U & u, T const & t)
	{
		u = (U)t;
	}

//----- 22.09.06 Fukushiro M. 変更前 ()-----
//	template<>
//	static void T_to_U<std::string, std::wstring>(std::string & u, std::wstring const & t)
//	{
//		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t > converter;
//		u = converter.to_bytes(t);
//	}
//
//	template<>
//	static void T_to_U<std::wstring, std::string>(std::wstring & u, std::string const & t)
//	{
//		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t > converter;
//		u = converter.from_bytes(t);
//	}
//----- 22.09.06 Fukushiro M. 変更後 ()-----
	template<>
	static void T_to_U<std::string, std::wstring>(std::string & u, std::wstring const & t)
	{
		if (!t.empty())
		{
			auto iBuffSz = (t.length() + 1) * sizeof(wchar_t);
			std::vector<char> vBuff(iBuffSz);
			auto writtenLen = WideCharToMultiByte(CP_UTF8, 0, t.c_str(), (int)t.length(), vBuff.data(), (int)vBuff.size(), nullptr, nullptr);
			if (writtenLen == 0)
			{
				//----- The size might become more than twice of the source size -----
				iBuffSz = WideCharToMultiByte(CP_UTF8, 0, t.c_str(), (int)t.length(), nullptr, 0, nullptr, nullptr);
				vBuff.resize(iBuffSz);
				writtenLen = WideCharToMultiByte(CP_UTF8, 0, t.c_str(), (int)t.length(), vBuff.data(), (int)vBuff.size(), nullptr, nullptr);
			}
			vBuff[writtenLen] = L'\0';
			u = vBuff.data();
		}
		else
		{
			u.clear();
		}
	}

	template<>
	static void T_to_U<std::wstring, std::string>(std::wstring & u, std::string const & t)
	{
		if (!t.empty())
		{
			std::vector<wchar_t> vBuff(t.length() + 1);
			const auto writeLen = MultiByteToWideChar(CP_UTF8, 0, t.c_str(), (int)t.length(), vBuff.data(), (int)vBuff.size());
			vBuff[writeLen] = L'\0';
			u = vBuff.data();
		}
		else
		{
			u.clear();
		}
	}
//----- 22.09.06 Fukushiro M. 変更終 ()-----

	template<>
	static void T_to_U<char *, std::string>(char * & u, std::string const & t)
	{
		u = (char *)t.c_str();
	}

	template<>
	static void T_to_U<std::string, char *>(std::string & u, char * const & t)
	{
		u = t;
	}

//----- 21.05.27 Fukushiro M. 変更前 ()-----
//	static void wstring_to_wtext(wchar_t *& wtext, const std::wstring & wstr)
//	{
//		wtext = (wchar_t*)wstr.data();
//	}
//
//	static void wtext_to_wstring(std::wstring & wstr, wchar_t * const & wtx)
//	{
//		wstr = wtx;
//	}
//----- 21.05.27 Fukushiro M. 変更後 ()-----
	template<>
	static void T_to_U<wchar_t *, std::wstring>(wchar_t * & u, std::wstring const & t)
	{
		u = (wchar_t *)t.data();
	}

	template<>
	static void T_to_U<std::wstring, wchar_t *>(std::wstring & u, wchar_t * const & t)
	{
		u = t;
	}
//----- 21.05.27 Fukushiro M. 変更終 ()-----


//----- 21.05.27 Fukushiro M. 削除始 ()-----
//	template<typename T> static void int64_to_enumT(T & v, __int64 i)
//	{
//		v = (T)i;
//	}
//
//	template<typename T> static void enumT_to_int64(__int64 & i, T v)
//	{
//		i = (__int64)v;
//	}
//----- 21.05.27 Fukushiro M. 削除終 ()-----


#if defined(WIN32)
//----- 21.05.26 Fukushiro M. 変更前 ()-----
//	static void bool_to_BOOL(BOOL & v, bool b)
//	{
//		v = b ? TRUE : FALSE;
//	}
//
//	static void BOOL_to_bool(bool & b, BOOL v)
//	{
//		b = v ? 1 : 0;
//	}
//----- 21.05.26 Fukushiro M. 変更後 ()-----
	template<>
	static void T_to_U<BOOL, bool>(BOOL & u, bool const & t)
	{
		u = t ? TRUE : FALSE;
	}

	template<>
	static void T_to_U<bool, BOOL>(bool & u, BOOL const & t)
	{
		u = t ? true : false;
	}
//----- 21.05.26 Fukushiro M. 変更終 ()-----
#endif


	template<typename U, typename T>
	static void vectorT_to_vectorU(std::vector<U> & vu, const std::vector<T> & vt)
	{
		vu.clear();
		for (const auto & t : vt)
		{
			U u;
			T_to_U<U, T>(u, t);
			vu.emplace_back(u);
		}
	}

	template<typename T>
	static void vector_to_uset(std::unordered_set<T> & su, const std::vector<T> & vt)
	{
		su.clear();
		su.insert(vt.begin(), vt.end());
	}

	template<typename T>
	static void uset_to_vector(std::vector<T> & vu, const std::unordered_set<T> & st)
	{
		vu.assign(st.begin(), st.end());
	}


	template<typename U, typename T>
	static void vectorT_to_usetU(std::unordered_set<U> & su, const std::vector<T> & vt)
	{
		su.clear();
		for (const auto & t : vt)
		{
			U u;
			T_to_U<U, T>(u, t);
			su.insert(u);
		}
	}

	template<typename U, typename T>
	static void usetT_to_vectorU(std::vector<U> & vu, const std::unordered_set<T> & st)
	{
		vu.clear();
		for (const auto & t : st)
		{
			U u;
			T_to_U<U, T>(u, t);
			vu.emplace_back(u);
		}
	}

	template<typename T>
	static void vector_to_set(std::set<T> & su, const std::vector<T> & vt)
	{
		su.clear();
		su.insert(vt.begin(), vt.end());
	}

	template<typename T>
	static void set_to_vector(std::vector<T> & vu, const std::set<T> & st)
	{
		vu.assign(st.begin(), st.end());
	}


	template<typename U, typename T>
	static void vectorT_to_setU(std::set<U> & su, const std::vector<T> & vt)
	{
		su.clear();
		for (const auto & t : vt)
		{
			U u;
			T_to_U<U, T>(u, t);
			su.insert(u);
		}
	}

	template<typename U, typename T>
	static void setT_to_vectorU(std::vector<U> & vu, const std::set<T> & st)
	{
		vu.clear();
		for (const auto & t : st)
		{
			U u;
			T_to_U<U, T>(u, t);
			vu.emplace_back(u);
		}
	}




//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	static void lboolean_to_bool(bool & v, bool b)
//	{
//		v = b ? true : false;
//	}
//
//	static void bool_to_lboolean(bool & b, bool v)
//	{
//		b = v ? 1 : 0;
//	}
//----- 21.05.26 Fukushiro M. 削除終 ()-----


//----- 19.12.03 Fukushiro M. 変更前 ()-----
//	template<typename T> static void linteger_to_enumT(T & v, __int64 i)
//	{
//		v = (T)i;
//	}
//
//	template<typename T> static void enumT_to_linteger(__int64 & i, T v)
//	{
//		i = (__int64)v;
//	}
//----- 19.12.03 Fukushiro M. 変更後 ()-----
//----- 19.12.03 Fukushiro M. 変更終 ()-----

//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	template<typename T> static void ref_to_ptr(T * ptr, T & ref)
//	{
//		ptr = &ref;
//	}
//
//	template<typename T> static void ptr_to_ref(T & ref, T * ptr)
//	{
//		ref = *ptr;
//	}
//----- 21.05.26 Fukushiro M. 削除終 ()-----

//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	template<typename T> static void classobject_to_typeptr(T *& cobj, Sticklib::classobject ptr)
//	{
//		cobj = (T *)ptr;
//	}
//
//	template<typename T> static void typeptr_to_classobject(Sticklib::classobject & ptr, const T * cobj)
//	{
//		ptr = (void*)cobj;
//	}
//----- 21.05.26 Fukushiro M. 削除終 ()-----

public:
	/// <summary>
	/// Gets std::vector from the stack.
	/// This does not change the stack.
	/// </summary>
	/// <param name="v">std::vector.</param>
	/// <param name="L">Lua.</param>
	/// <param name="ud">The argument.</param>
	template<typename T>
	static void check_array(std::vector<T> & v, lua_State * L, int ud)
	{
		v.clear();

		//       stack
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |  1 |   5 |
		//                     |----|-----|
		//                     |  2 |  23 |
		//                     |----|-----|
		//                     |  3 |   0 |
		//                     |----|-----|
		//                     :    :     :

		for (lua_Integer n = 1;; n++)
		{
			//       stack
			//    +---------+
			//  -1|    5    |
			//    |---------|
			//    :         :
			//    |---------|      +----+-----+
			//  ud|  table  |----->| Key|Value|
			//    |---------|      |----|-----|
			//    :         :      |  1 |   5 |
			//                     |----|-----|
			//                     :    :     :
			if (::lua_rawgeti(L, ud, n) == LUA_TNIL) break;

			//       stack
			//    +---------+
			//  -1|    5    |  ----> Sticklib::check_lvalue returns 5
			//    |---------|
			//    :         :
			//    |---------|      +----+-----+
			//  ud|  table  |----->| Key|Value|
			//    |---------|      |----|-----|
			//    :         :      |  1 |   5 |
			//                     |----|-----|
			//                     :    :     :
			T value;
			Sticklib::check_lvalue<T>(value, L, -1);
			v.emplace_back(value);

			//       stack
			//    :         :
			//    |---------|      +----+-----+
			//  ud|  table  |----->| Key|Value|
			//    |---------|      |----|-----|
			//    :         :      |  1 |   5 |
			//                     |----|-----|
			//                     :    :     :
			lua_pop(L, 1);
		}
		//       stack
		//    +---------+
		//  -1|LUA_TNIL |
		//    |---------|
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |  1 |   5 |
		//                     |----|-----|
		//                     :    :     :

		//       stack
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |  1 |   5 |
		//                     |----|-----|
		//                     :    :     :
		lua_pop(L, 1);
	}

	template<typename K, typename V>
	static void check_hash(std::unordered_map<K, V> & v, lua_State * L, int ud)
	{
		v.clear();

		//       stack
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |"x1"|   5 |
		//                     |----|-----|
		//                     |"x2"|  23 |
		//                     |----|-----|
		//                     |"x3"|   0 |
		//                     +----+-----+

		//       stack
		//    +---------+
		//  -1|   nil   |
		//    |---------|
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |"x1"|   5 |
		//                     |----|-----|
		//                     |"x2"|  23 |
		//                     |----|-----|
		//                     |"x3"|   0 |
		//                     +----+-----+
		lua_pushnil(L);

		//       stack
		//    +---------+
		//  -1|    5    |
		//    |---------|
		//  -2|  "x1"   |
		//    |---------|
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |"x1"|   5 |
		//                     |----|-----|
		//                     |"x2"|  23 |
		//                     |----|-----|
		//                     |"x3"|   0 |
		//                     +----+-----+
		while (lua_next(L, ud) != 0)
		{
			//       stack
			//    +---------+
			//  -1|    5    | ----> Sticklib::check_lvalue(value, L, -1)
			//    |---------|
			//  -2|  "x1"   | ----> Sticklib::check_lvalue(key, L, -2)
			//    |---------|
			//    :         :
			//    |---------|      +----+-----+
			//  ud|  table  |----->| Key|Value|
			//    |---------|      |----|-----|
			//    :         :      |"x1"|   5 |
			//                     |----|-----|
			//                     :    :     :
			K key;
			V value;
			Sticklib::check_lvalue<K>(key, L, -2);			// key="x1"
			Sticklib::check_lvalue<V>(value, L, -1);		// value=5
			v[key] = value;

			//       stack
			//    +---------+
			//  -1|  "x1"   |
			//    |---------|
			//    :         :
			//    |---------|      +----+-----+
			//  ud|  table  |----->| Key|Value|
			//    |---------|      |----|-----|
			//    :         :      |"x1"|   5 |
			//                     |----|-----|
			//                     :    :     :
			lua_pop(L, 1);
		}
		//       stack
		//    :         :
		//    |---------|      +----+-----+
		//  ud|  table  |----->| Key|Value|
		//    |---------|      |----|-----|
		//    :         :      |"x1"|   5 |
		//                     |----|-----|
		//                     :    :     :
	}

	/// <summary>
	/// Converts the lua value at the given arg index to the T. If the given index value is not T, an exception will be thrown.
	/// This does not change the stack.
	/// </summary>
	/// <param name="value">value.</param>
	/// <param name="L">The l.</param>
	/// <param name="arg">The argument.</param>
	template<typename T>
	static void check_lvalue(T & value, lua_State * L, int arg)
	{
		StickThrowRuntimeError(STICKERR_SYSTEM, (const char *)"check_lvalue");
	}

	template<>
	static void check_lvalue<bool>(bool & value, lua_State * L, int arg)
	{
		//       stack
		//    :         :
		//    |---------|
		// arg|    1    |  ----> lua_toboolean returns 1
		//    |---------|
		//    :         :
		value = (lua_toboolean(L, arg) != 0) ? true : false;
	}

	template<>
	static void check_lvalue<__int64>(__int64 & value, lua_State * L, int arg)
	{
		int isnum;
		//       stack
		//    :         :
		//    |---------|
		// arg|    5    |  ----> lua_tointegerx returns 5
		//    |---------|
		//    :         :
		value = lua_tointegerx(L, arg, &isnum);
		if (!isnum)
			StickThrowRuntimeError(STICKERR_INCORRECT_ARG_TYPE, (const char *)"Not integer");
	}

	template<>
	static void check_lvalue<double>(double & value, lua_State * L, int arg)
	{
		int isnum;
		//       stack
		//    :         :
		//    |---------|
		// arg|  3.14   |  ----> lua_tonumberx returns 3.14
		//    |---------|
		//    :         :
		value = lua_tonumberx(L, arg, &isnum);
		if (!isnum)
			throw std::invalid_argument("The argument is not number");
	}

	template<>
	static void check_lvalue<std::string>(std::string & value, lua_State * L, int arg)
	{
		const auto acp = lua_tolstring(L, arg, nullptr);
		if (acp == nullptr)
			throw std::invalid_argument("The argument is not string");
		value = acp;
	}

	template<>
	static void check_lvalue<void *>(void * & value, lua_State * L, int arg)
	{
		// lua_touserdata is valid for userdata and lightuserdata both.
		// lua_touserdata returns nullptr if the value in the stack is neither userdata or lightluserdata.
		// But if the stack value is nullptr lightuserdata, then it returns nullptr too.
		// So we must test the value using lua_type.
		switch (lua_type(L, arg))
		{
		case LUA_TUSERDATA:
		case LUA_TLIGHTUSERDATA:
			value = lua_touserdata(L, arg);
			break;
		default:
			throw std::invalid_argument("The argument is not userdata nor lightuserdata");
		}
	}

	template<>
	static void check_lvalue<Sticklib::AnyValue>(Sticklib::AnyValue & value, lua_State * L, int arg)
	{
		//       stack
		//    :         :
		//    |---------|
		// arg|    5    |  ----> lua_tointegerx returns 5
		//    |---------|
		//    :         :
		const auto type = lua_type(L, arg);
		switch (type)
		{
		case LUA_TNIL:
			value = AnyValue();
			break;
		case LUA_TNUMBER:
		{
			double v;
			check_lvalue<double>(v, L, arg);
			value = AnyValue(v);
			break;
		}
		case LUA_TBOOLEAN:
		{
			bool v;
			check_lvalue<bool>(v, L, arg);
			value = AnyValue(v);
			break;
		}
		case LUA_TSTRING:
		{
			std::string v;
			check_lvalue<std::string>(v, L, arg);
			value = AnyValue(v.c_str());
			break;
		}
		default:
			throw std::invalid_argument("The type of argument is not supported");
		}
	}

//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	template<typename T>
//	static void check_classobject(T & value, lua_State * L, int arg)
//	{
//		// lua_touserdata is valid for userdata and lightuserdata both.
//		auto wrapper = (StickInstanceWrapper *)lua_touserdata(L, arg);
//		if (!wrapper)
//			throw std::invalid_argument("The argument is not userdata nor lightuserdata");
//		value = (T)wrapper->ptr;
//	}
//----- 21.05.26 Fukushiro M. 削除終 ()-----

//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	/// <summary>
//	/// Gets std::vector from the stack.
//	/// This does not change the stack.
//	/// </summary>
//	/// <param name="v">std::vector.</param>
//	/// <param name="L">Lua.</param>
//	/// <param name="ud">The argument.</param>
//	template<typename T>
//	static void check_classobjectarray(std::vector<T> & v, lua_State * L, int ud)
//	{
//		v.clear();
//
//		//       stack
//		//    :         :
//		//    |---------|      +----+---------+
//		//  ud|  table  |----->| Key|  Value  |
//		//    |---------|      |----|---------|
//		//    :         :      |  1 | object1 |
//		//                     |----|---------|
//		//                     |  2 | object2 |
//		//                     |----|---------|
//		//                     |  3 | object3 |
//		//                     |----|---------|
//		//                     :    :         :
//
//		for (lua_Integer n = 1;; n++)
//		{
//			//       stack
//			//    +---------+
//			//  -1| object1 |
//			//    |---------|
//			//    :         :
//			//    |---------|      +----+---------+
//			//  ud|  table  |----->| Key|  Value  |
//			//    |---------|      |----|---------|
//			//    :         :      |  1 | object1 |
//			//                     |----|---------|
//			//                     |  2 | object2 |
//			//                     |----|---------|
//			//                     :    :         :
//			//
//			if (::lua_rawgeti(L, ud, n) == LUA_TNIL) break;
//
//			//       stack
//			//    +---------+
//			//  -1| object1 |  ----> Sticklib::check_lvalue returns 5
//			//    |---------|
//			//    :         :
//			//    |---------|      +----+---------+
//			//  ud|  table  |----->| Key|  Value  |
//			//    |---------|      |----|---------|
//			//    :         :      |  1 | object1 |
//			//                     |----|---------|
//			//                     |  2 | object2 |
//			//                     |----|---------|
//			//                     :    :         :
//			//
//			T value;
//			Sticklib::check_classobject<T>(value, L, -1);
//			v.emplace_back(value);
//
//			//       stack
//			//    :         :
//			//    |---------|      +----+---------+
//			//  ud|  table  |----->| Key|  Value  |
//			//    |---------|      |----|---------|
//			//    :         :      |  1 | object1 |
//			//                     |----|---------|
//			//                     |  2 | object2 |
//			//                     |----|---------|
//			//                     :    :         :
//			//
//			lua_pop(L, 1);
//		}
//		//       stack
//		//    +---------+
//		//  -1|LUA_TNIL |
//		//    |---------|
//		//    :         :
//		//    |---------|      +----+---------+
//		//  ud|  table  |----->| Key|  Value  |
//		//    |---------|      |----|---------|
//		//    :         :      |  1 | object1 |
//		//                     |----|---------|
//		//                     |  2 | object2 |
//		//                     |----|---------|
//		//                     :    :         :
//		//
//
//		//       stack
//		//    :         :
//		//    |---------|      +----+---------+
//		//  ud|  table  |----->| Key|  Value  |
//		//    |---------|      |----|---------|
//		//    :         :      |  1 | object1 |
//		//                     |----|---------|
//		//                     |  2 | object2 |
//		//                     |----|---------|
//		//                     :    :         :
//		//
//		lua_pop(L, 1);
//	}
//----- 21.05.26 Fukushiro M. 削除終 ()-----


//----- 21.05.26 Fukushiro M. 削除始 ()-----
//#if 0
//	/// <summary>
//	/// Converts the lua value at the given arg index to the lua_Integer. If the given index value is not integer, an exception will be thrown.
//	/// This does not change the stack.
//	/// </summary>
//	/// <param name="L">The l.</param>
//	/// <param name="arg">The argument.</param>
//	/// <returns></returns>
//	static lua_Integer checkinteger(lua_State *L, int arg)
//	{
//		int isnum;
//		//       stack
//		//    :         :
//		//    |---------|
//		// arg|    5    |  ----> lua_tointegerx returns 5
//		//    |---------|
//		//    :         :
//		lua_Integer d = lua_tointegerx(L, arg, &isnum);
//		if (!isnum)
//			StickThrowRuntimeError(STICKERR_INCORRECT_ARG_TYPE, (const char *)"Not integer");
//		return d;
//	}
//
//	/// <summary>
//	/// Converts the lua value at the given arg index to the lua_Number. If the given index value is not number, an exception will be thrown.
//	/// This does not change the stack.
//	/// </summary>
//	/// <param name="L">The l.</param>
//	/// <param name="arg">The argument.</param>
//	/// <returns></returns>
//	static lua_Number checknumber(lua_State *L, int arg)
//	{
//		int isnum;
//		//       stack
//		//    :         :
//		//    |---------|
//		// arg|  3.14   |  ----> lua_tonumberx returns 3.14
//		//    |---------|
//		//    :         :
//		lua_Number d = lua_tonumberx(L, arg, &isnum);
//		if (!isnum)
//			throw std::invalid_argument("The argument is not number");
//		return d;
//	}
//
//	/// <summary>
//	/// Converts the lua value at the given arg index to the string. If the given index value is not string, an exception will be thrown.
//	/// This does not change the stack.
//	/// </summary>
//	/// <param name="L">The l.</param>
//	/// <param name="arg">The argument.</param>
//	/// <returns></returns>
//	static const char * checkstring(lua_State *L, int arg)
//	{
//		const  char * s = lua_tolstring(L, arg, nullptr);
//		if (!s)
//			throw std::invalid_argument("The argument is not string");
//		return s;
//	}
//
//	static void * checklightuserdata(lua_State *L, int arg)
//	{
//		void * p = lua_touserdata(L, arg);
//		if (!p)
//			throw std::invalid_argument("The argument is not userdata");
//		return p;
//	}
//
//#endif//0
//----- 21.05.26 Fukushiro M. 削除終 ()-----

	/// <summary>
	/// Converts the lua value at the given arg index to the class object. If the given index value is not target class Type, an exception will be thrown.
	/// This does not change the stack.
	/// </summary>
	/// <param name="L">The l.</param>
	/// <param name="arg">The argument.</param>
	/// <returns></returns>
	static void * test_classobject(lua_State *L, int ud, const char * tname)
	{
		void * p = luaL_testudata(L, ud, tname);
		if (p == nullptr)
			throw std::invalid_argument("The argument is not correct type");
		return p;
	}


private:
//----- 20.01.16 Fukushiro M. 削除始 ()-----
//	/// <summary>
//	/// Returns the user data if it is tname-Type or its subclass Type.
//	/// if not, returns nullptr.
//	/// </summary>
//	/// <param name="L">The l.</param>
//	/// <param name="ud">stack index where user data is.</param>
//	/// <param name="tname">user data Type.</param>
//	/// <returns>user data pointer</returns>
//	static void * testsubclassobject(lua_State * L, int ud, const char * tname)
//	{
//		ud = lua_absindex(L, ud);
//
//		//        stack
//		//     |          |
//		//     :          :
//		//     |----------|
//		//   ud| udata U  |
//		//     |----------|
//		//     :          :
//		void * p = lua_touserdata(L, ud);
//		// if value is not a userdata.
//		if (p == nullptr) return nullptr;
//
//		//        stack
//		//     |          |
//		//     |----------|
//		//   -1| meta T   |
//		//     |----------|
//		//     :          :
//		//     |----------|
//		//   ud| udata U  |
//		//     |----------|
//		//     :          :
//		if (luaL_getmetatable(L, tname) != LUA_TTABLE)  /* get correct metatable */
//		{
//			//        stack
//			//     |          |
//			//     |----------|
//			//   -1| LUA_TNIL |                 stack
//			//     |----------|              |          |
//			//     :          :    ==>       :          :
//			//     |----------|              |----------|
//			//   ud| udata U  |            ud| udata U  |
//			//     |----------|              |----------|
//			//     :          :              :          :
//			lua_pop(L, 1);  /* remove both metatables */
//			return nullptr;
//		}
//
//		const int indexT = lua_gettop(L);
//
//		//        stack
//		//     |          |
//		//     |----------|
//		//   -1| meta U   |
//		//     |----------|
//		//   -2| meta T   |indexT
//		//     |----------|
//		//     :          :
//		//     |----------|
//		//   ud| udata U  |
//		//     |----------|
//		//     :          :
//		if (lua_getmetatable(L, ud) == 0)
//		{
//			//        stack
//			//     |          |
//			//     |----------|
//			//   -1| meta T   |                 stack
//			//     |----------|              |          |
//			//     :          :    ==>       :          :
//			//     |----------|              |----------|
//			//   ud| udata U  |            ud| udata U  |
//			//     |----------|              |----------|
//			//     :          :              :          :
//			lua_pop(L, 1);  /* remove both metatables */
//			return nullptr;
//		}
//
//		for (;;)
//		{
//			//        stack
//			//     |          |
//			//     |----------|
//			//   -1| meta U   |
//			//     |----------|
//			//   -2| meta T   |indexT
//			//     |----------|
//			//     :          :
//			//     |----------|
//			//   ud| udata U  |
//			//     |----------|
//			//     :          :
//			if (lua_rawequal(L, -1, indexT))  /* not the same? */
//			{
//				break;
//			}
//
//			//        stack
//			//     |          |
//			//     |----------|
//			//   -1| meta UU  |
//			//     |----------|
//			//   -2| meta U   |
//			//     |----------|
//			//   -3| meta T   |indexT
//			//     |----------|
//			//     :          :
//			//     |----------|
//			//   ud| udata U  |
//			//     |----------|
//			//     :          :
//			if (lua_getmetatable(L, -1) == 0)
//			{
//				p = nullptr;
//				break;
//			}
//		}
//
//		//        stack
//		//     |          |
//		//     |----------|
//		//   -1| meta U   |
//		//     |----------|
//		//   -2| meta T   |indexT             stack
//		//     |----------|                |          |
//		//     :          :      ==>       :          :
//		//     |----------|                |----------|
//		//   ud| udata U  |              ud| udata U  |
//		//     |----------|                |----------|
//		//     :          :                :          :
//		lua_pop(L, lua_gettop(L) - indexT + 1);  /* remove both metatables */
//		return p;
//	}
//----- 20.01.16 Fukushiro M. 削除終 ()-----

public:
	/// <summary>
	/// Push the value on the stack.
	/// This does not change the stack.
	/// </summary>
	/// <param name="L">The l.</param>
	/// <param name="value">value.</param>
	template<typename T>
	static void push_lvalue(lua_State * L, T const & value, bool own)
	{
		StickThrowRuntimeError(STICKERR_SYSTEM, (const char *)"push_lvalue");
	}

	template<>
	static void push_lvalue<double>(lua_State * L, double const & value, bool own)
	{
		lua_pushnumber(L, value);
	}

	template<>
	static void push_lvalue<__int64>(lua_State * L, __int64 const & value, bool own)
	{
		lua_pushinteger(L, value);
	}

	template<>
	static void push_lvalue<bool>(lua_State * L, bool const & value, bool own)
	{
		lua_pushboolean(L, value ? 1 : 0);
	}

	template<>
	static void push_lvalue<charConstP>(lua_State * L, charConstP const & value, bool own)
	{
		lua_pushstring(L, value);
	}

	template<>
	static void push_lvalue<charP>(lua_State * L, charP const & value, bool own)
	{
		lua_pushstring(L, value);
	}

	template<>
	static void push_lvalue<voidp>(lua_State * L, voidp const & value, bool own)
	{
		lua_pushlightuserdata(L, value);
	}

	template<>
	static void push_lvalue<std::string>(lua_State * L, std::string const & value, bool own)
	{
		lua_pushlstring(L, value.c_str(), value.length());
	}

	template<>
	static void push_lvalue<Sticklib::AnyValue>(lua_State * L, Sticklib::AnyValue const & value, bool own)
	{
		switch (value.type)
		{
		case AnyValue::Type::NIL:
			lua_pushnil(L);
			break;
		case AnyValue::Type::DOUBLE:
			push_lvalue<double>(L, value.doubleValue, own);
			break;
		case AnyValue::Type::BOOL:
			push_lvalue<bool>(L, value.boolValue, own);
			break;
		case AnyValue::Type::CHARP:
			push_lvalue<charP>(L, value.charpValue, own);
			break;
		default:
			throw std::invalid_argument("The type of argument is not supported");
		}
	}

//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	/// <summary>
//	/// Allocate StickInstanceWrapper-class object as a userdata in Lua, which has the pointer of the class object.
//	/// And push it on the lua-stack. Metatable is assigned to the userdata.
//	/// </summary>
//	/// <param name="L">Lua object</param>
//	/// <param name="object">Class object.</param>
//	/// <param name="own">true:Own the class object. The class object will be deleted automatically./false:Do not won the class object.</param>
//	/// <param name="metatable_name">Name of the metatable.</param>
//	template<typename T>
//	static void push_classobject(lua_State * L, T const & object, bool own, const char * metatable_name)
//	{
//		//                   Premise.
//		//       stack
//		//    +---------+
//		//  -1|   v1    |
//		//    |---------|
//		//    :         :
//		//
//
//		//       stack
//		//    |---------|            +------------------+
//		//  -1|userdata |----------->| allocated memory |
//		//    |---------|            +------------------+
//		//  -2|   v1    |
//		//    |---------|            |------------------|
//		//    :         :          sizeof(StickInstanceWrapper)
//		//
//		auto ptr = (StickInstanceWrapper *)lua_newuserdata(L, sizeof(StickInstanceWrapper));
//		ptr->own = own;
//		ptr->ptr = object;
//
//		//       stack
//		//    |---------|
//		//  -1|metatable|--------------------------+   +----------------------------------------+
//		//    |---------|                          |   |                       registry         |
//		//  -2|userdata |---+                      V   V                    +-------+-------+   |
//		//    |---------|   |              +----------+--------+            | Key   | Value |   |
//		//    |   v1    |   |              |   Key    | Value  |            |-------|-------|   |
//		//    |---------|   |              |----------|--------|            |regName| table |---+
//		//    :         :   |              |  "__gc"  |c++func1<---+        +---A---+-------+
//		//                  |              +----------|--------+   |        :   |   :       :
//		//                  |              | "xxxxx"  |c++func2|   |            |
//		//                  |              +----------|--------+   |       metatable_name
//		//                  |              :          :        :   |
//		//                  |                                      |
//		//                  |    +------------------+              |
//		//                  +--->| allocated memory |        c++ Destructor
//		//                       +------------------+
//		//
//		luaL_getmetatable(L, metatable_name);
//
//		//                             metatable
//		//       stack              +--------------+   +----------------------------------------+
//		//    |---------|           |              |   |                       registry         |
//		//  -1|userdata |---+       |              V   V                    +-------+-------+   |
//		//    |---------|   |       |      +----------+--------+            | Key   | Value |   |
//		//    |   v1    |   |       |      |   Key    | Value  |            |-------|-------|   |
//		//    |---------|   |       |      |----------|--------|            |regName| table |---+
//		//    :         :   |       |      |  "__gc"  |c++func1<---+        +---A---+-------+
//		//                  |       |      +----------|--------+   |        :   |   :       :
//		//                  |       |      | "xxxxx"  |c++func2|   |            |
//		//                  |       |      +----------|--------+   |       metatable_name
//		//                  |       |      :          :        :   |
//		//                  |       |                              |
//		//                  |    +------------------+              |
//		//                  +--->| allocated memory |        c++ Destructor
//		//                       +------------------+
//		//
//		::lua_setmetatable(L, -2);
//	}
//----- 21.05.26 Fukushiro M. 削除終 ()-----

	template<typename T>
	static void push_array(lua_State * L, const std::vector<T> & v, bool own)
	{
		//                   Premise.
		//       stack
		//    +---------+
		//  -1| result1 | <-- return value 1 from c++ function.
		//    |---------|
		//  -2|   v2    | <-- argument 2 for c++ function.
		//    |---------|
		//  -3|   v1    | <-- argument 1 for c++ function.
		//    |---------|
		//    :         :

		//       stack
		//    +---------+      +--------+--------+
		//  -1|  table  |----->| Key    | Value  |
		//    |---------|      +--------+--------+ -+
		//  -2| result1 |      :                 :  |
		//    |---------|      :                 :  |
		//    |   v2    |      :                 :  |v.size() prepared.
		//    |---------|      :                 :  |
		//    :         :      :                 :  |
		//                     +- - - - - - - - -+ -+
		//
		lua_createtable(L, (int)v.size(), 0);

		lua_Integer i = 1;
		for (const auto & value : v)
		{
			//       stack
			//    +---------+
			//  -1|  value  |
			//    |---------|      +--------+--------+
			//  -2|  table  |----->| Key    | Value  |
			//    |---------|      +--------+--------+
			//  -3| result1 |
			//    |---------|
			//    :         :
			Sticklib::push_lvalue<T>(L, value, own);

			//       stack
			//    +---------+      +--------+--------+
			//  -1|  table  |----->| Key    | Value  |
			//    |---------|      |--------+--------|
			//  -2| result1 |      :        :        :
			//    |---------|      |--------|--------|
			//    :         :      |   i    | value  |
			//                     +--------+--------+
			lua_rawseti(L, -2, i);
			i++;
		}
	}

//----- 21.05.26 Fukushiro M. 削除始 ()-----
////----- 21.05.25 Fukushiro M. 追加始 ()-----
//	template<typename T>
//	static void push_classobjectarray(lua_State * L, const std::vector<T> & v, bool own)
//	{
//		//                   Premise.
//		//       stack
//		//    +---------+
//		//  -1| result1 | <-- return value 1 from c++ function.
//		//    |---------|
//		//  -2|   v2    | <-- argument 2 for c++ function.
//		//    |---------|
//		//  -3|   v1    | <-- argument 1 for c++ function.
//		//    |---------|
//		//    :         :
//
//		//       stack
//		//    +---------+      +--------+--------+
//		//  -1|  table  |----->| Key    | Value  |
//		//    |---------|      +--------+--------+ -+
//		//  -2| result1 |      :                 :  |
//		//    |---------|      :                 :  |
//		//    |   v2    |      :                 :  |v.size() prepared.
//		//    |---------|      :                 :  |
//		//    :         :      :                 :  |
//		//                     +- - - - - - - - -+ -+
//		//
//		lua_createtable(L, v.size(), 0);
//
//		lua_Integer i = 1;
//		for (const auto & object : v)
//		{
//			//       stack
//			//    +---------+
//			//  -1| object  |
//			//    |---------|      +--------+--------+
//			//  -2|  table  |----->| Key    | Value  |
//			//    |---------|      +--------+--------+
//			//  -3| result1 |
//			//    |---------|
//			//    :         :
//			Sticklib::push_classobject<T>(L, object, own);
//
//			//       stack
//			//    +---------+      +--------+--------+
//			//  -1|  table  |----->| Key    | Value  |
//			//    |---------|      |--------+--------|
//			//  -2| result1 |      :        :        :
//			//    |---------|      |--------|--------|
//			//    :         :      |   i    | object |
//			//                     +--------+--------+
//			lua_rawseti(L, -2, i);
//			i++;
//		}
//	}
////----- 21.05.25 Fukushiro M. 追加終 ()-----
//----- 21.05.26 Fukushiro M. 削除終 ()-----

	template<typename K, typename V>
	static void push_hash(lua_State * L, const std::unordered_map<K, V> & v, bool own)
	{
		//                   Premise.
		//       stack
		//    +---------+
		//  -1| result1 | <-- return value 1 from c++ function.
		//    |---------|
		//  -2|   v2    | <-- argument 2 for c++ function.
		//    |---------|
		//  -3|   v1    | <-- argument 1 for c++ function.
		//    |---------|
		//    :         :

		//       stack
		//    +---------+      +---------+---------+
		//  -1|  table  |----->| Key     | Value   |
		//    |---------|      +---------+---------+ -+
		//  -2| result1 |      :                   :  |
		//    |---------|      :                   :  |
		//    :         :      :                   :  |v.size() prepared.
		//                     :                   :  |
		//                     :                   :  |
		//                     +- - - - - - - - - -+ -+
		//
		lua_createtable(L, (int)v.size(), 0);

		for (const auto & kv : v)
		{
			//       stack
			//    +---------+
			//  -1|kv.first |
			//    |---------|      +---------+---------+
			//  -2|  table  |----->| Key     | Value   |
			//    |---------|      +---------+---------+
			//  -3| result1 |
			//    |---------|
			//    :         :
			Sticklib::push_lvalue<K>(L, kv.first, false);

			//       stack
			//    +---------+
			//  -1|kv.second|
			//    |---------|
			//  -2|kv.first |
			//    |---------|      +---------+---------+
			//  -3|  table  |----->| Key     | Value   |
			//    |---------|      +---------+---------+
			//  -4| result1 |
			//    |---------|
			//    :         :
			Sticklib::push_lvalue<V>(L, kv.second, own);

			//       stack
			//    +---------+      +---------+---------+
			//  -1|  table  |----->| Key     | Value   |
			//    |---------|      |---------+---------|
			//  -2| result1 |      | kv.first|kv.second|
			//    |---------|      +---------+---------+
			//    :         :
			//
			lua_settable(L, -3);
		}
	}

public:

	//-----------------------------------------------------------------------------------
	// Functions called from LuaStickInit.

	static void push_table(lua_State * L, const char * name)
	{
		//                      Premise.
		// if name = nullptr      |                   if name != nullptr
		//                        |
		//       stack            |           stack
		//    +---------+         |        +---------+      +---------+--------------+
		//    :         :         |        |   _G    |----->| Key     |    Value     |
		//    :         :         |        |---------|      |---------|--------------|
		//                        |        :         :      :         :              :

		if (name == nullptr || *name == '\0')
		{
			//       stack
			//    +---------+      +---------+--------------+
			//    |   _G    |----->| Key     |    Value     |
			//    |---------|      |---------|--------------|
			//    :         :      :         :              :
			lua_pushglobaltable(L);
			return;
		}

		//       stack
		//    +---------+
		//  -1|  name   |
		//    |---------|     +--------+--------+
		//  -2|   _G    |---->| Key    | Value  |
		//    |---------|     |--------|--------|     +--------+--------+
		//    :         :     | name   | table  |---->| Key    | Value  |
		//                    |--------|--------|     |--------|--------|
		//                    :        :        :     :        :        :
		lua_pushstring(L, name);

		//       stack
		//    +---------+
		//  -1|  table  |------------------------------------+
		//    |---------|     +--------+--------+            |
		//  -2|   _G    |---->| Key    | Value  |            V
		//    |---------|     |--------|--------|      +--------+--------+
		//    :         :     | name   | table  |----->| Key    | Value  |
		//                    |--------|--------|      |--------|--------|
		//                    :        :        :      :        :        :
		lua_rawget(L, -2);
		if (!lua_istable(L, -1))
		{	//----- if table does not exist -----
			// create new table.

			//
			//       stack
			//    +---------+
			//  -1|   _G    |
			//    |---------|
			//    :         :


			lua_pop(L, 1);

			//       stack
			//    +---------+      +--------+--------+
			//  -1|  table  |----->| Key    | Value  |
			//    |---------|      |--------|--------|
			//  -2|   _G    |      :        :        :
			//    |---------|
			//    :         :
			lua_newtable(L);

			//       stack
			//    +---------+
			//  -1|  name   |
			//    |---------|      +--------+--------+
			//  -2|  table  |----->| Key    | Value  |
			//    |---------|      |--------|--------|
			//  -3|   _G    |      :        :        :
			//    |---------|
			//    :         :
			lua_pushstring(L, name);

			//       stack
			//    +---------+
			//  -1|  table  |--+
			//    |---------|  |
			//  -2|  name   |  |
			//    |---------|  |   +--------+--------+
			//  -3|  table  |--+-->| Key    | Value  |
			//    |---------|      |--------|--------|
			//  -4|   _G    |      :        :        :
			//    |---------|
			//    :         :
			lua_pushvalue(L, -2);

			//       stack
			//    +---------+
			//  -1|  table  |------------------------------------+
			//    |---------|      +--------+--------+           |
			//  -2|   _G    |----->| Key    | Value  |           V
			//    |---------|      |--------|--------|      +--------+--------+
			//    :         :      | name   | table  |----->| Key    | Value  |
			//                     |--------|--------|      |--------|--------|
			//                     :        :        :      :        :        :
			lua_rawset(L, -4);
		}
	}

	static void pop(lua_State * L)
	{
		lua_pop(L, 1);
	}

	static void remove_table_item(lua_State * L, const char * name)
	{
		//       stack
		//    +---------+      +--------+--------+
		//  -1|  table  |----->| Key    | Value  |
		//    |---------|      |--------|--------|
		//    :         :      |"nameA" | valueA |
		//                     |--------|--------|
		//                     :        :        :

		//       stack
		//    +---------+
		//  -1| "nameA" |
		//    |---------|      +--------+--------+
		//  -2|  table  |----->| Key    | Value  |
		//    |---------|      |--------|--------|
		//    :         :      |"nameA" | valueA |
		//                     |--------|--------|
		//                     :        :        :
		::lua_pushstring(L, name);

		//       stack
		//    +---------+
		//  -1|   nil   |
		//    |---------|
		//  -2| "nameA" |
		//    |---------|      +--------+--------+
		//  -3|  table  |----->| Key    | Value  |
		//    |---------|      |--------|--------|
		//    :         :      |"nameA" | valueA |
		//                     |--------|--------|
		//                     :        :        :
		::lua_pushnil(L);

		//       stack
		//    +---------+      +--------+--------+
		//  -1|  table  |----->| Key    | Value  |
		//    |---------|      |--------|--------|
		//    :         :      :        :        :
		::lua_rawset(L, -3);
	}

#if 0
	static void register_class(lua_State * L, const char * regName, const luaL_Reg methods[], const char * superRegName)
	{
		//        stack
		//     |          |
		//     |----------|
		//     |          |
		//     :          :
		// int luaL_newmetatable (lua_State * L, const char * tname)
		// レジストリにキー tname がすでにあれば0を返します。 まだなければ、ユーザーデータ用のメタテーブルとして
		// 使われる新しいテーブルを作成し、ペア __name = tname をこの新しいテーブルに追加し、ペア[tname] = 新しいテーブル
		// をレジストリに追加し、1を返します。(エントリ __name はエラー報告関数のいくつかで使われます。)
		// どちらの場合でも、レジストリの tname に関連付けられた最終的な値がスタックに積まれます。

		//
		//                    +-----------+  +-----------------------------------+
		//                    |           |  |                  registry         |
		//        stack       |           V  V               +-------+-------+   |
		//     |          |   |   +----------+-------+       | Key   | Value |   |
		//     |----------|   |   |   Key    | Value |       |-------|-------|   |
		//     | table    |---+   |----------|-------|       |regName| table |---+
		//     |----------|       | "__name" |regName|       +-------+-------+
		//     :          :       +----------+-------+       :       :       :
		luaL_newmetatable(L, regName);

		//        stack       +-----------+  +-----------------------------------+
		//     |          |   |           |  |                  registry         |
		//     |----------|   |           V  V               +-------+-------+   |
		//   -1|"__index" |   |   +----------+-------+       | Key   | Value |   |
		//     |----------|   |   |   Key    | Value |       |-------|-------|   |
		//   -2| table    |---+   |----------|-------|       |regName| table |---+
		//     |----------|       | "__name" |regName|       +-------+-------+
		//     :          :       +----------+-------+       :       :       :
		lua_pushstring(L, "__index");

		// void lua_pushvalue (lua_State * L, int index);
		// 指定されたインデックスの要素のコピーをスタックに積みます。
		//
		//        stack
		//     |          |  +--------------+
		//     |----------|  | +----------+ | +----------------------------------+
		//   -1| table    |--+ |          | | |                 registry         |
		//     |----------|    |          V V V              +-------+-------+   |
		//   -2|"__index" |    |  +----------+-------+       | Key   | Value |   |
		//     |----------|    |  |   Key    | Value |       |-------|-------|   |
		//   -3| table    |----+  |----------|-------|       |regName| table |---+
		//     |----------|       | "__name" |regName|       +-------+-------+
		//     :          :       +----------+-------+       :       :       :
		lua_pushvalue(L, -2);

		// void lua_settable (lua_State * L, int index);
		// t[k] = v と同等のことをします。 t はスタック[index]の値、 v はスタック[-1]の値で、 k はスタック[-2]の値。
		// キーと値は両方ともスタックから削除されます。
		//
		//
		//                                  +------------------------------------+
		//                     +----------+ | +----------+                       |
		//                     |          | | |          |      registry         |
		//        stack        |          V V V          |   +-------+-------+   |
		//     |          |    |  +----------+-------+   |   | Key   | Value |   |
		//     |----------|    |  |   Key    | Value |   |   |-------|-------|   |
		//     | table    |----+  |----------|-------|   |   |regName| table |---+
		//     |----------|       | "__name" |regName|   |   +-------+-------+
		//     :          :       |----------|-------|   |   :       :       :
		//                        |"__index" | table |---+
		//                        +----------+-------+
		lua_settable(L, -3);

		// void luaL_setfuncs (lua_State * L, const luaL_Reg *l, int nup);
		// 配列 l (luaL_Reg を参照) のすべての関数を、スタックトップのテーブル (ただし上位値がある場合はその下です。下記参照) に登録します。
		//
		//
		//                                  +------------------------------------+
		//                     +----------+ | +----------+                       |
		//                     |          | | |          |      registry         |
		//        stack        |          V V V          |   +-------+-------+   |
		//     |          |    |  +----------+-------+   |   | Key   | Value |   |
		//     |----------|    |  |   Key    | Value |   |   |-------|-------|   |
		//     | table    |----+  |----------|-------|   |   |regName| table |---+
		//     |----------|       | "__name" |regName|   |   +-------+-------+
		//     :          :       |----------|-------|   |   :       :       :
		//                        |"__index" | table |---+
		//                        |----------|-------|
		//                        | "__gc"   |c-func1|
		//                        |----------|-------|
		//                        |"SetValue"|c-func2|
		//                        +----------+-------+
		luaL_setfuncs(L, methods, 0);

		if (superRegName != nullptr && *superRegName != '\0')
		{	//----- set superclass if specified -----
			//        stack
			//     |          |
			//     |----------|
			//   -1| table S  |---------------------------------------------------------------------------------------+
			//     |----------|                        +------------------------------------------+  +-------------+  |
			//   -2| table    |----------------------+ | +----------+                             |  |             |  |
			//     |----------|                      | | |          |      registry               |  |             |  |
			//     :          :                      V V V          |   +------------+--------+   |  |             V  V
			//                               +----------+-------+   |   |     Key    | Value  |   |  |    +----------+-------+
			//                               |   Key    | Value |   |   |------------|--------|   |  |    |   Key    | Value |
			//                               |----------|-------|   |   |  regName   | table  |---+  |    |----------|-------|
			//                               | "__name" |regName|   |   |------------|--------|      |    | "__name" |regName|
			//                               |----------|-------|   |   |superRegName| table  |------+    |----------|-------|
			//                               |"__index" | table |---+   |------------|--------|           |"__index" | table |
			//                               |----------|-------|       :            :        :           |----------|-------|
			//                               | "__gc"   |c-func1|                                         | "__gc"   |c-func1|
			//                               |----------|-------|                                         |----------|-------|
			//                               |"SetValue"|c-func2|                                         |"GetValue"|c-func2|
			//                               +----------+-------+                                         +----------+-------+
			luaL_getmetatable(L, superRegName);

			//        stack
			//     |          |
			//     |----------|                        +------------------------------------------+  +-------------+
			//     | table    |----------------------+ | +----------+                             |  |             |
			//     |----------|                      | | |          |      registry               |  |             |
			//     :          :                      V V V          |   +------------+--------+   |  |             V
			//                               +----------+-------+   |   |     Key    | Value  |   |  |    +----------+-------+
			//                               |   Key    | Value |   |   |------------|--------|   |  |    |   Key    | Value |
			//                               |----------|-------|   |   |  regName   | table  |---+  |    |----------|-------|
			//                               | "__name" |regName|   |   |------------|--------|      |    | "__name" |regName|
			//                               |----------|-------|   |   |superRegName| table  |------+    |----------|-------|
			//                               |"__index" | table |---+   |------------|--------|           |"__index" | table |
			//                               |----------|-------|       :            :        :           |----------|-------|
			//                               | "__gc"   |c-func1|                                         | "__gc"   |c-func1|
			//                               |----------|-------|                metatable                |----------|-------|
			//                               |"SetValue"|c-func2|========================================>|"GetValue"|c-func2|
			//                               +----------+-------+                                         +----------+-------+


			// テスト。ここでメモリーリークが発生。なぜ？
			lua_setmetatable(L, -2);
			// lua_pop(L, 1);
		}

		//
		//
		//                                  +------------------------------------+
		//                                  | +----------+                       |
		//                                  | |          |      registry         |
		//                                  V V          |   +-------+-------+   |
		//                        +----------+-------+   |   | Key   | Value |   |
		//        stack           |   Key    | Value |   |   |-------|-------|   |
		//     |          |       |----------|-------|   |   |regName| table |---+
		//     |----------|       | "__name" |regName|   |   +-------+-------+
		//     :          :       |----------|-------|   |   :       :       :
		//                        |"__index" | table |---+
		//                        |----------|-------|
		//                        | "__gc"   |c-func1|
		//                        |----------|-------|
		//                        |"SetValue"|c-func2|
		//                        +----------+-------+
		lua_pop(L, 1);
	}
#endif

	static void register_class(lua_State * L, const char * regName, const luaL_Reg methods[], const char * superRegName)
	{
		//       Premise.
		//        stack
		//     |          |
		//     |          |
		//     :          :
		// int luaL_newmetatable (lua_State * L, const char * tname)
		// レジストリにキー tname がすでにあれば0を返します。 そうでなければ、ユーザーデータ用のメタテーブルとして
		// 使われる新しいテーブルを作成し、ペア __name = tname をこの新しいテーブルに追加し、ペア[tname] = 新しいテーブル
		// をレジストリに追加し、1を返します。(エントリ __name はエラー報告関数のいくつかで使われます。)
		// どちらの場合でも、レジストリの tname に関連付けられた最終的な値がスタックに積まれます。


		// If superRegName is specified, every key and value in the superRegName is copied into the new metatable.
		// It means 'new metatable inherit functions from superRegName'.

		//
		//                    +-----------+  +-----------------------------------+
		//                    |           |  |                  registry         |
		//        stack       |           V  V               +-------+-------+   |
		//                    |   +----------+-------+       | Key   | Value |   |
		//     +----------+   |   |   Key    | Value |       |-------|-------|   |
		//     | table    |---+   |----------|-------|       |regName| table |---+
		//     |----------|       | "__name" |regName|       +-------+-------+
		//     :          :       +----------+-------+       :       :       :
		//                              metatable
		luaL_newmetatable(L, regName);

		if (superRegName != nullptr && *superRegName != '\0')
		{	//----- set superclass if specified -----
			//        stack
			//     +----------+
			//   -1| table S  |---------------------------------------------------------------------------------------+
			//     |----------|                        +------------------------------------------+  +-------------+  |  +------------+
			//   -2| table    |----------------------+ |                                          |  |             |  |  |            |
			//     |----------|                      | |                        registry          |  |             |  |  |            |
			//     :          :                      V V                +------------+--------+   |  |             V  V  V            |
			//                               +----------+-------+       |     Key    | Value  |   |  |    +----------+------------+   |
			//                               |   Key    | Value |       |------------|--------|   |  |    |   Key    |   Value    |   |
			//                               |----------|-------|       |  regName   | table  |---+  |    |----------|------------|   |
			//                               | "__name" |regName|       |------------|--------|      |    | "__name" |superRegName|   |
			//                               +----------+-------+       |superRegName| table  |------+    |----------|------------|   |
			//                                     metatable            |------------|--------|           |"__index" |   table    |---+
			//                                                          :            :        :           |----------|------------|
			//                                                                                            | "__gc"   |  c-func1   |
			//                                                                                            |----------|------------|
			//                                                                                            |"GetValue"|  c-func2   |
			//                                                                                            +----------+------------+
			//                                                                                                   metatable
			luaL_getmetatable(L, superRegName);

			//        stack
			//     +----------+
			//   -1|   nil    |
			//     |----------|
			//   -2| table S  |---------------------------------------------------------------------------------------+
			//     |----------|                        +------------------------------------------+  +-------------+  |  +------------+
			//   -3| table    |----------------------+ |                                          |  |             |  |  |            |
			//     |----------|                      | |                        registry          |  |             |  |  |            |
			//     :          :                      V V                +------------+--------+   |  |             V  V  V            |
			//                               +----------+-------+       |     Key    | Value  |   |  |    +----------+------------+   |
			//                               |   Key    | Value |       |------------|--------|   |  |    |   Key    |   Value    |   |
			//                               |----------|-------|       |  regName   | table  |---+  |    |----------|------------|   |
			//                               | "__name" |regName|       |------------|--------|      |    | "__name" |superRegName|   |
			//                               +----------+-------+       |superRegName| table  |------+    |----------|------------|   |
			//                                     metatable            |------------|--------|           |"__index" |   table    |---+
			//                                                          :            :        :           |----------|------------|
			//                                                                                            | "__gc"   |  c-func1   |
			//                                                                                            |----------|------------|
			//                                                                                            |"GetValue"|  c-func2   |
			//                                                                                            +----------+------------+
			//                                                                                                   metatable
			lua_pushnil(L);

			//         stack
			//     +------------+
			//   -1|superRegName|
			//     |------------|
			//   -2|  "__name"  |
			//     |------------|
			//   -3|  table S   |---------------------------------------------------------------------------------------+
			//     |------------|                        +------------------------------------------+  +-------------+  |  +------------+
			//   -4|  table     |----------------------+ |                                          |  |             |  |  |            |
			//     |------------|                      | |                        registry          |  |             |  |  |            |
			//     :            :                      V V                +------------+--------+   |  |             V  V  V            |
			//                                 +----------+-------+       |     Key    | Value  |   |  |    +----------+------------+   |
			//                                 |   Key    | Value |       |------------|--------|   |  |    |   Key    |   Value    |   |
			//                                 |----------|-------|       |  regName   | table  |---+  |    |----------|------------|   |
			//                                 | "__name" |regName|       |------------|--------|      |    | "__name" |superRegName|   |
			//                                 +----------+-------+       |superRegName| table  |------+    |----------|------------|   |
			//                                                            |------------|--------|           |"__index" |   table    |---+
			//                                                            :            :        :           |----------|------------|
			//                                                                                              | "__gc"   |  c-func1   |
			//                                                                                              |----------|------------|
			//                                                                                              |"GetValue"|  c-func2   |
			//                                                                                              +----------+------------+
			//                                                                                                     metatable
			while (lua_next(L, -2) != 0)
			{
				//         stack
				//     +------------+
				//   -1|  "__name"  |
				//     |------------|
				//   -2|superRegName|
				//     |------------|
				//   -3|  "__name"  |
				//     |------------|
				//   -4|  table S   |---------------------------------------------------------------------------------------+
				//     |------------|                        +------------------------------------------+  +-------------+  |  +------------+
				//   -5|  table     |----------------------+ |                                          |  |             |  |  |            |
				//     |------------|                      | |                        registry          |  |             |  |  |            |
				//     :            :                      V V                +------------+--------+   |  |             V  V  V            |
				//                                 +----------+-------+       |     Key    | Value  |   |  |    +----------+------------+   |
				//                                 |   Key    | Value |       |------------|--------|   |  |    |   Key    |   Value    |   |
				//                                 |----------|-------|       |  regName   | table  |---+  |    |----------|------------|   |
				//                                 | "__name" |regName|       |------------|--------|      |    | "__name" |superRegName|   |
				//                                 +----------+-------+       |superRegName| table  |------+    |----------|------------|   |
				//                                                            |------------|--------|           |"__index" |   table    |---+
				//                                                            :            :        :           |----------|------------|
				//                                                                                              | "__gc"   |  c-func1   |
				//                                                                                              |----------|------------|
				//                                                                                              |"GetValue"|  c-func2   |
				//                                                                                              +----------+------------+
				//                                                                                                     metatable
				lua_pushvalue(L, -2);

				//         stack
				//     +------------+
				//   -1|superRegName|
				//     |------------|
				//   -2|  "__name"  |
				//     |------------|
				//   -3|  "__name"  |
				//     |------------|
				//   -4|  table S   |---------------------------------------------------------------------------------------+
				//     |------------|                        +------------------------------------------+  +-------------+  |  +------------+
				//   -5|  table     |----------------------+ |                                          |  |             |  |  |            |
				//     |------------|                      | |                        registry          |  |             |  |  |            |
				//     :            :                      V V                +------------+--------+   |  |             V  V  V            |
				//                                 +----------+-------+       |     Key    | Value  |   |  |    +----------+------------+   |
				//                                 |   Key    | Value |       |------------|--------|   |  |    |   Key    |   Value    |   |
				//                                 |----------|-------|       |  regName   | table  |---+  |    |----------|------------|   |
				//                                 | "__name" |regName|       |------------|--------|      |    | "__name" |superRegName|   |
				//                                 +----------+-------+       |superRegName| table  |------+    |----------|------------|   |
				//                                                            |------------|--------|           |"__index" |   table    |---+
				//                                                            :            :        :           |----------|------------|
				//                                                                                              | "__gc"   |  c-func1   |
				//                                                                                              |----------|------------|
				//                                                                                              |"GetValue"|  c-func2   |
				//                                                                                              +----------+------------+
				//                                                                                                     metatable
				lua_insert(L, -2);

				//         stack
				//     +------------+
				//   -1|  "__name"  |
				//     |------------|
				//   -2|  table S   |--------------------------------------------------------------------------------------------+
				//     |------------|                        +-----------------------------------------------+  +-------------+  |  +------------+
				//   -3|  table     |----------------------+ |                                               |  |             |  |  |            |
				//     |------------|                      | |                             registry          |  |             |  |  |            |
				//     :            :                      V V                     +------------+--------+   |  |             V  V  V            |
				//                                 +----------+------------+       |     Key    | Value  |   |  |    +----------+------------+   |
				//                                 |   Key    |   Value    |       |------------|--------|   |  |    |   Key    |   Value    |   |
				//                                 |----------|------------|       |  regName   | table  |---+  |    |----------|------------|   |
				//                                 | "__name" |superRegName|       |------------|--------|      |    | "__name" |superRegName|   |
				//                                 +----------+------------+       |superRegName| table  |------+    |----------|------------|   |
				//                                                                 |------------|--------|           |"__index" |   table    |---+
				//                                                                 :            :        :           |----------|------------|
				//                                                                                                   | "__gc"   |  c-func1   |
				//                                                                                                   |----------|------------|
				//                                                                                                   |"GetValue"|  c-func2   |
				//                                                                                                   +----------+------------+
				//                                                                                                          metatable
				lua_settable(L, -5);
			}
			//         stack
			//     +------------+
			//   -1|  table S   |--------------------------------------------------------------------------------------------+
			//     |------------|                        +-----------------------------------------------+  +-------------+  |  +------------+
			//   -2|  table     |----------------------+ |                                               |  |             |  |  |            |
			//     |------------|                      | |                             registry          |  |             |  |  |            |
			//     :            :                      V V                     +------------+--------+   |  |             V  V  V            |
			//                                 +----------+------------+       |     Key    | Value  |   |  |    +----------+------------+   |
			//                                 |   Key    |   Value    |       |------------|--------|   |  |    |   Key    |   Value    |   |
			//                                 |----------|------------|       |  regName   | table  |---+  |    |----------|------------|   |
			//                                 | "__name" |superRegName|       |------------|--------|      |    | "__name" |superRegName|   |
			//                                 |----------|------------|       |superRegName| table  |------+    |----------|------------|   |
			//                                 |"__index" |   table    |---+   |------------|--------|           |"__index" |   table    |---+
			//                                 |----------|------------|   |   :            :        :           |----------|------------|
			//                                 | "__gc"   |  c-func1   |   |                                     | "__gc"   |  c-func1   |
			//                                 |----------|------------|   :                                     |----------|------------|
			//                                 |"GetValue"|  c-func2   |                                         |"GetValue"|  c-func2   |
			//                                 +----------+------------+                                         +----------+------------+
			//                                                                                                          metatable

			//         stack
			//     +------------+                        +-----------------------------------------------+
			//   -1|  table     |----------------------+ |                                               |
			//     |------------|                      | |                             registry          |
			//     :            :                      V V                     +------------+--------+   |
			//                                 +----------+------------+       |     Key    | Value  |   |
			//                                 |   Key    |   Value    |       |------------|--------|   |
			//                                 |----------|------------|       |  regName   | table  |---+
			//                                 | "__name" |superRegName|       |------------|--------|
			//                                 |----------|------------|       |superRegName| table  |-- - -
			//                                 |"__index" |   table    |---+   |------------|--------|
			//                                 |----------|------------|   |   :            :        :
			//                                 | "__gc"   |  c-func1   |   |
			//                                 |----------|------------|   :
			//                                 |"GetValue"|  c-func2   |
			//                                 +----------+------------+
			//                                        metatable
			lua_pop(L, 1);

			//         stack
			//     +------------+                        +-----------------------------------------------+
			//   -1|  table     |----------------------+ |                                               |
			//     |------------|                      | |                             registry          |
			//     :            :                      V V                     +------------+--------+   |
			//                                 +----------+------------+       |     Key    | Value  |   |
			//                                 |   Key    |   Value    |       |------------|--------|   |
			//                                 |----------|------------|       |  regName   | table  |---+
			//                                 | "__name" |  regName   |       |------------|--------|
			//                                 |----------|------------|       |superRegName| table  |-- - -
			//                                 |"__index" |   table    |---+   |------------|--------|
			//                                 |----------|------------|   |   :            :        :
			//                                 | "__gc"   |  c-func1   |   |
			//                                 |----------|------------|   :
			//                                 |"GetValue"|  c-func2   |
			//                                 +----------+------------+
			//                                        metatable
			Sticklib::set_lvalue_to_table(L, "__name", regName, false);
		}

		// void lua_pushvalue (lua_State * L, int index);
		// 指定されたインデックスの要素のコピーをスタックに積みます。
		//         stack
		//     +------------+
		//   -1|  table     |------------------------+
		//     |------------|                        | +---------------------------------------------+
		//   -2|  table     |----------------------+ | |                                             |
		//     |------------|                      | | |                           registry          |
		//     :            :                      V V V                   +------------+--------+   |
		//                                 +----------+------------+       |     Key    | Value  |   |
		//                                 |   Key    |   Value    |       |------------|--------|   |
		//                                 |----------|------------|       |  regName   | table  |---+
		//                                 | "__name" |  regName   |       |------------|--------|
		//                                 |----------|------------|       |superRegName| table  |-- - -
		//                                 |"__index" |   table    |---+   |------------|--------|
		//                                 |----------|------------|   |   :            :        :
		//                                 | "__gc"   |  c-func1   |   |
		//                                 |----------|------------|   :
		//                                 |"GetValue"|  c-func2   |
		//                                 +----------+------------+
		//                                        metatable
		lua_pushvalue(L, -1);

		//         stack
		//     +------------+                        +-----------------------------------------------+
		//   -1|  table     |----------------------+ | +---------------+                             |
		//     |------------|                      | | |               |           registry          |
		//     :            :                      V V V               |   +------------+--------+   |
		//                                 +----------+------------+   |   |     Key    | Value  |   |
		//                                 |   Key    |   Value    |   |   |------------|--------|   |
		//                                 |----------|------------|   |   |  regName   | table  |---+
		//                                 | "__name" |  regName   |   |   |------------|--------|
		//                                 |----------|------------|   |   |superRegName| table  |-- - -
		//                                 |"__index" |   table    |---+   |------------|--------|
		//                                 |----------|------------|       :            :        :
		//                                 | "__gc"   |  c-func1   |
		//                                 |----------|------------|
		//                                 |"GetValue"|  c-func2   |
		//                                 +----------+------------+
		//                                        metatable
		lua_setfield(L, -2, "__index");

		// void luaL_setfuncs (lua_State * L, const luaL_Reg *l, int nup);
		// 配列 l (luaL_Reg を参照) のすべての関数を、スタックトップのテーブル (ただし上位値がある場合はその下です。下記参照) に登録します。
		// l={"__gc"->c-func21, "SetValue"->c-func22}
		//
		//         stack
		//     +------------+                        +-----------------------------------------------+
		//   -1|  table     |----------------------+ | +---------------+                             |
		//     |------------|                      | | |               |           registry          |
		//     :            :                      V V V               |   +------------+--------+   |
		//                                 +----------+------------+   |   |     Key    | Value  |   |
		//                                 |   Key    |   Value    |   |   |------------|--------|   |
		//                                 |----------|------------|   |   |  regName   | table  |---+
		//                                 | "__name" |  regName   |   |   |------------|--------|
		//                                 |----------|------------|   |   |superRegName| table  |-- - -
		//                                 |"__index" |   table    |---+   |------------|--------|
		//                                 |----------|------------|       :            :        :
		//                                 | "__gc"   |  c-func21  |
		//                                 |----------|------------|
		//                                 |"GetValue"|  c-func1   |
		//                                 |----------|------------|
		//                                 |"SetValue"|  c-func22  |
		//                                 +----------+------------+
		//                                        metatable
		luaL_setfuncs(L, methods, 0);


		//         stack
		//     |            |
		//     |            |                        +-----------------------------------------------+
		//     :            :                        | +---------------+                             |
		//                                           | |               |           registry          |
		//                                           V V               |   +------------+--------+   |
		//                                 +----------+------------+   |   |     Key    | Value  |   |
		//                                 |   Key    |   Value    |   |   |------------|--------|   |
		//                                 |----------|------------|   |   |  regName   | table  |---+
		//                                 | "__name" |  regName   |   |   |------------|--------|
		//                                 |----------|------------|   |   |superRegName| table  |-- - -
		//                                 |"__index" |   table    |---+   |------------|--------|
		//                                 |----------|------------|       :            :        :
		//                                 | "__gc"   |  c-func21  |
		//                                 |----------|------------|
		//                                 |"GetValue"|  c-func1   |
		//                                 |----------|------------|
		//                                 |"SetValue"|  c-func22  |
		//                                 +----------+------------+
		//                                        metatable
		lua_pop(L, 1);
	}

	static void set_functions(lua_State * L, const luaL_Reg methods[]) noexcept(false)
	{
		//                 Premise.
		//        stack
		//     +----------+
		//   -1| table A  |--------------+
		//     |----------|              |
		//     :          :              V
		//                       +----------+-------+
		//                       |   Key    | Value |
		//                       |----------|-------|
		//                       | "name 1" |c-func0|
		//                       |----------|-------|
		//                       | "name 2" |c-func1|
		//                       +----------+-------+

		for (auto l = methods; l->name != nullptr; l++)
		{
			//           if l->name = "name 1"                |           if l->name = "name 3"
			//                                                |
			//        stack                                   |        stack
			//     +----------+                               |     +----------+
			//   -1| c-func0  |                               |   -1|   nil    |
			//     |----------|                               |     |----------|
			//   -2| table A  |--------------+                |   -2| table A  |--------------+
			//     |----------|              |                |     |----------|              |
			//     :          :              V                |     :          :              V
			//                       +----------+-------+     |                       +----------+-------+
			//                       |   Key    | Value |     |                       |   Key    | Value |
			//                       |----------|-------|     |                       |----------|-------|
			//                       | "name 1" |c-func0|     |                       | "name 1" |c-func0|
			//                       |----------|-------|     |                       |----------|-------|
			//                       | "name 2" |c-func1|     |                       | "name 2" |c-func1|
			//                       +----------+-------+     |                       +----------+-------+
			auto result = lua_getfield(L, -1, l->name);

			if (!lua_isnil(L, -1))
			{
				//           if l->name = "name 1"
				//
				//        stack
				//     +----------+
				//     | table A  |--------------+
				//     |----------|              |
				//     :          :              V
				//                       +----------+-------+
				//                       |   Key    | Value |
				//                       |----------|-------|
				//                       | "name 1" |c-func0|
				//                       |----------|-------|
				//                       | "name 2" |c-func1|
				//                       +----------+-------+
				lua_pop(L, 1);
				throw std::invalid_argument(
					std::string("Try to register same name method twice:") + l->name
					);
			}
			//           if l->name = "name 3"
			//
			//        stack
			//     +----------+
			//     | table A  |--------------+
			//     |----------|              |
			//     :          :              V
			//                       +----------+-------+
			//                       |   Key    | Value |
			//                       |----------|-------|
			//                       | "name 1" |c-func0|
			//                       |----------|-------|
			//                       | "name 2" |c-func1|
			//                       +----------+-------+
			lua_pop(L, 1);

			//           if l->name = "name 3"
			//
			//        stack
			//     +----------+
			//     | c-func2  |
			//     |----------|
			//     | table A  |--------------+
			//     |----------|              |
			//     :          :              V
			//                       +----------+-------+
			//                       |   Key    | Value |
			//                       |----------|-------|
			//                       | "name 1" |c-func0|
			//                       |----------|-------|
			//                       | "name 2" |c-func1|
			//                       +----------+-------+
			lua_pushcclosure(L, l->func, 0);

			//        stack
			//     +----------+
			//     | table A  |--------------+
			//     |----------|              |
			//     :          :              V
			//                       +----------+-------+
			//                       |   Key    | Value |
			//                       |----------|-------|
			//                       | "name 1" |c-func0|
			//                       |----------|-------|
			//                       | "name 2" |c-func1|
			//                       |----------|-------|
			//                       | "name 3" |c-func2| <-- set
			//                       +----------+-------+
			lua_setfield(L, -2, l->name);
		}
	}

	template<typename T>
	static void set_lvalue_to_table(lua_State * L, const char * name, const T & value, bool own)
	{
		//                 Premise.
		//        stack
		//     +----------+
		//   -1| table A  |--------------+
		//     |----------|              |
		//     :          :              V
		//                       +----------+-------+
		//                       |   Key    | Value |
		//                       |----------|-------|
		//                       :          :       :

		//        stack
		//     +----------+
		//   -1|  value   |
		//     |----------|
		//   -2| table A  |--------------+
		//     |----------|              |
		//     :          :              V
		//                       +----------+-------+
		//                       |   Key    | Value |
		//                       |----------|-------|
		//                       :          :       :
		push_lvalue<T>(L, value, own);

		//        stack
		//     +----------+
		//     | table A  |--------------+
		//     |----------|              |
		//     :          :              V
		//                       +----------+-------+
		//                       |   Key    | Value |
		//                       |----------|-------|
		//                       :          :       :
		//                       |----------|-------|
		//                       |  name    | value | <-- set
		//                       |----------|-------|
		//                       :          :       :
		lua_setfield(L, -2, name);
	}

//----- 21.05.26 Fukushiro M. 削除始 ()-----
//	template<typename T>
//	static void set_classobject_to_table(lua_State * L, const char * name, T * object, bool own)
//	{
//		//                 Premise.
//		//        stack
//		//     +----------+
//		//   -1| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//
//
//		//                                metatable
//		//        stack                 +-----------+   +----------------------------------------+
//		//     |----------|             |           |   |                       registry         |
//		//   -1| userdata |---------+   |           V   V                    +-------+-------+   |
//		//     |----------|         |   |   +----------+--------+            | Key   | Value |   |
//		//   -2| table A  |         |   |   |   Key    | Value  |            |-------|-------|   |
//		//     |----------|         |   |   |----------|--------|            |regName| table |---+
//		//     :          :         |   |   |  "__gc"  |c++func1<---+        +---A---+-------+
//		//                          |   |   +----------|--------+   |        :   |   :       :
//		//                          |   |   | "xxxxx"  |c++func2|   |            |
//		//                          |   |   +----------|--------+   |       metatable_name
//		//                          |   |   :          :        :   |
//		//                          V   |                           |
//		//     +----------+       +--------------------+            |
//		//     |  object  |<------|StickInstanceWrapper|      c++ Destructor
//		//     +----------+       |  class instance    |
//		//                        +--------------------+
//		//
//		push_classobject<T>(L, object, own);
//
//		//        stack
//		//     +----------+
//		//     | table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+--------+
//		//                       |   Key    | Value  |
//		//                       |----------|--------|
//		//                       :          :        :
//		//                       |----------|--------|
//		//                       |  name    |userdata| <-- set
//		//                       |----------|--------|
//		//                       :          :        :
//		lua_setfield(L, -2, name);
//	}
//
//
//#if 0
//	static void setnumber(lua_State * L, const char * name, lua_Number value)
//	{
//		//                 Premise.
//		//        stack
//		//     +----------+
//		//   -1| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//
//		//        stack
//		//     +----------+
//		//   -1|  value   |
//		//     |----------|
//		//   -2| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_pushnumber(L, value);
//
//		//        stack
//		//     +----------+
//		//     | table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		//                       |----------|-------|
//		//                       |  name    | value | <-- set
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_setfield(L, -2, name);
//	}
//
//	static void setinteger(lua_State * L, const char * name, lua_Integer value)
//	{
//		//                 Premise.
//		//        stack
//		//     +----------+
//		//   -1| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//
//		//        stack
//		//     +----------+
//		//   -1|  value   |
//		//     |----------|
//		//   -2| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_pushinteger(L, value);
//
//		//        stack
//		//     +----------+
//		//     | table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		//                       |----------|-------|
//		//                       |  name    | value | <-- set
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_setfield(L, -2, name);
//	}
//
//	static void setboolean(lua_State * L, const char * name, int value)
//	{
//		//                 Premise.
//		//        stack
//		//     +----------+
//		//   -1| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//
//		//        stack
//		//     +----------+
//		//   -1|  value   |
//		//     |----------|
//		//   -2| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_pushboolean(L, value);
//
//		//        stack
//		//     +----------+
//		//     | table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		//                       |----------|-------|
//		//                       |  name    | value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_setfield(L, -2, name);
//	}
//
//	static void setstring(lua_State * L, const char * name, const char * value)
//	{
//		//                 Premise.
//		//        stack
//		//     +----------+
//		//   -1| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//
//		//        stack
//		//     +----------+
//		//   -1|  value   |
//		//     |----------|
//		//   -2| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_pushstring(L, value);
//
//		//        stack
//		//     +----------+
//		//     | table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		//                       |----------|-------|
//		//                       |  name    | value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_setfield(L, -2, name);
//	}
//
//	static void setlightuserdata(lua_State * L, const char * name, void * value)
//	{
//		//                 Premise.
//		//        stack
//		//     +----------+
//		//   -1| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//
//		//        stack
//		//     +----------+
//		//   -1|  value   |
//		//     |----------|
//		//   -2| table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_pushlightuserdata(L, value);
//
//		//        stack
//		//     +----------+
//		//     | table A  |--------------+
//		//     |----------|              |
//		//     :          :              V
//		//                       +----------+-------+
//		//                       |   Key    | Value |
//		//                       |----------|-------|
//		//                       :          :       :
//		//                       |----------|-------|
//		//                       |  name    | value |
//		//                       |----------|-------|
//		//                       :          :       :
//		lua_setfield(L, -2, name);
//	}
//#endif//0
//----- 21.05.26 Fukushiro M. 削除終 ()-----

};

