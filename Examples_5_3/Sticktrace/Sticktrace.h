// Sticktrace.h
//

#pragma once

#include <string>
#include <codecvt>
#include <unordered_set>
//#include <locale>
#include "Sticklib.h"
#include "Stickrun.h"

//
//     +---------------+
//     | LuaStick.exe  |
//     |               |    Loads
//     |  LuaStick     |<-----------------------------------------+
//     |               |                                          |
//     +---------------+                                          |
//             |                                                  |
//             |               +----------------------------------|---------------+
//             |               |  Main Application .exe           |               |
//             |               |                                  |               |
//             |               |    +--------------+     +-------------------+    |
//             |               |    | xxx.cpp      |     | yyy.h             |    |
//             |               |    |              |     |                   |    |
//             |   Generates   |    | App and Lua  |     | Defines exporting |    |
//             +------------------->| binding      |     | classes and       |    |
//                             |    | source code  |     |  functions        |    |
//                             |    +--------------+     +-------------------+    |
//                             |         |    A                                   |
//                             |         |    |                                   |
//                             |         |    |                                   |
//                             |         V    |                                   |
//                             |      +-----------+         +--------------+      |      +------------------------+
//                             |      | lua.lib   |         | Sticktrace.h |      |      | Sticktrace.dll         |
//                             |      |           |-------->|              |------------>|                        |
//                             |      |    Lua    |         |              |      |      |  Sticktrace Window     |
//                             |      |           |<--------|              |<------------|                        |
//                             |      +-----------+         +--------------+      |      |                        |
//                             |                                                  |      |                        |
//                             +--------------------------------------------------+      +------------------------+
//
//

using LuaLineHookFunc = void(*)(lua_State* luaState, lua_Debug* luaDebug);

enum class SticktraceCommand
{
	NONE,
	GET_VARIABLE,
	RESUME,
	STOP,
	PROCEED_NEXT,
};

class Stickutil
{
public:
	static void SkipSpace(const char*& cpStr)
	{
		for (;;)
		{
			switch (*cpStr)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				cpStr++;
				break;
			default:
				return;
			}
		}
	} // SkipSpace.

	static void Serialize(std::string & data, const std::string & v)
	{
		data += std::to_string(v.length()) + ' ';
		data += v + ' ';
	}

	static void Serialize(std::string & data, const std::vector<std::string>& v)
	{
		auto strSize = std::to_string(v.size());
		data += strSize + ' ';
		for (const auto & i : v)
			Serialize(data, i);
	}

	static void Serialize(std::string & data, const std::unordered_set<std::string>& v)
	{
		auto strSize = std::to_string(v.size());
		data += strSize + ' ';
		for (const auto & i : v)
			Serialize(data, i);
	}

	static void Serialize(std::string & data, const std::tuple<std::string, std::string, std::string, std::string, std::string>& v)
	{
		Serialize(data, std::get<0>(v));
		Serialize(data, std::get<1>(v));
		Serialize(data, std::get<2>(v));
		Serialize(data, std::get<3>(v));
		Serialize(data, std::get<4>(v));
	}

	static void Serialize(std::string & data, const std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>>& v)
	{
		auto strSize = std::to_string(v.size());
		data += strSize + ' ';
		for (const auto & i : v)
			Serialize(data, i);
	}

	static void Unserialize(std::string & str, const char*& data)
	{
		const auto size = std::strtol(data, (char**)&data, 10);
		data++;
		str.assign(data, data + size);
		data += size + 1;
	}

	static void Unserialize(std::vector<std::string>& v, const char*& data)
	{
		const auto size = std::strtol(data, (char**)&data, 10);
		data++;
		std::string str;
		for (int i = 0; i != size; i++)
		{
			Unserialize(str, data);
			v.insert(v.end(), str);
		}
	}

	static void Unserialize(std::unordered_set<std::string>& v, const char*& data)
	{
		const auto size = std::strtol(data, (char**)&data, 10);
		data++;
		std::string str;
		for (int i = 0; i != size; i++)
		{
			Unserialize(str, data);
			v.insert(v.end(), str);
		}
	}

	static void Unserialize(std::tuple<std::string, std::string, std::string, std::string, std::string>& v, const char*& data)
	{
		Unserialize(std::get<0>(v), data);
		Unserialize(std::get<1>(v), data);
		Unserialize(std::get<2>(v), data);
		Unserialize(std::get<3>(v), data);
		Unserialize(std::get<4>(v), data);
	}

	static void Unserialize(std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>>& v, const char*& data)
	{
		const auto size = std::strtol(data, (char**)&data, 10);
		data++;
		std::tuple<std::string, std::string, std::string, std::string, std::string> elem;
		for (int i = 0; i != size; i++)
		{
			Unserialize(elem, data);
			v.insert(v.end(), elem);
		}
	}
}; // class Stickutil.

#if !defined(_STICKTRACE)

class Stickobject;
class Sticktrace;

class __declspec(dllimport) SticktraceWindow
{
	friend class Sticktrace;
private:
	static SticktraceWindow* New();
	static void Delete(SticktraceWindow* mtw);
	SticktraceWindow() = delete;
	~SticktraceWindow() = delete;
	bool Show(bool show);
	bool SetSource(const std::string& name, const std::string& source);
	bool IsDebugMode();
	bool IsBreakpoint(const char* name, int lineIndex);
//----- 17.10.20 Fukushiro M. 削除始 ()-----
//	bool IsSuspended();
//	int GetMode();
//----- 17.10.20 Fukushiro M. 削除終 ()-----
	bool OnSuspended();
	bool OnResumed();
	bool Jump(const char * name, int lineIndex);
	bool OnStart();
	bool OnStop();
	bool OutputError(const char* message);
	bool OutputDebug(const char* message);
	bool SetWatch(const std::string& data);
	SticktraceCommand GetCommand(std::string & param, uint32_t waitMilliseconds);
}; // class __declspec(dllimport) SticktraceWindow.


class Sticktrace;

template<typename T> struct _AnyValue
{
	enum Type
	{
		NONE = 0,
		NIL,
		BOOLEAN,
		LIGHTUSERDATA,
		NUMBER,
		STRING,
		TABLE,
		FUNCTION,
		USERDATA,
		THREAD,
		TOKEN,
	};

public:
	class Move
	{
	public:
		Move(_AnyValue & v)
			: m_value(&v)
		{}
		~Move()
		{
			m_value = nullptr;
		}
	private:
		friend struct _AnyValue;
		_AnyValue* m_value;
	};

	_AnyValue()
		: type(_AnyValue::NONE)
		, num(0.0)
	{}

	_AnyValue(_AnyValue & v)
		: _AnyValue()
	{
		type = v.type;
		str = v.str;
		num = v.num;
		obj = Stickobject::Move(v.obj);
		v.type = _AnyValue::NONE;
		v.str.clear();
		v.num = 0.0;
	}

	_AnyValue(Move && v) = delete;

	_AnyValue(Stickobject & o)
		:_AnyValue()
	{
		Set(o);
	}

	_AnyValue(const boolean & b)
		: _AnyValue()
	{
		type = _AnyValue::BOOLEAN;
		num = b ? 1.0 : 0.0;
	}

	_AnyValue(const double & n)
		: _AnyValue()
	{
		type = _AnyValue::NUMBER;
		num = n;
	}

	_AnyValue(const char* s)
		: _AnyValue()
	{
		type = _AnyValue::STRING;
		str = s;
	}

	~_AnyValue()
	{
		Clear();
	}

	void Clear()
	{
		type = _AnyValue::NONE;
		str.clear();
		num = 0.0;
		obj.Clear();
	}

	_AnyValue & operator = (const _AnyValue && v) = delete;
	_AnyValue & operator = (Move && v)
	{
		Clear();
		type = v.m_value->type;
		str = v.m_value->str;
		num = v.m_value->num;
		obj = Stickobject::Move(v.m_value->obj);
		v.m_value->type = _AnyValue::NONE;
		v.m_value->str.clear();
		v.m_value->num = 0.0;
		return *this;
	}

	void Set(Stickobject & o)
	{
		Clear();
		obj = Stickobject::Move(o);
		if (obj.Isnull())
		{
			type = _AnyValue::NONE;
		}
		else
		{
			switch (obj.GetType())
			{
			case LUA_TNONE:
				type = _AnyValue::NONE;
				break;
			case LUA_TNIL:
				type = _AnyValue::NIL;
				break;
			case LUA_TBOOLEAN:
				type = _AnyValue::BOOLEAN;
				num = obj.GetBoolean();
				break;
			case LUA_TLIGHTUSERDATA:
				type = _AnyValue::LIGHTUSERDATA;
				break;
			case LUA_TNUMBER:
				type = _AnyValue::NUMBER;
				num = obj.GetNumber();
				break;
			case LUA_TSTRING:
				type = _AnyValue::STRING;
				str = obj.GetString();
				break;
			case LUA_TTABLE:
				type = _AnyValue::TABLE;
				break;
			case LUA_TFUNCTION:
				type = _AnyValue::FUNCTION;
				break;
			case LUA_TUSERDATA:
				type = _AnyValue::USERDATA;
				break;
			case LUA_TTHREAD:
				type = _AnyValue::THREAD;
				break;
			default:
				type = _AnyValue::NONE;
				break;
			}
		}
	} // Set(Stickobject & o)

	void Set(const boolean & b)
	{
		Clear();
		type = _AnyValue::BOOLEAN;
		num = b ? 1.0 : 0.0;
	}

	void Set(const double & n)
	{
		Clear();
		type = _AnyValue::NUMBER;
		num = n;
	}

	void Set(const char* s)
	{
		Clear();
		type = _AnyValue::STRING;
		str = s;
	}

	void SetNil()
	{
		Clear();
		type = _AnyValue::NIL;
	}

	void Add(const _AnyValue& v)
	{
		if (type == _AnyValue::NUMBER && v.type == _AnyValue::NUMBER)
			num += v.num;
		else
			throw std::runtime_error("error!");
	}

	void Subtract(const _AnyValue& v)
	{
		if (type == _AnyValue::NUMBER && v.type == _AnyValue::NUMBER)
			num -= v.num;
		else
			throw std::runtime_error("error!");
	}

	void Multiple(const _AnyValue& v)
	{
		if (type == _AnyValue::NUMBER && v.type == _AnyValue::NUMBER)
			num *= v.num;
		else
			throw std::runtime_error("error!");
	}

	void Divide(const _AnyValue& v)
	{
		if (type == _AnyValue::NUMBER && v.type == _AnyValue::NUMBER)
			num /= v.num;
		else
			throw std::runtime_error("error!");
	}

	void Remaind(const _AnyValue& v)
	{
		if (type == _AnyValue::NUMBER && v.type == _AnyValue::NUMBER)
			num = num - floor(num / v.num) * v.num;
		else
			throw std::runtime_error("error!");
	}

	void Power(const _AnyValue& v)
	{
		if (type == _AnyValue::NUMBER && v.type == _AnyValue::NUMBER)
			num = std::pow(num, v.num);
		else
			throw std::runtime_error("error!");
	}

	void Connect(const _AnyValue & v)
	{
		ToString();
		// v.ToString();
		str += v.GetValueAsString();
	}

	void ToString()
	{
		if (type == _AnyValue::NUMBER)
		{
			type = _AnyValue::STRING;
			char buf[20];
			sprintf_s(buf, "%.15g", num);
			str = buf;
		}
		else if (type != _AnyValue::STRING)
		{
			throw std::runtime_error("error!");
		}
	}

	// WString GetValueAsWString() const;

	std::string GetValueAsString() const
	{
		std::string astr;
		switch (type)
		{
		case _AnyValue::NUMBER:
		{
			char buff[30];
			sprintf_s(buff, "%.15g", num);
			astr = buff;
			break;
		}
		case _AnyValue::BOOLEAN:
			astr = (num == 0.0) ? "false" : "true";
			break;
		case _AnyValue::STRING:
			astr = std::string("\"") + str + "\"";
			break;
		}
		return astr;
	}

	std::string GetTypeAsString() const
	{
		switch (type)
		{
		case _AnyValue::NIL:
			return "nil";
		case _AnyValue::BOOLEAN:
			return "boolean";
		case _AnyValue::LIGHTUSERDATA:
			return "lightuserdata";
		case _AnyValue::NUMBER:
			return "number";
		case _AnyValue::STRING:
			return "string";
		case _AnyValue::TABLE:
			return "table";
		case _AnyValue::FUNCTION:
			return "function";
		case _AnyValue::USERDATA:
			return "userdata";
		case _AnyValue::THREAD:
			return "thread";
		case _AnyValue::TOKEN:
			return "word";
		default:
			return "";
		}
	}
	
	void BeginEnum()
	{
		obj.BeginEnum();
	}

	void EndEnum()
	{
		obj.EndEnum();
	}

	bool NextEnum(_AnyValue& key, _AnyValue& value)
	{
		return obj.NextEnum(key, value);
	}

	Type type;
	std::string str;
	double num;
	Stickobject obj;
}; // struct _AnyValue

class Stickobject
{
public:
	class Move
	{
	public:
		Move(Stickobject & v)
			: m_value(&v)
		{}
		~Move()
		{
			m_value = nullptr;
		}
	private:
		friend class Stickobject;
		Stickobject* m_value;
	};

public:
	Stickobject()
		: m_lua_state(nullptr)
		, m_ref(-1)
		, m_type(-1)
	{}
	Stickobject(lua_State* L)
		: Stickobject()
	{
		m_lua_state = L;
		m_type = ::lua_type(m_lua_state, -1);
		//
		//        stack                   stack             registry
		//     |          |            |          |      +-------+--------+
		//     |----------|            |----------|      | Key   | Value  |
		//   -1| valueX   |          -1|any value |      |-------|--------|
		//     |----------|   ==>      |----------|      |       | valueX | <- m_ref
		//   -2|any value |            :          :      +-------+--------+
		//     |----------|                              :       :        :
		//     :          :    
		//
		m_ref = ::luaL_ref(m_lua_state, LUA_REGISTRYINDEX);
	}

	Stickobject(Stickobject & v)
		: Stickobject()
	{
		m_lua_state = v.m_lua_state;
		m_ref = v.m_ref;
		m_type = v.m_type;
		v.m_lua_state = nullptr;
		v.m_ref = -1;
		v.m_type = -1;
	}

	Stickobject(Move && v) = delete;
	/*
		: Stickobject()
	{
		m_lua_state = v.m_value->m_lua_state;
		m_ref = v.m_value->m_ref;
		m_type = v.m_value->m_type;
	}
	*/

	~Stickobject()
	{
		Clear();
	}

	void Clear()
	{
		if (m_lua_state != nullptr)
		{
			::luaL_unref(m_lua_state, LUA_REGISTRYINDEX, m_ref);
			m_lua_state = nullptr;
			m_ref = -1;
			m_type = -1;
		}
	}

	bool Isnull() const
	{
		return (m_lua_state == nullptr);
	}

	double GetNumber()
	{
		if (m_type != LUA_TNUMBER)
			throw std::runtime_error("The argument is not number");
		::lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_ref);
		auto v = ::lua_tonumber(m_lua_state, -1);
		::lua_pop(m_lua_state, 1);
		return v;
	}

	bool GetBoolean()
	{
		if (m_type != LUA_TBOOLEAN)
			throw std::runtime_error("The argument is not number");
		::lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_ref);
		auto v = ::lua_toboolean(m_lua_state, -1);
		::lua_pop(m_lua_state, 1);
		return v ? true : false;
	}

	const char* GetString()
	{
		if (m_type != LUA_TSTRING)
			throw std::runtime_error("The argument is not string");
		::lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_ref);
		auto v = lua_tostring(m_lua_state, -1);
		::lua_pop(m_lua_state, 1);
		return v;
	}

	inline int GetType() const
	{
		return m_type;
	}
	
	Stickobject GetTableValue(const _AnyValue<void>& name)
	{
		if (m_type != LUA_TTABLE)
			return Stickobject();

		//        stack             registry
		//     |          |      +-------+--------+
		//     |----------|      | Key   | Value  |
		//   -1| tableX   |      |-------|--------|
		//     |----------|      |       | tableX | <- m_ref
		//     :          :      |-------|--------|
		//                       :       :        :
		::lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_ref);

		//        stack         +------------+
		//     |          |     |            |
		//     |----------|     |            V
		//   -1| nameY    |     |          tableX
		//     |----------|     |     +-------+--------+
		//   -2| tableX   |-----+     | Key   | Value  |
		//     |----------|           |-------|--------|
		//     :          :           :       :        :
		//                            |-------|--------|
		//                            | nameY | valueZ |
		//                            |-------|--------|
		//                            :       :        :
		switch (name.type)
		{
		case _AnyValue<void>::NUMBER:
			::lua_pushnumber(m_lua_state, name.num);
			break;
		case _AnyValue<void>::STRING:
		case _AnyValue<void>::TOKEN:
			::lua_pushstring(m_lua_state, name.str.c_str());
			break;
		case _AnyValue<void>::BOOLEAN:
			::lua_pushboolean(m_lua_state, name.num ? true : false);
			break;
		default:
			throw std::runtime_error("error!");
		}

		//        stack         +------------+
		//     |          |     |            |
		//     |----------|     |            V
		//   -1| valueZ   |     |          tableX
		//     |----------|     |     +-------+--------+
		//   -2| tableX   |-----+     | Key   | Value  |
		//     |----------|           |-------|--------|
		//     :          :           :       :        :
		//                            |-------|--------|
		//                            | nameY | valueZ |
		//                            |-------|--------|
		//                            :       :        :
		::lua_rawget(m_lua_state, -2);

		//        stack
		//     |          |
		//     |----------|
		//   -1| valueZ   |
		//     |----------|
		//     :          :
		lua_remove(m_lua_state, -2);

		return Stickobject(m_lua_state);
	}

	void BeginEnum()
	{
		if (m_type != LUA_TTABLE) return;
		//        stack             registry
		//     |          |      +-------+--------+
		//     |----------|      | Key   | Value  |
		//   -1| tableX   |      |-------|--------|
		//     |----------|      |       | tableX | <- m_ref
		//     :          :      |-------|--------|
		//                       :       :        :
		::lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_ref);

		//        stack         +------------+
		//     |          |     |            |
		//     |----------|     |            V
		//   -1|   nil    |     |          tableX
		//     |----------|     |     +-------+--------+
		//   -2| tableX   |-----+     | Key   | Value  |
		//     |----------|           |-------|--------|
		//     :          :           | nameX | valueX |
		//                            |-------|--------|
		//                            | nameY | valueY |
		//                            |-------|--------|
		//                            | nameZ | valueZ |
		//                            +-------+--------+
		::lua_pushnil(m_lua_state);
	}

	void EndEnum()
	{
		if (m_type != LUA_TTABLE) return;

		//        stack         +------------+                            stack
		//     |          |     |            |                         |          |
		//     |----------|     |            V                --->     |----------|
		//   -1|  nameX   |     |          tableX                      :          :
		//     |----------|     |     +-------+--------+
		//   -2| tableX   |-----+     | Key   | Value  |
		//     |----------|           |-------|--------|
		//     :          :           | nameX | valueX |
		//                            |-------|--------|
		//                            | nameY | valueY |
		//                            |-------|--------|
		//                            | nameZ | valueZ |
		//                            +-------+--------+
		::lua_pop(m_lua_state, 2);
	}

	bool NextEnum(_AnyValue<void>& key, _AnyValue<void>& value)
	{
		if (m_type != LUA_TTABLE) return false;

		//        stack         +------------+                           stack         +------------+
		//     |          |     |            |                        |          |     |            |
		//     |----------|     |            V                        |----------|     |            V
		//   -1|   nil    |     |          tableX                   -1|  nameZ   |     |          tableX
		//     |----------|     |     +-------+--------+              |----------|     |     +-------+--------+
		//   -2| tableX   |-----+     | Key   | Value  |            -2| tableX   |-----+     | Key   | Value  |
		//     |----------|           |-------|--------|              |----------|           |-------|--------|
		//     :          :           | nameX | valueX |              :          :           | nameX | valueX |
		//                            |-------|--------|                                     |-------|--------|
		//                            | nameY | valueY |                                     | nameY | valueY |
		//                            |-------|--------|                                     |-------|--------|
		//                            | nameZ | valueZ |                                     | nameZ | valueZ |
		//                            +-------+--------+                                     +-------+--------+

		//             If lua_next(L, -2) != 0                                If lua_next(L, -2) == 0
		//
		//        stack         +------------+                           stack         +------------+
		//     |          |     |            |                        |          |     |            |
		//     |----------|     |            V                        |----------|     |            V
		//   -1|  valueX  |     |          tableX                   -1| tableX   |-----+          tableX
		//     |----------|     |     +-------+--------+              |----------|           +-------+--------+
		//   -2|  nameX   |     |     | Key   | Value  |              :          :           | Key   | Value  |
		//     |----------|     |     |-------|--------|                                     |-------|--------|
		//   -3| tableX   |-----+     | nameX | valueX |                                     | nameX | valueX |
		//     |----------|           |-------|--------|                                     |-------|--------|
		//     :          :           | nameY | valueY |                                     | nameY | valueY |
		//                            |-------|--------|                                     |-------|--------|
		//                            | nameZ | valueZ |                                     | nameZ | valueZ |
		//                            +-------+--------+                                     +-------+--------+
		if (lua_next(m_lua_state, -2) == 0)
		{
			//        stack         +------------+
			//     |          |     |            |
			//     |----------|     |            V
			//   -1|   nil    |     |          tableX
			//     |----------|     |     +-------+--------+
			//   -2| tableX   |-----+     | Key   | Value  |
			//     |----------|           |-------|--------|
			//     :          :           | nameX | valueX |
			//                            |-------|--------|
			//                            | nameY | valueY |
			//                            |-------|--------|
			//                            | nameZ | valueZ |
			//                            +-------+--------+
			::lua_pushnil(m_lua_state);
			return false;
		}
		//        stack         +------------+                           stack         +------------+
		//     |          |     |            |                        |          |     |            |
		//     |----------|     |            V                        |----------|     |            V
		//   -1|  valueX  |     |          tableX                   -1|  nameX   |     |          tableX
		//     |----------|     |     +-------+--------+              |----------|     |     +-------+--------+
		//   -2|  nameX   |     |     | Key   | Value  |            -2| tableX   |-----+     | Key   | Value  |
		//     |----------|     |     |-------|--------|    --->      |----------|           |-------|--------|
		//   -3| tableX   |-----+     | nameX | valueX |              :          :           | nameX | valueX |
		//     |----------|           |-------|--------|                                     |-------|--------|
		//     :          :           | nameY | valueY |                                     | nameY | valueY |
		//                            |-------|--------|                                     |-------|--------|
		//                            | nameZ | valueZ |                                     | nameZ | valueZ |
		//                            +-------+--------+                                     +-------+--------+
		value.Set(Stickobject(m_lua_state));

		//        stack         +------------+
		//     |          |     |            |
		//     |----------|     |            V
		//   -1|  nameX   |     |          tableX
		//     |----------|     |     +-------+--------+
		//   -2|  nameX   |     |     | Key   | Value  |
		//     |----------|     |     |-------|--------|
		//   -3| tableX   |-----+     | nameX | valueX |
		//     |----------|           |-------|--------|
		//     :          :           | nameY | valueY |
		//                            |-------|--------|
		//                            | nameZ | valueZ |
		//                            +-------+--------+
		::lua_pushvalue(m_lua_state, -1);

		//        stack         +------------+
		//     |          |     |            |
		//     |----------|     |            V
		//   -1|  nameX   |     |          tableX
		//     |----------|     |     +-------+--------+
		//   -2| tableX   |-----+     | Key   | Value  |
		//     |----------|           |-------|--------|
		//     :          :           | nameX | valueX |
		//                            |-------|--------|
		//                            | nameY | valueY |
		//                            |-------|--------|
		//                            | nameZ | valueZ |
		//                            +-------+--------+
		key.Set(Stickobject(m_lua_state));

		return true;
	}


	Stickobject & operator = (const Stickobject && v) = delete;
	Stickobject & operator = (Move && v)
	{
		Clear();
		m_lua_state = v.m_value->m_lua_state;
		m_ref = v.m_value->m_ref;
		m_type = v.m_value->m_type;
		v.m_value->m_lua_state = nullptr;
		v.m_value->m_ref = -1;
		v.m_value->m_type = -1;
		return *this;
	}

private:
	friend class Move;
	lua_State* m_lua_state;
	int m_ref;
	int m_type;
}; // class Stickobject.

using AnyValue = _AnyValue<void>;

class Stickvar
{
public:
	enum TokenType
	{
		TT_NONE = 0,
		TT_STRING,
		TT_NUMBER,
		TT_TOKEN,
		TT_OPERATOR_BEGIN,
		TT_OPE_NEGATIVE,	// -（先頭）
		TT_OPE_ADD,			// +
		TT_OPE_SUBTRACT,	// -
		TT_OPE_MULTIPLE,	// *
		TT_OPE_DIVIDE,		// /
		TT_OPE_REMAIND,		// %
		TT_OPE_POWER,		// ^
		TT_OPE_CONNECT,		// ..
		TT_OPERATOR_END,
		TT_JOINT,			// .
		TT_ARRAY_LEFT,		// [
		TT_ARRAY_RIGHT,		// ]
		TT_PARE_LEFT,		// (
		TT_PARE_RIGHT,		// )
	};
public:
	Stickvar(lua_State* L, lua_Debug* ar)
		: m_lua_state(L)
		, m_lua_debug(ar)
		, m_isFirstLocal(true)
		, m_isFirstGlobal(true)
	{}
	
	/// <summary>
	/// Return the level of operators.
	/// </summary>
	/// <param name="opeType">Type of the ope.</param>
	/// <returns></returns>
	int LevelOf(TokenType opeType) const
	{
		switch (opeType)
		{
		case TT_OPE_NEGATIVE:
			return 7;
		case TT_OPE_ADD:
		case TT_OPE_SUBTRACT:
			return 5;
		case TT_OPE_MULTIPLE:
		case TT_OPE_DIVIDE:
		case TT_OPE_REMAIND:
			return 6;
		case TT_OPE_POWER:
			return 8;
		case TT_OPE_CONNECT:
			return 4;
		}
		throw std::runtime_error("error!");
	} // LevelOf.
	
	Stickobject GetLocalVariable(const std::string & varname)
	{
		if (m_isFirstLocal)
		{
			m_isFirstLocal = false;
			for (int id = 1;; id++)
			{
				// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
				//
				// ┌───────┐
				// │ローカル変数値│<- nで指定されたローカル変数を積む
				// ├───────┤
				// │              │
				// ├───────┤
				// │              │
				// ├───────┤
				// ：              ：
				//
				const char* name = ::lua_getlocal(m_lua_state, m_lua_debug, id);
				if (name == nullptr) break;
				// Records local variable name to id.
				// Two or more same name variable can be exist, so override the name to id hash.
				if (strcmp(name, "(*temporary)") != 0)
					m_localVariableNameToId[name] = id;
				// lua_pop (lua_State *L, int n)
				//
				// ┌───────┐─┬
				// │//////////////│  │
				// ├───────┤  │
				// │//////////////│  │ <- n個の要素を取り除く（n=3の場合）
				// ├───────┤  │
				// │//////////////│  │
				// ├───────┤─┴
				// │              │
				// ├───────┤
				// ：              ：
				//
				::lua_pop(m_lua_state, 1);	// lua_getlocalで積んだ変数を削除
			}
		}
		const auto i = m_localVariableNameToId.find(varname);
		if (i != m_localVariableNameToId.end())
		{
			// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
			//
			// ┌───────┐
			// │ローカル変数値│<- nで指定されたローカル変数を積む
			// ├───────┤
			// │              │
			// ├───────┤
			// │              │
			// ├───────┤
			// ：              ：
			//
			::lua_getlocal(m_lua_state, m_lua_debug, i->second);
			return Stickobject(m_lua_state);
		}
		else
		{
			return Stickobject();
		}
	} // GetLocalVariable.

	Stickobject GetGlobalVariable(const char* varname)
	{
		if (m_isFirstGlobal)
		{
			m_isFirstGlobal = false;
			m_ENV = Stickobject::Move(GetLocalVariable("_ENV"));
			if (m_ENV.Isnull())
			{	//----- if "_ENV" is not defined as the local variable -----
				lua_pushglobaltable(m_lua_state);
				m_ENV = Stickobject::Move(Stickobject(m_lua_state));
			}
			else if (m_ENV.GetType() != LUA_TTABLE)
			{	//----- if "_ENV" is defined as the local variable and its type is not parentTable -----
				m_ENV.Clear();
			}
		}
		if (m_ENV.Isnull())
			return Stickobject();
		return m_ENV.GetTableValue(AnyValue(varname));
	}

	Stickobject GetVariableObject(const std::string & varname)
	{
		auto value = GetLocalVariable(varname);
		if (value.Isnull())
			value = Stickobject::Move(GetGlobalVariable(varname.c_str()));
		return value;
	}
		
	/// <summary>
	/// Extracts one token.
	/// Ex.  B:tokenBegin / E:tokenEnd
	/// Formula = " ABC * 100"
	///             ^B ^E
	/// Formula = " (ABC * 100"
	///            B^^E
	/// Formula = " (ABC * 100"
	///                    ^B ^E
	/// </summary>
	/// <param name="cpTokenBegin">The beginning of the token.</param>
	/// <param name="cpTokenEnd">The end of the token.</param>
	/// <param name="cpFormula">Head point of the formula.</param>
	/// <returns>Token type.</returns>
	TokenType ExtractToken(const char*& cpTokenBegin, const char*& cpTokenEnd, const char* cpFormula)
	{
		// 先頭の空白をスキップ。
		Stickutil::SkipSpace(cpFormula);
		if (cpFormula[0] == '\0') return TT_NONE;
		if ('0' <= cpFormula[0] && cpFormula[0] <= '9')
		{
			cpTokenBegin = cpFormula;
			(void)strtod(cpFormula, (char**)&cpFormula);
			cpTokenEnd = cpFormula;
			return TT_NUMBER;
		}
		else if (
			('a' <= cpFormula[0] && cpFormula[0] <= 'z') ||
			('A' <= cpFormula[0] && cpFormula[0] <= 'Z') ||
			(cpFormula[0] == '_')
			)
		{
			cpTokenBegin = cpFormula;
			cpFormula++;
			while (('a' <= cpFormula[0] && cpFormula[0] <= 'z') ||
				('A' <= cpFormula[0] && cpFormula[0] <= 'Z') ||
				('0' <= cpFormula[0] && cpFormula[0] <= '9') ||
				(cpFormula[0] == '_'))
				cpFormula++;
			cpTokenEnd = cpFormula;
			return TT_TOKEN;
		}
		else if (cpFormula[0] == '\"' || cpFormula[0] == '\'')
		{
			cpTokenBegin = cpFormula;
			const char terminator = cpFormula[0];
			boolean isEsc = false;
			for (cpFormula++;; cpFormula++)
			{
				if (cpFormula[0] == '\0') throw std::runtime_error("error!");
				if (isEsc)
				{
					isEsc = false;
				}
				else
				{
					if (cpFormula[0] == '\\')
						isEsc = true;
					else
						if (cpFormula[0] == terminator)
							break;
				}
			}
			cpFormula++;
			cpTokenEnd = cpFormula;
			return TT_STRING;
		}
		else
		{
			cpTokenBegin = cpFormula;
			cpFormula++;
			cpTokenEnd = cpFormula;
			switch (cpTokenBegin[0])
			{
			case '+':
				return TT_OPE_ADD;			// +
			case '-':
				return TT_OPE_SUBTRACT;		// -
			case '*':
				return TT_OPE_MULTIPLE;		// *
			case '/':
				return TT_OPE_DIVIDE;		// /
			case '%':
				return TT_OPE_REMAIND;		// %
			case '^':
				return TT_OPE_POWER;		// ^
			case '.':
				if (cpFormula[0] == '.')
				{
					cpFormula++;
					cpTokenEnd = cpFormula;
					return TT_OPE_CONNECT;		// ..
				}
				else
				{
					return TT_JOINT;
				}
				break;

			case '[':
				return TT_ARRAY_LEFT;
			case ']':
				return TT_ARRAY_RIGHT;
			case '(':
				return TT_PARE_LEFT;
			case ')':
				return TT_PARE_RIGHT;
			}
		}
		throw std::runtime_error("error!");
	} // ExtractToken.

	/*
	Stickobject GetTableValue(const AnyValue& name, Stickobject const& obj) const
	{
		if (obj.GetType() != LUA_TTABLE) throw std::runtime_error("error!");
		switch (name.type)
		{
		case AnyValue::NUMBER:
			return obj[name.num];
		case AnyValue::STRING:
		case AnyValue::TOKEN:
			return obj[name.v];
		case AnyValue::BOOLEAN:
			return obj[name.num ? true : false];
		default:
			throw std::runtime_error("error!");
		}
	} // FCDdLua::GetTableValue
	*/
		
	/// <summary>
	/// Gets the token value.
	///
	/// 数字から始まる場合は数値と判断。「"」「'」から始まる場合は文字列、英字から始まる場合はワードとする。
	/// </summary>
	/// <param name="tokenType">Type of the token.</param>
	/// <param name="cpTokenBegin">The cp token begin.</param>
	/// <param name="cpTokenEnd">The cp token end.</param>
	/// <returns></returns>
	AnyValue GetTokenValue(TokenType tokenType, const char* cpTokenBegin, const char* cpTokenEnd) const
	{
		AnyValue value;
		switch (tokenType)
		{
		case TT_STRING:
		{
			value.type = AnyValue::STRING;
			boolean isEsc = false;
			for (const char* cp = cpTokenBegin + 1; cp != cpTokenEnd - 1; cp++)
			{
				if (isEsc)
				{
					switch (cp[0])
					{
					case 'n':
						value.str += '\n';
						break;
					case 'r':
						value.str += '\r';
						break;
					case 't':
						value.str += '\t';
						break;
					default:
						value.str += cp[0];
						break;
					}
					isEsc = false;
				}
				else
				{
					if (cp[0] == '\\')
						isEsc = true;
					else
						value.str += cp[0];
				}
			}
			break;
		}
		case TT_NUMBER:
			value.type = AnyValue::NUMBER;
			value.num = strtod(cpTokenBegin, NULL);
			break;
		case TT_TOKEN:
			value.str.assign(cpTokenBegin, cpTokenEnd);
			if (value.str == "nil")
			{
				value.type = AnyValue::NIL;
			}
			else
				if (value.str == "true")
				{
					value.type = AnyValue::BOOLEAN;
					value.num = 1.0;
				}
				else
					if (value.str == "false")
					{
						value.type = AnyValue::BOOLEAN;
						value.num = 0.0;
					}
					else
					{
						value.type = AnyValue::TOKEN;
					}
			break;
		default:
			throw std::runtime_error("error!");
		}
		return value;
	} // GetTokenValue

	AnyValue GetAtomValue(const char*& str, TokenType terminator)
	{
		const char* cpTokenBegin;
		const char* cpTokenEnd;
		TokenType tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
		switch (tokenType)
		{
		case TT_NONE:
			return AnyValue();
		case TT_OPE_SUBTRACT:	// -:
		{
			str = cpTokenEnd;
			AnyValue value = GetFormulaValue(str, terminator, LevelOf(TT_OPE_NEGATIVE));
			if (value.type != AnyValue::NUMBER) throw std::runtime_error("error!");
			value.num = -value.num;
			return value;
		}
		case TT_PARE_LEFT:	// (:
		{
			str = cpTokenEnd;
			AnyValue value = GetFormulaValue(str, TT_PARE_RIGHT, 0);
			// 終端を送る。
			tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
			if (tokenType == TT_PARE_RIGHT) str = cpTokenEnd;
			return value;
		}
		default:
		{
			str = cpTokenEnd;
			AnyValue value = GetTokenValue(tokenType, cpTokenBegin, cpTokenEnd);
			if (value.type == AnyValue::STRING) return value;
			if (value.type == AnyValue::NUMBER) return value;
			if (value.type == AnyValue::BOOLEAN) return value;
			if (value.type == AnyValue::TOKEN)
			{
				auto obj = GetVariableObject(value.str.c_str());
				return GetAtomValueX(str, terminator, obj);
			}
			throw std::runtime_error("error!");
		}
		}
	} // GetAtomValue

	AnyValue GetAtomValueX(const char*& str, TokenType terminator, Stickobject & obj)
	{
		const char* cpTokenBegin;
		const char* cpTokenEnd;
		TokenType tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
		switch (tokenType)
		{
		case TT_JOINT:	// .
		{
			str = cpTokenEnd;
			tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
			str = cpTokenEnd;
			AnyValue name = GetTokenValue(tokenType, cpTokenBegin, cpTokenEnd);
			if (name.type != AnyValue::TOKEN) throw std::runtime_error("error!");
			Stickobject obj1 = obj.GetTableValue(name);
			return GetAtomValueX(str, terminator, obj1);
		}
		case TT_ARRAY_LEFT:	// [
		{
			str = cpTokenEnd;
			AnyValue value = GetFormulaValue(str, TT_ARRAY_RIGHT, 0);
			// 終端を送る。
			tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
			if (tokenType == TT_ARRAY_RIGHT) str = cpTokenEnd;
			Stickobject obj1 = obj.GetTableValue(value);
			return GetAtomValueX(str, terminator, obj1);
		}
		default:
			return AnyValue(obj);
		}
	} // GetAtomValueX.

	/// <summary>
	/// Analyzes the formula and calculates the result of it.
	/// formulaの例：" a [ b - 5 ] + 4"
	/// terminatorは式終端を表す文字。例えば配列要素（[]の中の式）を解析する場合は']'が指定される。
	/// opeLevelで指定される演算子レベル以下の演算子が登場した時点で式の解析は中断し、そこまでの値を返す。
	/// 例：f = "a + b * c * d / e - f" を計算する場合、GetFormulaValue(f, '\0', 0)で計算を開始する。
	/// "+"の右側を本関数で再帰的に評価するが、このときGetFormulaValue(f, '\0', LevelOf('+'))を指定する。
	/// これにより、"+"より優先度の高い演算子を含む"b * c * d / e"までの値が取得される。
	/// </summary>
	/// <param name="formula">The formula. Returns the terminated position.</param>
	/// <param name="terminator">The terminator.</param>
	/// <param name="opeLevel">The operator's level.</param>
	/// <returns></returns>
	AnyValue GetFormulaValue(const char*& formula, TokenType terminator, int opeLevel)
	{
		AnyValue left = GetAtomValue(formula, terminator);
		for (;;)
		{
			const char* cpTokenBegin;
			const char* cpTokenEnd;
			const TokenType tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, formula);
			if (tokenType == terminator)
				//----- 終端に至った場合 -----
			{
				break;
			}
			// オペレーター以外の場合はエラー。
			if (tokenType < TT_OPERATOR_BEGIN || TT_OPERATOR_END < tokenType)
				throw std::runtime_error("error!");
			if (LevelOf(tokenType) <= opeLevel) break;
			formula = cpTokenEnd;
			const AnyValue right = GetFormulaValue(formula, terminator, LevelOf(tokenType));
			switch (tokenType)
			{
			case TT_OPE_ADD:
				left.Add(right);
				break;
			case TT_OPE_SUBTRACT:
				left.Subtract(right);
				break;
			case TT_OPE_MULTIPLE:
				left.Multiple(right);
				break;
			case TT_OPE_DIVIDE:
				left.Divide(right);
				break;
			case TT_OPE_REMAIND:
				left.Remaind(right);
				break;
			case TT_OPE_POWER:
				left.Power(right);
				break;
			case TT_OPE_CONNECT:
				left.Connect(right);
				break;
			}
		}
		return left;
	} // GetFormulaValue

	AnyValue GetVariableValue(const char* piledName)
	{
		return GetFormulaValue(piledName, TT_NONE, 0);
	} // GetVariableValue.

private:
	lua_State* m_lua_state;
	lua_Debug* m_lua_debug;
	bool m_isFirstLocal;
	bool m_isFirstGlobal;
	std::unordered_map<std::string, int> m_localVariableNameToId;
	Stickobject m_ENV;
}; // class Stickvar.

template<typename T>
class LuaToClassHook
{
private:
	using LuaHookFunc = void(*)(lua_State* luaState, lua_Debug* luaDebug);
	using ClassObjFunc = T*&(*)();
private:
	template<size_t S>
	struct LH
	{
		static void HookFunc(lua_State* L, lua_Debug* ar)
		{
			ClassObj()->OnCallLuaHook(L, ar);
		}

		static T*& ClassObj()
		{
			static T* s_ClassObj = nullptr;
			return s_ClassObj;
		}
	};
private:
	static std::vector<LuaToClassHook> & LUA_TO_CLASS_HOOK_STACK()
	{
		static std::vector<LuaToClassHook> s_LuaToClassHookArray {
			LuaToClassHook{ LH<0>::HookFunc, LH<0>::ClassObj },
			LuaToClassHook{ LH<1>::HookFunc, LH<1>::ClassObj },
			LuaToClassHook{ LH<2>::HookFunc, LH<2>::ClassObj },
			LuaToClassHook{ LH<3>::HookFunc, LH<3>::ClassObj },
			LuaToClassHook{ LH<4>::HookFunc, LH<4>::ClassObj },
			LuaToClassHook{ LH<5>::HookFunc, LH<5>::ClassObj },
			LuaToClassHook{ LH<6>::HookFunc, LH<6>::ClassObj },
			LuaToClassHook{ LH<7>::HookFunc, LH<7>::ClassObj },
			LuaToClassHook{ LH<8>::HookFunc, LH<8>::ClassObj },
			LuaToClassHook{ LH<9>::HookFunc, LH<9>::ClassObj },
		};
		return s_LuaToClassHookArray;
	}

public:
	static bool Alloc(LuaToClassHook & luaToClassHook, T* classObj)
	{
		if (LUA_TO_CLASS_HOOK_STACK().empty()) return false;
		luaToClassHook = LUA_TO_CLASS_HOOK_STACK().back();
		LUA_TO_CLASS_HOOK_STACK().pop_back();
		luaToClassHook.obj() = classObj;
		return true;
	}

	static void Free(LuaToClassHook & luaToClassHook)
	{
		LUA_TO_CLASS_HOOK_STACK().emplace_back(luaToClassHook);
	}

public:
	LuaToClassHook()
		: hook(nullptr)
		, obj(nullptr)
	{}

	LuaToClassHook(LuaHookFunc h, ClassObjFunc c)
		: hook(h)
		, obj(c)
	{}

	~LuaToClassHook() = default;

	void Clear()
	{
		hook = nullptr;
		obj = nullptr;
	}
public:
	LuaHookFunc hook;
	ClassObjFunc obj;
}; // class LuaToClassHook.

class Sticktrace
{
private:
	template<typename T> friend class LuaToClassHook;
public:
	enum class Mode : int
	{
		STOP = 0,		// Stop.
		RUN,			// Run.
		SUSPEND,		// Suspend.
		PROCEED_NEXT,	// Proceed to next line.
	};

public:
	using ScriptHookFunc = void (*)(
		lua_State* luaState,
		lua_Debug* luaDebug,
		void* userData,
		Sticktrace::Mode mode,
		Sticktrace* sticktrace
		);

public:
	Sticktrace()
		: m_stickTraceWindow(nullptr)
		, m_stickrun(nullptr)
		, m_scriptHookFunc(nullptr)
		, m_scriptUserData(nullptr)
		, m_scriptHookInterval(0)
		, m_scriptHookCount(0)
		, m_suspendMode(Mode::STOP)
	{
	}

	~Sticktrace()
	{
		Terminate();
	}
	
private:
	static std::unordered_map<lua_State*, Sticktrace*>& LUA_TO_TRACE()
	{
		static std::unordered_map<lua_State*, Sticktrace*> s_LUA_TO_TRACE;
		return s_LUA_TO_TRACE;
	}

	/// <summary>
	/// 
	/// </summary>
	static int OutputDebugMessage(lua_State* L)
	{
		try
		{
			// Check the count of arguments.
			if (lua_gettop(L) != 1)
				throw std::invalid_argument("Count of arguments is not correct.");

			std::string message;
			Sticklib::check_lvalue(message, L, 1);
			LUA_TO_TRACE().at(L)->OutputDebug(message.c_str());
		}
		catch (std::exception & e)
		{
			luaL_error(L, (std::string("C function error:::OutputDebugMessage:") + e.what()).c_str());
		}
		catch (...)
		{
			luaL_error(L, "C function error:::OutputDebugMessage");
		}
		return 0;
	}

public:
	/// <summary>
	/// Initializes the specified stickrun.
	/// </summary>
	/// <param name="stickrun">The stickrun.</param>
	/// <param name="scriptHookFunc">The script hook function. It is called when script is suspended and script is executed.</param>
	/// <param name="suspendingUserData">The suspending user data.</param>
	/// <param name="scriptHookInterval">The script hook interval. If scriptHookInterval is set to 10, scriptHookFunc is called once every 10 lines execution.</param>
	void Initialize()
	{
		if (m_stickTraceWindow == nullptr)
		{
			m_stickTraceWindow = SticktraceWindow::New();
			LuaToClassHook<Sticktrace>::Alloc(m_luaToClassHook, this);
		}
	}

	void Terminate()
	{
		if (m_stickTraceWindow != nullptr)
		{
			LuaToClassHook<Sticktrace>::Free(m_luaToClassHook);
			m_luaToClassHook.Clear();

			SticktraceWindow::Delete(m_stickTraceWindow);
			m_stickTraceWindow = nullptr;
		}
		ClearStickrun();
	}

	/// <summary>
	/// Initializes with the specified stickrun.
	/// </summary>
	/// <param name="stickrun">The stickrun.</param>
	/// <param name="scriptHookFunc">The script hook function. It is called when script is suspended and script is executed.</param>
	/// <param name="suspendingUserData">The suspending user data.</param>
	/// <param name="scriptHookInterval">The script hook interval. If scriptHookInterval is set to 10, scriptHookFunc is called once every 10 lines execution.</param>
	void SetStickrun(
		Stickrun* stickrun,
		Sticktrace::ScriptHookFunc scriptHookFunc,
		void* suspendingUserData,
		size_t scriptHookInterval
	)
	{
		if (m_stickrun == nullptr)
		{
			m_stickrun = stickrun;
			m_stickrun->SetHook(Sticktrace::HookFunc, this);
			m_scriptHookFunc = scriptHookFunc;
			m_scriptUserData = suspendingUserData;
			m_scriptHookInterval = scriptHookInterval;

			// push global table.
			//       STACK
			//    |         |
			//    |---------|      +---------+--------------+
			//    |   _G    |----->| Key     |    Value     |
			//    +---------+      |---------|--------------|
			//                     :         :              :
			Sticklib::push_table(stickrun->GetLuaState(), nullptr);

			// Create and register the static class 'STICKTRACE'.
			//       STACK
			//    |         |
			//    |---------|
			//  -1|  table  |-------------------------------+
			//    |---------|      +------------+--------+  |
			//  -2|   _G    |----->| Key        | Value  |  |
			//    +---------+      |------------|--------|  |   +--------+--------+
			//                     |"STICKTRACE"| table  |--+-->| Key    | Value  |
			//                     |------------|--------|      |--------|--------|
			//                     :            :        :      :        :        :
			Sticklib::push_table(stickrun->GetLuaState(), "STICKTRACE");

			// Register functions into the 'STICKTRACE' table.
			//       STACK
			//    |         |
			//    |---------|
			//  -1|  table  |-------------------------------+
			//    |---------|      +------------+--------+  |
			//  -2|   _G    |----->| Key        | Value  |  |
			//    +---------+      |------------|--------|  |   +--------------------+------------------+
			//                     |"STICKTRACE"| table  |--+-->| Key                | Value            |
			//                     |------------|--------|      |--------------------|------------------|
			//                     :            :        :      |"OutputDebugMessage"|OutputDebugMessage|
			//                                                  |--------------------|------------------|
			//                                                  :                    :                  :
			static struct luaL_Reg funcs[] =
			{
				{ "OutputDebugMessage", OutputDebugMessage },
				{ nullptr, nullptr },
			};
			Sticklib::set_functions(stickrun->GetLuaState(), funcs);

			// Pop the "STICKTRACE" table and Global table.
			Sticklib::pop(stickrun->GetLuaState());
			Sticklib::pop(stickrun->GetLuaState());
		}
	}

	void ClearStickrun()
	{
		if (m_stickrun != nullptr)
		{
			//       STACK
			//    |         |
			//    |---------|      +------------+--------+   
			//  -1|   _G    |----->| Key        | Value  |   
			//    +---------+      |------------|--------|      +--------+--------+
			//                     |"STICKTRACE"| table  |----->| Key    | Value  |
			//                     |------------|--------|      |--------|--------|
			//                     :            :        :      :        :        :
			Sticklib::push_table(m_stickrun->GetLuaState(), nullptr);

			//       STACK
			//    |         |
			//    |---------|      +------------+--------+   
			//  -1|   _G    |----->| Key        | Value  |   
			//    +---------+      |------------|--------|
			//                     :            :        :
			Sticklib::remove_table_item(m_stickrun->GetLuaState(), "STICKTRACE");

			// Pop the Global table.
			Sticklib::pop(m_stickrun->GetLuaState());

			m_stickrun->SetHook(nullptr, nullptr);
			m_stickrun = nullptr;
			m_scriptHookFunc = nullptr;
			m_scriptUserData = nullptr;
			m_scriptHookInterval = 0;
		}

	}

	void ShowWindow(bool show)
	{
		m_stickTraceWindow->Show(show);
	}

	bool OutputError(const char* message)
	{
		return m_stickTraceWindow->OutputError(message);
	}

	bool OutputDebug(const char* message)
	{
		return m_stickTraceWindow->OutputDebug(message);
	}

	bool SetScriptMode(Sticktrace::Mode mode)
	{
		switch (mode)
		{
		case Sticktrace::Mode::STOP:
		case Sticktrace::Mode::RUN:
		case Sticktrace::Mode::SUSPEND:
			if (m_suspendMode == Sticktrace::Mode::STOP)
				return false;
			break;
		case Sticktrace::Mode::PROCEED_NEXT:
			switch (m_suspendMode)
			{
			case Sticktrace::Mode::STOP:
			case Sticktrace::Mode::RUN:
				return false;
			}
			break;
		default:
			return false;
		}
		m_suspendMode = mode;
		return true;
	}

	Sticktrace::Mode GetScriptMode() const
	{
		return m_suspendMode;
	}

	bool Suspend()
	{
		m_suspendMode = Mode::SUSPEND;
	}

	bool Resume()
	{
		m_suspendMode = Mode::RUN;
	}

	bool ProceedToNext()
	{
		m_suspendMode = Mode::PROCEED_NEXT;
	}

	Stickrun* GetStickrun() const
	{
		return m_stickrun;
	}

private:
	SticktraceWindow* m_stickTraceWindow;
	Stickrun* m_stickrun;

	// テスト。
	LuaToClassHook<Sticktrace> m_luaToClassHook;

	Sticktrace::ScriptHookFunc m_scriptHookFunc;
	void* m_scriptUserData;	

	/// <summary>
	/// Interval when m_scriptHookFunc is called by line executing.
	/// For example, m_scriptHookInterval = 10 then m_scriptHookFunc is called once every ten lines execution.
	/// </summary>
	size_t m_scriptHookInterval;
	
	/// <summary>
	/// Work variable. It's used when line execution.
	/// </summary>
	size_t m_scriptHookCount;

	Mode m_suspendMode;
	
	/// <summary>
	/// The required variables.
	/// -example-----------
	/// "strA"
	/// "numB"
	/// "+tblC"
	/// " +tblD"
	/// "  +tblE"
	/// "tblF"
	/// "tblF["a"]"
	/// -return values-----------
	/// "strA" "string" "hello"
	/// "numB" "number" 10
	/// "+tblC" "parentTable" ""
	/// " strC" "string" "hello"
	/// " numD" "number" 15
	/// " +tblD" "parentTable" ""
	/// "  strE" "string" "hello"
	/// "  numF" "number" 15
	/// "  +tblE" "parentTable" ""
	/// "   strG" "string" "hello"
	/// "   numH" "number" 15
	/// "tblF" "parentTable" ""
	/// "tblF["a"]" "number" 20
	/// </summary>
	std::vector<std::string> m_requiredVariables;

private:
	void OnStartExec(lua_State* L)
	{
		if (m_stickTraceWindow->IsDebugMode())
			::lua_sethook(L, m_luaToClassHook.hook, LUA_MASKLINE, 0);

		
		LUA_TO_TRACE()[L] = this;

		m_stickTraceWindow->OnStart();
		m_suspendMode = Sticktrace::Mode::RUN;
		m_scriptHookCount = 0;
	}

	void OnStopExec(lua_State* L)
	{
		LUA_TO_TRACE().erase(L);
		::lua_sethook(L, nullptr, 0, 0);

		m_suspendMode = Sticktrace::Mode::STOP;
		m_stickTraceWindow->OnStop();
		m_stickTraceWindow->Jump(nullptr, -1);
	}

	// テスト。
	void PrintTable(lua_State* L)
	{
		lua_pushnil(L);

		while (lua_next(L, -2) != 0)
		{
			if (lua_isstring(L, -1))
				TRACE("Type = string : Key = %s, Value = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
			else if (lua_isnumber(L, -1))
				TRACE("Type = number : key = %s, value = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
			else if (lua_istable(L, -1))
				TRACE("Type = table : key = %s\n", lua_tostring(L, -2));
			else
				TRACE("Type = unknown : key = %s\n", lua_tostring(L, -2));

			lua_pop(L, 1);
		}
	}

	struct VariableRec
	{
		int id;
		int valueType;
		std::string value;
	}; // struct VariableRec
	
	Stickobject GetLocalVariable(lua_State* L, lua_Debug* ar, const std::string & varname, std::unordered_map<std::string, int> & localVariableNameToId)
	{
		if (localVariableNameToId.empty())
		{
			for (int id = 1;; id++)
			{
				// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
				//
				// ┌───────┐
				// │ローカル変数値│<- nで指定されたローカル変数を積む
				// ├───────┤
				// │              │
				// ├───────┤
				// │              │
				// ├───────┤
				// ：              ：
				//
				const char* name = ::lua_getlocal(L, ar, id);
				if (name == nullptr) break;
				// Records local variable name to id.
				// Two or more same name variable can be exist, so override the name to id hash.
				if (strcmp(name, "(*temporary)") != 0)
					localVariableNameToId[name] = id;
				// lua_pop (lua_State *L, int n)
				//
				// ┌───────┐─┬
				// │//////////////│  │
				// ├───────┤  │
				// │//////////////│  │ <- n個の要素を取り除く（n=3の場合）
				// ├───────┤  │
				// │//////////////│  │
				// ├───────┤─┴
				// │              │
				// ├───────┤
				// ：              ：
				//
				::lua_pop(L, 1);	// lua_getlocalで積んだ変数を削除
			}
		}
		const auto i = localVariableNameToId.find(varname);
		if (i != localVariableNameToId.end())
		{
			// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
			//
			// ┌───────┐
			// │ローカル変数値│<- nで指定されたローカル変数を積む
			// ├───────┤
			// │              │
			// ├───────┤
			// │              │
			// ├───────┤
			// ：              ：
			//
			::lua_getlocal(L, ar, i->second);
			return Stickobject(L);
		}
		else
		{
			return Stickobject();
		}
	} // GetLocalVariable.

	void EnumTable(
		std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>>& varIconIndentNameTypeValueArray,
		int indent,
		const std::string& piledName,
		AnyValue& parentTable,
		const std::unordered_set<std::string>& stExpandedName
	)
	{
		std::string icon;
		AnyValue key;
		AnyValue value;
		parentTable.BeginEnum();
		while (parentTable.NextEnum(key, value))
		{
			switch (value.type)
			{
			case AnyValue::TABLE:
				if (stExpandedName.find(piledName + '\b' + key.GetValueAsString()) == stExpandedName.end())
					icon = "CLOSED";
				else
					icon = "OPENED";
				break;
			default:
				icon = "MEMBER";
				break;
			}
			varIconIndentNameTypeValueArray.emplace_back(
				std::make_tuple(
					icon,
					std::to_string(indent),
					key.GetValueAsString(),
					value.GetTypeAsString(),
					value.GetValueAsString()
				));
			if (icon == "OPENED")
				EnumTable(
					varIconIndentNameTypeValueArray,
					indent + 1,
					piledName + '\b' + key.GetValueAsString(),
					value,
					stExpandedName
				);
		}
		parentTable.EndEnum();
	}
	
	/// <summary>
	/// Called when each one line execution of the lua script.
	/// </summary>
	/// <param name="L">Lua state object.</param>
	/// <param name="ar">Lua debug object.</param>
	void OnCallLuaHook(lua_State* L, lua_Debug* ar)
	{
		if (m_suspendMode == Sticktrace::Mode::STOP)
			(void)::luaL_error(L, "Script execution was interrupted.");
		// Check the possibility of the breakpoint existing on the current line. This function responds fast.
		if (m_suspendMode == Sticktrace::Mode::SUSPEND ||
			m_stickTraceWindow->IsBreakpoint(nullptr, ar->currentline - 1))
		{
			// 'n': fills in the field name and namewhat;
			// 'S': fills in the fields source, short_src, linedefined, lastlinedefined, and what;
			// 'l': fills in the field currentline;
			// 't': fills in the field istailcall;
			// 'u': fills in the fields nups, nparams, and isvararg;
			// 'f': pushes onto the stack the function that is running at the given level;
			// 'L': pushes onto the stack a parentTable whose indices are the numbers of the lines that are valid on the function. (A valid line is a line with some associated code, that is, a line where you can put a break point.Non - valid lines include empty lines and comments.)
			lua_getinfo(L, "Slnt", ar);
			// Check the exact breakpoint existing on the current line. This function needs some time.
			if (m_suspendMode == Sticktrace::Mode::SUSPEND ||
				m_stickTraceWindow->IsBreakpoint(ar->source, ar->currentline - 1))
			{
				m_suspendMode = Sticktrace::Mode::SUSPEND;
				std::string param1;
				auto command = m_stickTraceWindow->GetCommand(param1, 0);
				// If Stop command, throw the lua exception and stop the running of the script.
				if (command == SticktraceCommand::STOP)
					(void)::luaL_error(L, "Script execution was interrupted.");
				// Notify the sticktrace window that the script is suspended.
				m_stickTraceWindow->OnSuspended();
				// Jump the source code editor to the suspended point.
				m_stickTraceWindow->Jump(ar->source, ar->currentline - 1);

				Stickvar stickvar(L, ar);
				for (;;)
				{
					if (command == SticktraceCommand::GET_VARIABLE)
					{
						//
						//  [-]varA
						//      |
						//      +-varB
						//      |
						//     [+]varC
						//      |
						//     [-]varD
						//         |
						//         +-varE
						//         |
						//         +-varF
						//
						//  [#]varG
						//
						// varIconIndentNameTypeValueArray={
						//		Icon		Indent		Name		Type		Value
						//  { "OPENED",		 "0"	   "varA"	   "table"		 ""		},
						//  { "MEMBER",		 "1"	   "varB"	   "number"		 "5.2"	},
						//  { "CLOSED",		 "1"	   "varC"	   "table"		 ""		},
						//  { "OPENED",		 "1"	   "varD"	   "table"		 ""		},
						//  { "MEMBER",		 "2"	   "varE"	   "number"		 "3.1"	},
						//  { "MEMBER",		 "2"	   "varF"	   "string"		"mine"	},
						//  { "VARIABLE",	 "0"	   "varG"	   "string"		"hello"	},
						// }
						std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> varIconIndentNameTypeValueArray;
						std::vector<std::string> vTopLevelName;
						std::unordered_set<std::string> stExpandedName;
						auto receivedata = param1.c_str();
						Stickutil::Unserialize(vTopLevelName, receivedata);
						Stickutil::Unserialize(stExpandedName, receivedata);
						for (const auto & topvar : vTopLevelName)
						{
							auto xxx = stickvar.GetVariableValue(topvar.c_str());
							std::string icon;
							switch (xxx.type)
							{
							case AnyValue::TABLE:
								if (stExpandedName.find(topvar) == stExpandedName.end())
									icon = "CLOSED";
								else
									icon = "OPENED";
								break;
							default:
								icon = "VARIABLE";
								break;
							}
							varIconIndentNameTypeValueArray.emplace_back(
								std::make_tuple(
									icon,
									"0",
									topvar,
									xxx.GetTypeAsString(),
									xxx.GetValueAsString()
								));
							if (xxx.type == AnyValue::TABLE && icon == "OPENED")
							{
								EnumTable(varIconIndentNameTypeValueArray, 1, topvar, xxx, stExpandedName);
							}
						}

						std::string senddata;
						Stickutil::Serialize(senddata, varIconIndentNameTypeValueArray);
						m_stickTraceWindow->SetWatch(senddata);
					}
					else if (
						m_suspendMode == Sticktrace::Mode::RUN ||
						command == SticktraceCommand::RESUME
						)
					{
						m_suspendMode = Sticktrace::Mode::RUN;
						break;
					}
					else if (
						m_suspendMode == Sticktrace::Mode::STOP ||
						command == SticktraceCommand::STOP
						)
					{
						(void)::luaL_error(L, "Script execution was interrupted.");
					}
					else if (
						m_suspendMode == Sticktrace::Mode::PROCEED_NEXT ||
						command == SticktraceCommand::PROCEED_NEXT
						)
					{
						m_suspendMode = Sticktrace::Mode::SUSPEND;
						break;
					}
					if (m_scriptHookFunc != nullptr)
						m_scriptHookFunc(L, ar, m_scriptUserData, m_suspendMode, this);

					command = m_stickTraceWindow->GetCommand(param1, 100);
				}
				// Notify the sticktrace window that the script is resumed.
				m_stickTraceWindow->OnResumed();
				m_stickTraceWindow->Jump(nullptr, -1);
			}
		}
		if (m_scriptHookFunc != nullptr)
		{
			if (m_scriptHookCount == m_scriptHookInterval)
			{
				m_scriptHookFunc(L, ar, m_scriptUserData, m_suspendMode, this);
				m_scriptHookCount = 0;
			}
			else
			{
				m_scriptHookCount++;
			}
		}
	}
	
	void SetSource(const char* name, const char* src)
	{
		m_stickTraceWindow->SetSource(name, src);
	}

private:
	static void HookFunc(
		Stickrun::CallbackType callbackType,
		void* data,
		lua_State* L,
		void* userData,
		Stickrun* stickrun
	)
	{
		switch (callbackType)
		{
		case Stickrun::CallbackType::ON_LOAD_SCRIPT:
			if (((std::pair<const char*, const char*>*)data)->first != nullptr &&
				((std::pair<const char*, const char*>*)data)->second != nullptr)
				((Sticktrace*)userData)->SetSource(
					((std::pair<const char*, const char*>*)data)->first,
					((std::pair<const char*, const char*>*)data)->second
				);
			break;
		case Stickrun::CallbackType::ON_START_EXEC:
			((Sticktrace*)userData)->OnStartExec(L);
			break;
		case Stickrun::CallbackType::ON_STOP_EXEC:
			((Sticktrace*)userData)->OnStopExec(L);
			break;
		case Stickrun::CallbackType::ON_ERROR:
			((Sticktrace*)userData)->OutputError((const char*)data);
			break;
		default:
			break;
		}
	}
};

#endif // _STICKTRACE
