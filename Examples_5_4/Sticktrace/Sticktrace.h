// Sticktrace.h
//

#pragma once

#include <utility>
#include <string>
#include <codecvt>
#include <unordered_set>
#include "Sticklib.h"
#include "Stickrun.h"

// Structure of modules.
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


//
// Communication process between application and debugger window, when script loading.
//
//    Application
//         |
//         |              Stickrun
//         |                  |                                                                                                       SticktraceWindow
//         | Call Load script |                                                                                                              |
//         |----------------->|                                                                                                              |
//         :                  |-Load the script code                                                                                         |
//         :                  |                                                                                                              |
//         :                  |  Call with ON_LOAD_SCRIPT command                                                                            |
//         :                  |-------------------------------------> Sticktrace::HookFunc                                                   |
//         :                  :                                                |                                                             |
//         :                  :                                                |   Call                                                      |
//         :                  :                                                |-----------> APT_SetSource                                   |
//         :                  :                                                :                   |                                         |
//         :                  :                                                :                   |  Post the SET_SOURCE command notify     |
//         :                  :                                                :                   |---------------------------------------->|
//         :                  :                                                :                   |                                         |
//         :                  :                                                :                   |-Wait the command is taken over          |
//         :                  :                                                :                   :                                         |
//         :                  :                                                :                   :                                         |
//         :                  :                                                :                   :       Take the command and source code  |
//         :                  :                                                :           Result  |<----------------------------------------|
//         :                  :                                       Result   |<------------------V                                         |
//         :                  |<-----------------------------------------------V                                                             |
//         :                  |                                                                                                              |
//         :                  |                                                                                                              |
//         |<-----------------|                                                                                                              |
//         |                  :                                                                                                              |
//         |                  :                                                                                                              |
//         |                  :                                                                                                              |
//
//   |                                                                                                       |                     |                    |
//   |-------------------------------------------------------------------------------------------------------|                     |--------------------|
//                                                           Application thread                                                    SticktraceWindow thread
//


//
// Communication process between application and debugger window, when suspending.
//
//    Application
//         |
//         |      Create
//         |--------------------------------------------------------------------------------------------> SticktraceWindow
//         |                                                                                                     |
//         |      Start                                                                                          |
//         |---------------> Lua                                                                                 |
//         :                  |                                                                                  |
//         :             +--->|                                                                                  |
//         :             |    |                                                                                  |
//         :             |    |    Call                                                                          |
//         :             |    |------------> OnCallLuaHook                                                       |
//         :             |    :                    |                                                             |
//         :             |    :               +--->|                                                             |
//         :             |    :               |    |  Call                                                       |
//         :             |    :               |    |--------> APT_WaitCommandIsSet                               |
//         :             |    :               |    :                   |                                         |
//         :             |    :               |    :                   |-Wait a command is set                   |
//         :             |    :               |    :                   :                                         |
//         :             |    :               |    :                   :                                         |
//         :             |    :               |    :                   :                                         |-Button clicked
//         :             |    :               |    :                   :         A:Set the GET_VARIABLE command  |
//         :             |    :               |    :          Command  |<----------------------------------------|
//         :             |    :               |    |<------------------V                                         |
//         :             |    :               |    |                                                             |
//         :             |    :               |    |-Get variables                                               |
//         :             |    :               |    |                                                             |
//         :             |    :               |    |     Call                                                    |
//         :             |    :               |    |------------> APT_SetWatch                                   |
//         :             |    :               |    :                   |                                         |
//         :             |    :               |    :                   |  Post the SET_WATCH command notify      |
//         :             |    :               |    :                   |---------------------------------------->|
//         :             |    :               |    :                   |                                         |
//         :             |    :               |    :                   |-Wait the command is taken over          |
//         :             |    :               |    :                   :                                         |
//         :             |    :               |    :                   :                                         |
//         :             |    :               |    :                   :                                         |
//         :             |    :               |    :                   :         Take the command and variables  |
//         :             |    :               |    :          Result   |<----------------------------------------|
//         :             |    :               |    |-------------------V                                         |
//         :             |    :               |    |                                                             |
//         :             |    :               |    |                                                             |
//         :             |    :               +----|-Proceed if set the RESUME command at A.                     |
//         :             |    :                    |                                                             |
//         :             |    :                    |                                                             |
//         :             |    |<-------------------V                                                             |
//         :             |    |                                                                                  |
//         :             |    |                                                                                  |
//         :             +----|                                                                                  |
//         :                  |                                                                                  |
//
//   |                                                                                |                |                    |
//   |--------------------------------------------------------------------------------|                |--------------------|
//                                 Application thread                                                  SticktraceWindow thread
//


//
// Communication process between application and debugger window, when the save command is issued in the debugger window.
//
//    Application
//         |
//         |      Create
//         |--------------------------------------------------------------------------------------------> SticktraceWindow
//         |                                                                                                     |
//         |                                                                                                     |
//         |                                                                                                     |-Issue the save command
//         |                                                                                             Call    |
//         |                                                           DGT_debuggerCallbackFunc <----------------|
//         |                                                                      |                              :
//         |           Notify using a method provided by application              |                              :
//         |<---------------------------------------------------------------------|                              :
//         |                                                                      |                              :
//         |                                                                      |-Wait the result is set       :
//         |                                                                      :                              :
//         |                    Take the script code                              :                              :
//         |--------------------------------------------------------------------->:                              :
//         |                                                                      :                              :
//         |-Save the script code                                                 :                              :
//         |                                                                      :                              :
//         |     Set the result                                                   :                              :
//         |--------------------------------------------------------------------->|    Result                    :
//         |                                                                      V----------------------------->|
//         |                                                                                                     |
//         |                                                                                                     |
//         |                                                                                                     |
//         |                                                                                                     |
//
//   |                                                                                |                |                    |
//   |--------------------------------------------------------------------------------|                |--------------------|
//                                 Application thread                                                  SticktraceWindow thread
//

class Sticktrace;

namespace SticktraceDef
{
	enum class Mode : int
	{
		STOP = 0,		// Stop.
		RUN,			// Run.
		SUSPEND,		// Suspend.
		SUSPENDING,		// Suspending.
		PROCEED_NEXT,	// Proceed to next line.
//----- 21.05.18 Fukushiro M. 追加始 ()-----
		LUAERROR,		// Error occurred while loading or executing.
//----- 21.05.18 Fukushiro M. 追加終 ()-----
	};

	using ScriptHookFunc = void (*)(
		lua_State* luaState,
		lua_Debug* luaDebug,
		void* userData,
		SticktraceDef::Mode mode,
		Sticktrace* sticktrace
		);

	/// <summary>
	/// Commands issued from the debugger.
	/// </summary>
	enum class DebuggerCommand : int
	{
		NONE = 0,			// None.
		SAVE_SCRIPT,		// Save the script.
//----- 21.05.18 Fukushiro M. 追加始 ()-----
		ON_ERROR_OUTPUT,	// Output error message.
		ON_DEBUG_OUTPUT,	// Output debug message.
//----- 21.05.18 Fukushiro M. 追加終 ()-----
	};

	struct DebuggerCallbackParam
	{
		DebuggerCallbackParam()
			: command(DebuggerCommand::NONE)
			, strParam1(nullptr)
			, strParam2(nullptr)
		{}

		/// <summary>
		/// ja; コマンド。コールバックのタイプを指定する。
		/// en; Command. It specifies the type of callback.
		/// </summary>
		DebuggerCommand command;

		/// <summary>
		/// ja; パラメーター
		/// en; Parameter
		/// </summary>
		char const * strParam1;

		/// <summary>
		/// ja; パラメーター
		/// en; Parameter
		/// </summary>
		char const * strParam2;
	};

	/// <summary>
	/// Callback from debugger window.
	/// Case:command=SAVE_SCRIPT
	///   param->strParam1:Script name
	///   param->strParam2:Script code. Note:New line char is '\n', not "\r\n".
	/// </summary>
	using DebuggerCallbackFunc = bool (*)(
		unsigned int dialogId,
// 21.05.19 Fukushiro M. 1行削除 ()
//		SticktraceDef::DebuggerCommand command,
		DebuggerCallbackParam* param,
		void* userData
		);

	/// <summary>
	/// Commands used between the script is being suspended.
	/// </summary>
	enum class SuspendCommand
	{
		NONE,
		GET_VARIABLE,
		SET_VARIABLE,
		RESUME,
		STOP,
		PROCEED_NEXT,
	};

	struct WatchInfo
	{
		WatchInfo() = default;
		WatchInfo(
			const std::string& _icon,
			const std::string& _indent,
			const std::string& _name,
			const std::string& _type,
			const std::string& _value
		)
			: icon(_icon)
			, indent(_indent)
			, name(_name)
			, type(_type)
			, value(_value)
		{}
		std::string icon;
		std::string indent;
		std::string name;
		std::string type;
		std::string value;
	};

	/// <summary>
	/// Script execution type.
	/// </summary>
	enum class ExecType
	{
		ON_LOAD,	// Execute on load.
		ON_CALL,	// Execute on function call.
	};
}

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

	static void Serialize(std::string & data, const __int32 & v)
	{
		data += std::to_string(v) + ' ';
	}

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

	static void Serialize(
		std::string & data,
		const SticktraceDef::WatchInfo& v
	)
	{
		Serialize(data, v.icon);
		Serialize(data, v.indent);
		Serialize(data, v.name);
		Serialize(data, v.type);
		Serialize(data, v.value);
	}

	static void Serialize(
		std::string & data,
		const std::vector<SticktraceDef::WatchInfo>& v
	)
	{
		auto strSize = std::to_string(v.size());
		data += strSize + ' ';
		for (const auto & i : v)
			Serialize(data, i);
	}

	static void Unserialize(__int32 & v, const char*& data)
	{
		v = std::strtol(data, (char**)&data, 10);
		data++;
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

	static void Unserialize(SticktraceDef::WatchInfo& v, const char*& data)
	{
		Unserialize(v.icon, data);
		Unserialize(v.indent, data);
		Unserialize(v.name, data);
		Unserialize(v.type, data);
		Unserialize(v.value, data);
	}

	static void Unserialize(std::vector<SticktraceDef::WatchInfo>& v, const char*& data)
	{
		const auto size = std::strtol(data, (char**)&data, 10);
		data++;
		SticktraceDef::WatchInfo elem;
		for (int i = 0; i != size; i++)
		{
			Unserialize(elem, data);
			v.insert(v.end(), elem);
		}
	}
}; // class Stickutil.

/// <summary>
/// String buffer to get and set between exe and dll.
/// </summary>
class StickString
{
public:
	StickString() = default;
	virtual ~StickString() = default;
	virtual StickString & operator = (const char * cp)
	{
		m_str = cp;
		return *this;
	}
	virtual const char * c_str () const
	{
		return m_str.c_str();
	}
private:
	std::string m_str;
}; // class StickString.

#if !defined(_STICKTRACE)

class __declspec(dllimport) SticktraceWindow
{
	friend class Sticktrace;
private:
	static SticktraceWindow* New(
		const wchar_t* companyName,
		const wchar_t* packageName,
		const wchar_t* applicationName,
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);
	static void Delete(SticktraceWindow* mtw);
	SticktraceWindow(
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	) = delete;
	~SticktraceWindow() = delete;
	bool Show(bool show);
	bool IsVisible(bool & isVisible);
	bool IsScriptModified(bool & isModified);
	bool SetSource(const char * sandbox, const char * name, const char * source);
	bool IsDebugMode();
	bool IsBreakpoint(const char* name, int lineIndex);
	bool OnSuspended();
	bool OnResumed();
	bool Jump(const char * name, int lineIndex);
	bool NewSession();
	bool OnStart(SticktraceDef::ExecType execType);
	bool OnStop(SticktraceDef::ExecType execType);
	bool OutputError(const char* message);
	bool OutputDebug(const char* message);
	bool SetWatch(const char * data);
	bool SetVariableNotify(bool succeeded);
	SticktraceDef::SuspendCommand WaitCommandIsSet(StickString & paramA, uint32_t waitMilliseconds);
	void Destroy();
	static void StaticThreadFunc(
		SticktraceWindow* stickTrace,
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);
	void ThreadFunc(
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);
}; // class __declspec(dllimport) SticktraceWindow.

// class Sticktrace;

class Sticktrace
{
	class Stickobject;

private:
	template<typename T> struct _TrAnyValue
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
		_TrAnyValue();

		_TrAnyValue(_TrAnyValue && v)
			: _TrAnyValue()
		{
			type = v.type;
			str = v.str;
			num = v.num;
			obj = std::move(v.obj);
			v.type = _TrAnyValue::NONE;
			v.str.clear();
			v.num = 0.0;
		}

		_TrAnyValue(const _TrAnyValue & v) = delete;
// 20.07.16 Fukushiro M. 1行削除 ()
//		_TrAnyValue(_TrAnyValue & v) = delete;

		_TrAnyValue(Stickobject && o)
			:_TrAnyValue()
		{
			Set(std::move(o));
		}

		_TrAnyValue(const Stickobject & o) = delete;
		_TrAnyValue(Stickobject & o) = delete;

		_TrAnyValue(const bool & b)
			: _TrAnyValue()
		{
			type = _TrAnyValue::BOOLEAN;
			num = b ? 1.0 : 0.0;
		}

		_TrAnyValue(const bool && b)
			: _TrAnyValue()
		{
			type = _TrAnyValue::BOOLEAN;
			num = b ? 1.0 : 0.0;
		}

		_TrAnyValue(const double & n)
			: _TrAnyValue()
		{
			type = _TrAnyValue::NUMBER;
			num = n;
		}

		_TrAnyValue(const char* s)
			: _TrAnyValue()
		{
			type = _TrAnyValue::STRING;
			str = s;
		}

		~_TrAnyValue()
		{
			Clear();
		}

		void Clear()
		{
			type = _TrAnyValue::NONE;
			str.clear();
			num = 0.0;
		}

		_TrAnyValue & operator = (_TrAnyValue && v)
		{
			Clear();
			type = v.type;
			str = v.str;
			num = v.num;
			obj = std::move(v.obj);
			v.type = _TrAnyValue::NONE;
			v.str.clear();
			v.num = 0.0;
			return *this;
		}

		_TrAnyValue & operator = (const _TrAnyValue & v) = delete;
// 20.07.16 Fukushiro M. 1行削除 ()
//		_TrAnyValue & operator = (_TrAnyValue & v) = delete;

		void Set(Stickobject && o)
		{
			Clear();
			obj = std::move(o);
			if (obj.Isnull())
			{
				type = _TrAnyValue::NONE;
			}
			else
			{
				switch (obj.GetType())
				{
				case LUA_TNONE:
					type = _TrAnyValue::NONE;
					break;
				case LUA_TNIL:
					type = _TrAnyValue::NIL;
					break;
				case LUA_TBOOLEAN:
					type = _TrAnyValue::BOOLEAN;
					num = obj.GetBoolean();
					break;
				case LUA_TLIGHTUSERDATA:
					type = _TrAnyValue::LIGHTUSERDATA;
					break;
				case LUA_TNUMBER:
					type = _TrAnyValue::NUMBER;
					num = obj.GetNumber();
					break;
				case LUA_TSTRING:
					type = _TrAnyValue::STRING;
					str = obj.GetString();
					break;
				case LUA_TTABLE:
					type = _TrAnyValue::TABLE;
					break;
				case LUA_TFUNCTION:
					type = _TrAnyValue::FUNCTION;
					break;
				case LUA_TUSERDATA:
					type = _TrAnyValue::USERDATA;
					break;
				case LUA_TTHREAD:
					type = _TrAnyValue::THREAD;
					break;
				default:
					type = _TrAnyValue::NONE;
					break;
				}
			}
		} // Set(Stickobject & o)

		void Set(const bool & b)
		{
			Clear();
			type = _TrAnyValue::BOOLEAN;
			num = b ? 1.0 : 0.0;
		}

		void Set(const bool && b)
		{
			Set((const bool &)b);
		}

		void Set(const double & n)
		{
			Clear();
			type = _TrAnyValue::NUMBER;
			num = n;
		}

		void Set(const char* s)
		{
			Clear();
			type = _TrAnyValue::STRING;
			str = s;
		}

		void SetNil()
		{
			Clear();
			type = _TrAnyValue::NIL;
		}

		void Add(const _TrAnyValue& v)
		{
			if (type == _TrAnyValue::NUMBER && v.type == _TrAnyValue::NUMBER)
				num += v.num;
			else
				throw std::runtime_error("Try to add non number.");
		}

		void Subtract(const _TrAnyValue& v)
		{
			if (type == _TrAnyValue::NUMBER && v.type == _TrAnyValue::NUMBER)
				num -= v.num;
			else
				throw std::runtime_error("Try to subtract non number.");
		}

		void Multiple(const _TrAnyValue& v)
		{
			if (type == _TrAnyValue::NUMBER && v.type == _TrAnyValue::NUMBER)
				num *= v.num;
			else
				throw std::runtime_error("Try to multiple non number.");
		}

		void Divide(const _TrAnyValue& v)
		{
			if (type == _TrAnyValue::NUMBER && v.type == _TrAnyValue::NUMBER)
				num /= v.num;
			else
				throw std::runtime_error("Try to divide non number.");
		}

		void Remain(const _TrAnyValue& v)
		{
			if (type == _TrAnyValue::NUMBER && v.type == _TrAnyValue::NUMBER)
				num = num - floor(num / v.num) * v.num;
			else
				throw std::runtime_error("Try to calculate the remain of non number.");
		}

		void Power(const _TrAnyValue& v)
		{
			if (type == _TrAnyValue::NUMBER && v.type == _TrAnyValue::NUMBER)
				num = std::pow(num, v.num);
			else
				throw std::runtime_error("Try to calculate the power of non number.");
		}

		void Connect(const _TrAnyValue & v)
		{
			ToString();
			// v.ToString();
			str += v.GetValueAsString();
		}

		void ToString()
		{
			if (type == _TrAnyValue::NUMBER)
			{
				type = _TrAnyValue::STRING;
				char buf[20];
				sprintf_s(buf, "%.15g", num);
				str = buf;
			}
			else if (type != _TrAnyValue::STRING)
			{
				throw std::runtime_error("String conversion was failed.");
			}
		}

		std::string GetValueAsString() const
		{
			std::string astr;
			switch (type)
			{
			case _TrAnyValue::NUMBER:
			{
				char buff[30];
				sprintf_s(buff, "%.15g", num);
				astr = buff;
				break;
			}
			case _TrAnyValue::BOOLEAN:
				astr = (num == 0.0) ? "false" : "true";
				break;
			case _TrAnyValue::STRING:
				astr = std::string("\"") + str + "\"";
				break;
			}
			return astr;
		}

		std::string GetTypeAsString() const
		{
			switch (type)
			{
			case _TrAnyValue::NIL:
				return "nil";
			case _TrAnyValue::BOOLEAN:
				return "boolean";
			case _TrAnyValue::LIGHTUSERDATA:
				return "lightuserdata";
			case _TrAnyValue::NUMBER:
				return "number";
			case _TrAnyValue::STRING:
				return "string";
			case _TrAnyValue::TABLE:
				return "table";
			case _TrAnyValue::FUNCTION:
				return "function";
			case _TrAnyValue::USERDATA:
				return "userdata";
			case _TrAnyValue::THREAD:
				return "thread";
			case _TrAnyValue::TOKEN:
				return "word";
			default:
				return "";
			}
		}

		Sticklib::AnyValue ToSticklibAnyValue() const
		{
			switch (type)
			{
			case _TrAnyValue::NIL:
				return Sticklib::AnyValue();
			case _TrAnyValue::BOOLEAN:
				return Sticklib::AnyValue((num == 0.0) ? false : true);
			case _TrAnyValue::NUMBER:
				return Sticklib::AnyValue(num);
			case _TrAnyValue::STRING:
			case _TrAnyValue::TOKEN:
				return Sticklib::AnyValue(str.c_str());
			default:
				throw std::runtime_error("System error : AnyValue::ToSticklibAnyValue : Cannot convert to Sticklib::AnyValue.");
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

		bool NextEnum(_TrAnyValue& key, _TrAnyValue& value)
		{
			return obj.NextEnum(key, value);
		}

		Type type;
		std::string str;
		double num;
		Stickobject obj;
	}; // struct _TrAnyValue

	class Stickobject
	{
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
			// Move the top value of the stack to the registry.
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

		Stickobject(Stickobject && v)
			: Stickobject()
		{
			m_lua_state = v.m_lua_state;
			m_ref = v.m_ref;
			m_type = v.m_type;
			v.m_lua_state = nullptr;
			v.m_ref = -1;
			v.m_type = -1;
		}


		Stickobject(const Stickobject & v) = delete;
// 20.07.16 Fukushiro M. 1行削除 ()
//		Stickobject(Stickobject & v) = delete;

	//----- 20.06.04  削除始 ()-----
	//	Stickobject(const Stickobject & v)
	//		: Stickobject()
	//	{
	//		m_lua_state = v.m_lua_state;
	//		m_ref = v.m_ref;
	//		m_type = v.m_type;
	//	}
	//----- 20.06.04  削除終 ()-----

		// 20.06.04  1行削除 ()
	//	Stickobject(Move && v) = delete;
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
				//
				// Remove the value from the registry.
				//
				//      registry
				//   +-------+--------+
				//   | Key   | Value  |
				//   |-------|--------|
				//   |       | valueX | <- m_ref  Remove.
				//   +-------+--------+
				//   :       :        :
				//
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
		
		Stickobject GetTableValue(const _TrAnyValue<void>& name)
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
			case _TrAnyValue<void>::NUMBER:
				::lua_pushnumber(m_lua_state, name.num);
				break;
			case _TrAnyValue<void>::STRING:
			case _TrAnyValue<void>::TOKEN:
				::lua_pushstring(m_lua_state, name.str.c_str());
				break;
			case _TrAnyValue<void>::BOOLEAN:
				::lua_pushboolean(m_lua_state, name.num ? true : false);
				break;
			default:
				throw std::runtime_error("System error : GetTableValue");
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

		_TrAnyValue<void> SetTableValue(const _TrAnyValue<void>& name, const _TrAnyValue<void>& value)
		{
			if (m_type != LUA_TTABLE)
				throw std::runtime_error("System error : SetTableValue-1");

			//       stack           registry
			//    +---------+       +--------+
			//  -1| tableX  |<---+  | Value  |
			//    |---------|    |  |--------|
			//    :         :    +--| tableX | <- m_ref
			//                      |--------|
			//                      :        :
			::lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_ref);

			auto anyName = name.ToSticklibAnyValue();
			auto anyValue = value.ToSticklibAnyValue();

			//       stack
			//    +---------+
			//  -1| anyName |
			//    |---------|      +---------+---------+
			//  -2| tableX  |----->| Key     | Value   |
			//    |---------|      +---------+---------+
			//    :         :      :         :         :
			Sticklib::push_lvalue<Sticklib::AnyValue>(m_lua_state, anyName, false);

			//       stack
			//    +---------+
			//  -1| anyValue|
			//    |---------|
			//  -2| anyName |
			//    |---------|      +---------+---------+
			//  -3| tableX  |----->| Key     | Value   |
			//    |---------|      +---------+---------+
			//    :         :      :         :         :
			Sticklib::push_lvalue<Sticklib::AnyValue>(m_lua_state, anyValue, false);

			//       stack
			//    +---------+      +---------+---------+
			//  -1| tableX  |----->| Key     | Value   |
			//    |---------|      +---------+---------+
			//    :         :      | anyName | anyValue|
			//                     |---------|---------|
			//                     :         :         :
			lua_settable(m_lua_state, -3);

			// Erase tableX from stack.
			lua_pop(m_lua_state, 1);

			return _TrAnyValue<void>(true);
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

		bool NextEnum(_TrAnyValue<void>& key, _TrAnyValue<void>& value)
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

		Stickobject & operator = (const Stickobject & v) = delete;
// 20.07.16 Fukushiro M. 1行削除 ()
//		Stickobject & operator = (Stickobject & v) = delete;

		Stickobject & operator = (Stickobject && v)
		{
			Clear();
			m_lua_state = v.m_lua_state;
			m_ref = v.m_ref;
			m_type = v.m_type;
			v.m_lua_state = nullptr;
			v.m_ref = -1;
			v.m_type = -1;
			return *this;
		}

	private:
		lua_State* m_lua_state;
		int m_ref;
		int m_type;
	}; // class Stickobject.

	using TrAnyValue = _TrAnyValue<void>;

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
		/// Return the priority of operators.
		/// </summary>
		/// <param name="opeType">Type of the ope.</param>
		/// <returns></returns>
		int PriorityOfOperator(TokenType opeType) const
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
			throw std::runtime_error("System error : PriorityOfOperator");
		} // PriorityOfOperator.
		
		Stickobject GetLocalVariable(const std::string & varname)
		{
			if (m_isFirstLocal)
			{
				m_isFirstLocal = false;
				for (int id = 1;; id++)
				{
					// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
					//
					//       Stack
					// +---------------+
					// | variable value| <- Push the value of the local variable specified by n.
					// |---------------|
					// |               |
					// |---------------|
					// |               |
					// |---------------|
					// :               :
					//
					const char* name = ::lua_getlocal(m_lua_state, m_lua_debug, id);
					if (name == nullptr) break;
					// Records local variable name to id.
					// Two or more same name variable can be exist, so override the name to id hash.
					if (strcmp(name, "(*temporary)") != 0)
						m_localVariableNameToId[name] = id;
					// lua_pop (lua_State *L, int n)
					//
					//       Stack
					// +---------------+  ---
					// | variable value|    |<- Remove n items (e.g. n=2)
					// |---------------|    |
					// |               |    |
					// |---------------|  ---
					// |               |
					// |---------------|
					// :               :
					//
					::lua_pop(m_lua_state, 1);	// lua_getlocalで積んだ変数を削除
				}
			}
			const auto i = m_localVariableNameToId.find(varname);
			if (i != m_localVariableNameToId.end())
			{
				// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
				//
				//       Stack
				// +---------------+
				// | variable value| <- Push the value of the local variable specified by n.
				// |---------------|
				// |               |
				// |---------------|
				// |               |
				// |---------------|
				// :               :
				//
				::lua_getlocal(m_lua_state, m_lua_debug, i->second);
				return Stickobject(m_lua_state);
			}
			else
			{
				return Stickobject();
			}
		} // GetLocalVariable.

		TrAnyValue SetLocalVariable(const std::string & varname, const TrAnyValue & newValue)
		{
			if (m_isFirstLocal)
			{
				m_isFirstLocal = false;
				for (int id = 1;; id++)
				{
					// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
					//
					//       Stack
					// +---------------+
					// | variable value| <- Push the value of the local variable specified by n.
					// |---------------|
					// |               |
					// |---------------|
					// |               |
					// |---------------|
					// :               :
					//
					const char* name = ::lua_getlocal(m_lua_state, m_lua_debug, id);
					if (name == nullptr) break;
					// Records local variable name to id.
					// Two or more same name variable can be exist, so override the name to id hash.
					if (strcmp(name, "(*temporary)") != 0)
						m_localVariableNameToId[name] = id;
					// lua_pop (lua_State *L, int n)
					//
					//       Stack
					// +---------------+  ---
					// | variable value|    |<- Remove n items (e.g. n=2)
					// |---------------|    |
					// |               |    |
					// |---------------|  ---
					// |               |
					// |---------------|
					// :               :
					//
					::lua_pop(m_lua_state, 1);	// lua_getlocalで積んだ変数を削除
				}
			}
			const auto i = m_localVariableNameToId.find(varname);
			if (i != m_localVariableNameToId.end())
			{
				auto anyValue = newValue.ToSticklibAnyValue();

				//       stack
				//    +---------+
				//  -1| anyValue|
				//    |---------|
				//    :         :
				Sticklib::push_lvalue<Sticklib::AnyValue>(m_lua_state, anyValue, false);

				// Assign the value at the top of the stack to the variable and pop the top of the stack.
				//
				::lua_setlocal(m_lua_state, m_lua_debug, i->second);
				return TrAnyValue(true);
			}
			else
			{
				return TrAnyValue(false);
			}
		} // SetLocalVariable.

		Stickobject GetGlobalVariable(const std::string & varname)
		{
			if (m_isFirstGlobal)
			{
				m_isFirstGlobal = false;
				m_ENV = std::move(GetLocalVariable("_ENV"));
				if (m_ENV.Isnull())
				{	//----- if "_ENV" is not defined as the local variable -----
					lua_pushglobaltable(m_lua_state);
					m_ENV = std::move(Stickobject(m_lua_state));
				}
				else if (m_ENV.GetType() != LUA_TTABLE)
				{	//----- if "_ENV" is defined as the local variable and its type is not parentTable -----
					m_ENV.Clear();
				}
			}
			if (m_ENV.Isnull())
				return Stickobject();
			return m_ENV.GetTableValue(TrAnyValue(varname.c_str()));
		}

		TrAnyValue SetGlobalVariable(const std::string & varname, const TrAnyValue & newValue)
		{
			if (m_isFirstGlobal)
			{
				m_isFirstGlobal = false;
				m_ENV = std::move(GetLocalVariable("_ENV"));
				if (m_ENV.Isnull())
				{	//----- if "_ENV" is not defined as the local variable -----
					lua_pushglobaltable(m_lua_state);
					m_ENV = std::move(Stickobject(m_lua_state));
				}
				else if (m_ENV.GetType() != LUA_TTABLE)
				{	//----- if "_ENV" is defined as the local variable and its type is not parentTable -----
					m_ENV.Clear();
				}
			}
			if (m_ENV.Isnull())
				return TrAnyValue(false);
			return m_ENV.SetTableValue(TrAnyValue(varname.c_str()), newValue);
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
				bool isEsc = false;
				for (cpFormula++;; cpFormula++)
				{
					if (cpFormula[0] == '\0')
						throw std::runtime_error("System error : ExtractToken-1");
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
			throw std::runtime_error("System error : ExtractToken-2");
		} // ExtractToken.

		/// <summary>
		/// Gets the token value.
		///
		/// 数字から始まる場合は数値と判断。「"」「'」から始まる場合は文字列、英字から始まる場合はワードとする。
		/// </summary>
		/// <param name="tokenType">Type of the token.</param>
		/// <param name="cpTokenBegin">The cp token begin.</param>
		/// <param name="cpTokenEnd">The cp token end.</param>
		/// <returns></returns>
		TrAnyValue GetTokenValue(TokenType tokenType, const char* cpTokenBegin, const char* cpTokenEnd) const
		{
			TrAnyValue value;
			switch (tokenType)
			{
			case TT_STRING:
			{
				value.type = TrAnyValue::STRING;
				bool isEsc = false;
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
				value.type = TrAnyValue::NUMBER;
				value.num = strtod(cpTokenBegin, nullptr);
				break;
			case TT_TOKEN:
				value.str.assign(cpTokenBegin, cpTokenEnd);
				if (value.str == "nil")
				{
					value.type = TrAnyValue::NIL;
				}
				else
					if (value.str == "true")
					{
						value.type = TrAnyValue::BOOLEAN;
						value.num = 1.0;
					}
					else
						if (value.str == "false")
						{
							value.type = TrAnyValue::BOOLEAN;
							value.num = 0.0;
						}
						else
						{
							value.type = TrAnyValue::TOKEN;
						}
				break;
			default:
				throw std::runtime_error("System error : GetTokenValue");
			}
			return value;
		} // GetTokenValue

		TrAnyValue GetSetAtomValue(bool is_set, bool is_local, const char*& str, TokenType terminator, const TrAnyValue & newValue)
		{
			const char* cpTokenBegin;
			const char* cpTokenEnd;
			TokenType tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
			switch (tokenType)
			{
			case TT_NONE:
				return TrAnyValue();
			case TT_OPE_SUBTRACT:	// -:
			{
				str = cpTokenEnd;
				auto value = GetSetFormulaValue(is_set, is_local, str, terminator, PriorityOfOperator(TT_OPE_NEGATIVE), newValue);
				if (value.type != TrAnyValue::NUMBER)
					throw std::runtime_error("System error : GetSetAtomValue-1");
				value.num = -value.num;
				return value;
			}
			case TT_PARE_LEFT:	// (:
			{
				str = cpTokenEnd;
				auto value = GetSetFormulaValue(is_set, is_local, str, TT_PARE_RIGHT, 0, newValue);
				// 終端を送る。
				tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
				if (tokenType == TT_PARE_RIGHT) str = cpTokenEnd;
				return value;
			}
			default:
			{
				str = cpTokenEnd;
				auto value = GetTokenValue(tokenType, cpTokenBegin, cpTokenEnd);
				switch (value.type)
				{
				case TrAnyValue::STRING:
				case TrAnyValue::NUMBER:
				case TrAnyValue::BOOLEAN:
					return value;
				case TrAnyValue::TOKEN:
	//----- 20.06.04  変更前 ()-----
	//			{
	//				auto && obj = GetVariableObject(is_local, value.str);
	//				return GetSetAtomValueX(is_local, str, terminator, std::move(obj));
	//			}
	//----- 20.06.04  変更後 ()-----
					if (is_local)
					{
						if (is_set && str[0] == '\0')
						{
							return SetLocalVariable(value.str, newValue);
						}
						else
						{
// 20.07.16 Fukushiro M. 1行変更 ()
//							auto && obj = GetLocalVariable(value.str);
							auto obj = std::move(GetLocalVariable(value.str));
							if (obj.Isnull())
								return TrAnyValue();
							else
								return GetSetAtomValueX(is_set, is_local, str, terminator, std::move(obj), newValue);
						}
					}
					else
					{
						if (is_set && str[0] == '\0')
						{
							return SetGlobalVariable(value.str, newValue);
						}
						else
						{
// 20.07.16 Fukushiro M. 1行変更 ()
//							auto && obj = GetGlobalVariable(value.str);
							auto obj = std::move(GetGlobalVariable(value.str));
							return GetSetAtomValueX(is_set, is_local, str, terminator, std::move(obj), newValue);
						}
					}
					//----- 20.06.04  変更終 ()-----
				default:
					throw std::runtime_error("System error : GetSetAtomValue-2");
					break;
				}
			}
			}
		} // GetSetAtomValue

		TrAnyValue GetSetAtomValueX(bool is_set, bool is_local, const char*& str, TokenType terminator, Stickobject && obj, const TrAnyValue & newValue)
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
				auto name = GetTokenValue(tokenType, cpTokenBegin, cpTokenEnd);
				if (name.type != TrAnyValue::TOKEN)
					throw std::runtime_error("The token after the period is not a word.");
				if (is_set && str[0] == '\0')
				{
					return obj.SetTableValue(name, newValue);
				}
				else
				{
// 20.07.16 Fukushiro M. 1行変更 ()
//					auto && obj1 = obj.GetTableValue(name);
					auto obj1 = std::move(obj.GetTableValue(name));
					return GetSetAtomValueX(is_set, is_local, str, terminator, std::move(obj1), newValue);
				}
			}
			case TT_ARRAY_LEFT:	// [
			{
				str = cpTokenEnd;
				auto value = GetSetFormulaValue(is_set, is_local, str, TT_ARRAY_RIGHT, 0, newValue);
				// 終端を送る。
				tokenType = ExtractToken(cpTokenBegin, cpTokenEnd, str);
				if (tokenType == TT_ARRAY_RIGHT) str = cpTokenEnd;
	//----- 20.06.04  追加始 ()-----
				if (is_set && str[0] == '\0')
				{
					return obj.SetTableValue(value, newValue);
				}
	//----- 20.06.04  追加終 ()-----
// 20.07.16 Fukushiro M. 1行変更 ()
//				auto && obj1 = obj.GetTableValue(value);
				auto obj1 = std::move(obj.GetTableValue(value));
				return GetSetAtomValueX(is_set, is_local, str, terminator, std::move(obj1), newValue);
			}
			default:
				return TrAnyValue(std::move(obj));
			}
		} // GetSetAtomValueX.

		/// <summary>
		/// Analyzes the formula and calculates the result of it.
		/// formulaの例：" a [ b - 5 ] + 4"
		/// terminatorは式終端を表す文字。例えば配列要素（[]の中の式）を解析する場合は']'が指定される。
		/// operatorPriorityで指定される演算子レベル以下の演算子が登場した時点で式の解析は中断し、そこまでの値を返す。
		/// 例：f = "a + b * c * d / e - f" を計算する場合、GetSetFormulaValue(f, '\0', 0)で計算を開始する。
		/// "+"の右側を本関数で再帰的に評価するが、このときGetSetFormulaValue(f, '\0', PriorityOfOperator('+'))を指定する。
		/// これにより、"+"より優先度の高い演算子を含む"b * c * d / e"までの値が取得される。
		/// </summary>
		/// <param name="formula">The formula. Returns the terminated position.</param>
		/// <param name="terminator">The terminator.</param>
		/// <param name="operatorPriority">The operator's priority.</param>
		/// <returns></returns>
		TrAnyValue GetSetFormulaValue(
			bool is_set,
			bool is_local,
			const char*& formula,
			TokenType terminator,
			int operatorPriority,
			const TrAnyValue & newValue
		)
		{
			auto left = GetSetAtomValue(is_set, is_local, formula, terminator, newValue);
			if (left.type == TrAnyValue::Type::NONE)
				return left;
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
					throw std::runtime_error("System error : GetSetFormulaValue");
				if (PriorityOfOperator(tokenType) <= operatorPriority) break;
				formula = cpTokenEnd;
				const auto right = GetSetFormulaValue(is_set, is_local, formula, terminator, PriorityOfOperator(tokenType), newValue);
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
					left.Remain(right);
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
		} // GetSetFormulaValue

		TrAnyValue GetVariableValue(bool is_local, const char* piledName)
		{
			return GetSetFormulaValue(false, is_local, piledName, TT_NONE, 0, (const TrAnyValue &)TrAnyValue());
		} // GetVariableValue.

		bool SetVariableValue(bool is_local, const char* piledName, const TrAnyValue & newValue)
		{
			auto ret = GetSetFormulaValue(true, is_local, piledName, TT_NONE, 0, newValue);
			return (ret.type == TrAnyValue::Type::BOOLEAN && ret.num != 0.0);
		} // SetVariableValue.

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
// 21.05.07 Fukushiro M. 1行追加 ()
		using LuaFunc = int(*)(lua_State* luaState);
		using ClassObjFunc = T*&(*)();
	private:
		template<size_t S>
		struct LH
		{
			static void HookFunc(lua_State* L, lua_Debug* ar)
			{
				ClassObj()->OnCallLuaHook(L, ar);
			}

//----- 21.05.07 Fukushiro M. 追加始 ()-----
			static int DebugBreak(lua_State* L)
			{
				try
				{
					// Check the count of arguments.
					if (lua_gettop(L) != 0)
						throw std::invalid_argument("Count of arguments is not correct.");
					ClassObj()->SetScriptMode(SticktraceDef::Mode::SUSPEND);
					ClassObj()->ShowWindow(true);
				}
				catch (std::exception & e)
				{
					luaL_error(L, (std::string("C function error:::DebugBreak:") + e.what()).c_str());
				}
				catch (...)
				{
					luaL_error(L, "C function error:::DebugBreak");
				}
				return 0;
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
					Sticklib::check_lvalue<std::string>(message, L, 1);
					ClassObj()->OutputDebug(message.c_str());
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
//----- 21.05.07 Fukushiro M. 追加終 ()-----

			static T*& ClassObj()
			{
				static T* s_ClassObj = nullptr;
				return s_ClassObj;
			}
		};
	private:
		static std::vector<LuaToClassHook> & LUA_TO_CLASS_HOOK_STACK()
		{
//----- 21.05.07 Fukushiro M. 変更前 ()-----
//			static std::vector<LuaToClassHook> s_LuaToClassHookArray {
//				LuaToClassHook{ LH<0>::HookFunc, LH<0>::ClassObj },
//				LuaToClassHook{ LH<1>::HookFunc, LH<1>::ClassObj },
//				LuaToClassHook{ LH<2>::HookFunc, LH<2>::ClassObj },
//				LuaToClassHook{ LH<3>::HookFunc, LH<3>::ClassObj },
//				LuaToClassHook{ LH<4>::HookFunc, LH<4>::ClassObj },
//				LuaToClassHook{ LH<5>::HookFunc, LH<5>::ClassObj },
//				LuaToClassHook{ LH<6>::HookFunc, LH<6>::ClassObj },
//				LuaToClassHook{ LH<7>::HookFunc, LH<7>::ClassObj },
//				LuaToClassHook{ LH<8>::HookFunc, LH<8>::ClassObj },
//				LuaToClassHook{ LH<9>::HookFunc, LH<9>::ClassObj },
//			};
//----- 21.05.07 Fukushiro M. 変更後 ()-----
			static std::vector<LuaToClassHook> s_LuaToClassHookArray {
				LuaToClassHook{ LH<0>::HookFunc, LH<0>::DebugBreak, LH<0>::OutputDebugMessage, LH<0>::ClassObj },
				LuaToClassHook{ LH<1>::HookFunc, LH<1>::DebugBreak, LH<1>::OutputDebugMessage, LH<1>::ClassObj },
				LuaToClassHook{ LH<2>::HookFunc, LH<2>::DebugBreak, LH<2>::OutputDebugMessage, LH<2>::ClassObj },
				LuaToClassHook{ LH<3>::HookFunc, LH<3>::DebugBreak, LH<3>::OutputDebugMessage, LH<3>::ClassObj },
				LuaToClassHook{ LH<4>::HookFunc, LH<4>::DebugBreak, LH<4>::OutputDebugMessage, LH<4>::ClassObj },
				LuaToClassHook{ LH<5>::HookFunc, LH<5>::DebugBreak, LH<5>::OutputDebugMessage, LH<5>::ClassObj },
				LuaToClassHook{ LH<6>::HookFunc, LH<6>::DebugBreak, LH<6>::OutputDebugMessage, LH<6>::ClassObj },
				LuaToClassHook{ LH<7>::HookFunc, LH<7>::DebugBreak, LH<7>::OutputDebugMessage, LH<7>::ClassObj },
				LuaToClassHook{ LH<8>::HookFunc, LH<8>::DebugBreak, LH<8>::OutputDebugMessage, LH<8>::ClassObj },
				LuaToClassHook{ LH<9>::HookFunc, LH<9>::DebugBreak, LH<9>::OutputDebugMessage, LH<9>::ClassObj },
			};
//----- 21.05.07 Fukushiro M. 変更終 ()-----
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
//----- 21.05.07 Fukushiro M. 変更前 ()-----
//		LuaToClassHook()
//			: hook(nullptr)
//			, obj(nullptr)
//		{}
//
//		LuaToClassHook(LuaHookFunc h, ClassObjFunc c)
//			: hook(h)
//			, obj(c)
//		{}
//----- 21.05.07 Fukushiro M. 変更後 ()-----
		LuaToClassHook()
			: hook(nullptr)
			, debug_break(nullptr)
			, output_debug_message(nullptr)
			, obj(nullptr)
		{}

		LuaToClassHook(LuaHookFunc h, LuaFunc d, LuaFunc m, ClassObjFunc c)
			: hook(h)
			, debug_break(d)
			, output_debug_message(m)
			, obj(c)
		{}
//----- 21.05.07 Fukushiro M. 変更終 ()-----

		~LuaToClassHook() = default;

		void Clear()
		{
			hook = nullptr;
//----- 21.05.07 Fukushiro M. 追加始 ()-----
			debug_break = nullptr;
			output_debug_message = nullptr;
//----- 21.05.07 Fukushiro M. 追加終 ()-----
			obj = nullptr;
		}
	public:
		LuaHookFunc hook;
//----- 21.05.07 Fukushiro M. 追加始 ()-----
		LuaFunc debug_break;
		LuaFunc output_debug_message;
//----- 21.05.07 Fukushiro M. 追加終 ()-----
		ClassObjFunc obj;
	}; // class LuaToClassHook.

public:
	Sticktrace()
		: m_sticktraceWindow(nullptr)
		, m_stickrun(nullptr)
		, m_scriptHookFunc(nullptr)
		, m_scriptHookData(nullptr)
		, m_scriptHookInterval(0)
		, m_scriptHookCount(0)
		, m_suspendMode(SticktraceDef::Mode::STOP)
	{
	}

	~Sticktrace()
	{
		Terminate();
	}

private:
//----- 21.05.07 Fukushiro M. 削除始 ()-----
//	static std::unordered_map<lua_State*, Sticktrace*>& LUA_TO_TRACE()
//	{
//		static std::unordered_map<lua_State*, Sticktrace*> s_LUA_TO_TRACE;
//		return s_LUA_TO_TRACE;
//	}
//
//	/// <summary>
//	///
//	/// </summary>
//	static int OutputDebugMessage(lua_State* L)
//	{
//		try
//		{
//			// Check the count of arguments.
//			if (lua_gettop(L) != 1)
//				throw std::invalid_argument("Count of arguments is not correct.");
//
//			std::string message;
//			Sticklib::check_lvalue(message, L, 1);
//			LUA_TO_TRACE().at(L)->OutputDebug(message.c_str());
//		}
//		catch (std::exception & e)
//		{
//			luaL_error(L, (std::string("C function error:::OutputDebugMessage:") + e.what()).c_str());
//		}
//		catch (...)
//		{
//			luaL_error(L, "C function error:::OutputDebugMessage");
//		}
//		return 0;
//	}
//----- 21.05.07 Fukushiro M. 削除終 ()-----

public:
	/// <summary>
	/// Initializes the sticktrace instance. It must be called before a stickrun is attached.
	/// </summary>
	/// <param name="hwndParent">
	/// Parent window. This function creates the debugger window. The window deprives the focus despite it is hidden window. So this function gives the focus back to hwndParent.
	/// /NULL:Do not back the focus.
	/// </param>
	/// /nullptr or empty:Default value will be used.</param>
	/// <param name="companyName">Name of company. It's used to determine the name of registry./nullptr or empty:Default value will be used.</param>
	/// <param name="packageName">Name of package. It's used to determine the name of registry./nullptr:Default value will be used.</param>
	/// <param name="applicationName">Name of application. It's used to determine the name of registry./nullptr:Default value will be used.</param>
	/// <param name="dialogId">ID of Debugger window. It's used to determine the name of registry. You must specify different values if one application uses more than two debuggers.</param>
	/// <param name="DGT_debuggerCallbackFunc">
	/// Callback function called from the debugger window. It's called when script was saved./nullptr:No callback.
	/// Caution: The thread debugger window running in different from the main application's thread. So this function must be thread safe.
	/// </param>
	/// <param name="debuggerCallbackData">The user defined data which is passed to DGT_debuggerCallbackFunc.</param>
	void Initialize(
		HWND hwndParent,
		const wchar_t* companyName,
		const wchar_t* packageName,
		const wchar_t* applicationName,
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	)
	{
		if (m_sticktraceWindow == nullptr)
		{
			m_sticktraceWindow = SticktraceWindow::New(
				companyName,
				packageName,
				applicationName,
				dialogId,
				DGT_debuggerCallbackFunc,
				debuggerCallbackData
			);

			LuaToClassHook<Sticktrace>::Alloc(m_luaToClassHook, this);
			if (hwndParent != NULL)
				::SetForegroundWindow(hwndParent);
		}
	}

	/// <summary>
	/// Terminates this instance.
	/// Do not terminate the appliation if this function returns false.
	/// </summary>
	/// <returns>true:Succeeded/false:Lua script is suspending.</returns>
	bool Terminate()
	{
		DetachStickrun();
		if (m_sticktraceWindow != nullptr)
		{
			// The process cannot proceed when OnCallLuaHook function is running.
			// See the comment in OnCallLuaHook function.
			if (m_suspendMode == SticktraceDef::Mode::SUSPENDING)
				return false;

			LuaToClassHook<Sticktrace>::Free(m_luaToClassHook);
			m_luaToClassHook.Clear();
			SticktraceWindow::Delete(m_sticktraceWindow);
			m_sticktraceWindow = nullptr;
		}
		return true;
	}

	/// <summary>
	/// Initializes with the specified stickrun.
	/// </summary>
	/// <param name="stickrun">The stickrun.</param>
	/// <param name="scriptHookFunc">The script hook function. It is called when script is suspended and script is executed. See the comment described at OnCallLuaHook function.</param>
	/// <param name="scriptHookData">The user defined data which is passed to scriptHookFunc.</param>
	/// <param name="scriptHookInterval">The script hook interval. If scriptHookInterval is set to 10, scriptHookFunc is called once every 10 lines execution.</param>
	void AttachStickrun(
		Stickrun* stickrun,
		SticktraceDef::ScriptHookFunc scriptHookFunc,
		void* scriptHookData,
		size_t scriptHookInterval
	)
	{
		if (m_stickrun == nullptr)
		{
			m_stickrun = stickrun;
			m_stickrun->SetHook(Sticktrace::HookFunc, this);
			m_scriptHookFunc = scriptHookFunc;
			m_scriptHookData = scriptHookData;
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

//----- 21.05.07 Fukushiro M. 変更前 ()-----
//			// Register functions into the 'STICKTRACE' table.
//			//       STACK
//			//    |         |
//			//    |---------|
//			//  -1|  table  |-------------------------------+
//			//    |---------|      +------------+--------+  |
//			//  -2|   _G    |----->| Key        | Value  |  |
//			//    +---------+      |------------|--------|  |   +--------------------+------------------+
//			//                     |"STICKTRACE"| table  |--+-->| Key                | Value            |
//			//                     |------------|--------|      |--------------------|------------------|
//			//                     :            :        :      |"OutputDebugMessage"|OutputDebugMessage|
//			//                                                  |--------------------|------------------|
//			//                                                  :                    :                  :
//			static struct luaL_Reg funcs[] =
//			{
//				{ "OutputDebugMessage", OutputDebugMessage },
//				{ nullptr, nullptr },
//			};
//----- 21.05.07 Fukushiro M. 変更後 ()-----
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
			//                     :            :        :      |"DebugBreak"        |DebugBreak        |
			//                                                  |--------------------|------------------|
			//                                                  |"OutputDebugMessage"|OutputDebugMessage|
			//                                                  |--------------------|------------------|
			//                                                  :                    :                  :
// 21.05.08 Fukushiro M. 1行変更 ()
//			static struct luaL_Reg funcs[] =
			struct luaL_Reg funcs[] =
			{
				{ "DebugBreak", m_luaToClassHook.debug_break },
				{ "OutputDebugMessage", m_luaToClassHook.output_debug_message },
				{ nullptr, nullptr },
			};
//----- 21.05.07 Fukushiro M. 変更終 ()-----
			Sticklib::set_functions(stickrun->GetLuaState(), funcs);

			// Pop the "STICKTRACE" table and Global table.
			Sticklib::pop(stickrun->GetLuaState());
			Sticklib::pop(stickrun->GetLuaState());
		}
	}

	void DetachStickrun()
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
			m_scriptHookData = nullptr;
			m_scriptHookInterval = 0;
		}
	}

	void ShowWindow(bool show)
	{
		m_sticktraceWindow->Show(show);
	}

	bool IsWindowVisible()
	{
		bool isVisible;
		if (!m_sticktraceWindow->IsVisible(isVisible))
			return false;
		return isVisible;
	}

	bool IsScriptModified()
	{
		bool isModified;
		if (!m_sticktraceWindow->IsScriptModified(isModified))
			return false;
		return isModified;
	}

	bool OutputError(const char* message)
	{
		return m_sticktraceWindow->OutputError(message);
	}

	bool OutputDebug(const char* message)
	{
		return m_sticktraceWindow->OutputDebug(message);
	}

	bool SetScriptMode(SticktraceDef::Mode mode)
	{
		switch (mode)
		{
		case SticktraceDef::Mode::STOP:
		case SticktraceDef::Mode::RUN:
		case SticktraceDef::Mode::SUSPEND:
			if (m_suspendMode == SticktraceDef::Mode::STOP)
				return false;
			break;
		case SticktraceDef::Mode::PROCEED_NEXT:
			switch (m_suspendMode)
			{
			case SticktraceDef::Mode::STOP:
			case SticktraceDef::Mode::RUN:
				return false;
			}
			break;
		default:	// SticktraceDef::Mode::SUSPENDING.
			return false;
		}
		m_suspendMode = mode;
		return true;
	}

	SticktraceDef::Mode GetScriptMode() const
	{
		return m_suspendMode;
	}

//----- 20.06.14 Fukushiro M. 削除始 ()-----
//	bool Suspend()
//	{
//		m_suspendMode = SticktraceDef::Mode::SUSPEND;
//	}
//
//	bool Resume()
//	{
//		m_suspendMode = SticktraceDef::Mode::RUN;
//	}
//
//	bool ProceedToNext()
//	{
//		m_suspendMode = SticktraceDef::Mode::PROCEED_NEXT;
//	}
//----- 20.06.14 Fukushiro M. 削除終 ()-----

	Stickrun* GetStickrun() const
	{
		return m_stickrun;
	}

private:
	void NewSession()
	{
		m_sticktraceWindow->NewSession();
	}

	void OnStartExec(lua_State* L, SticktraceDef::ExecType execType)
	{
		if (m_sticktraceWindow->IsDebugMode())
			::lua_sethook(L, m_luaToClassHook.hook, LUA_MASKLINE, 0);

// 21.05.07 Fukushiro M. 1行削除 ()
//		LUA_TO_TRACE()[L] = this;

		m_sticktraceWindow->OnStart(execType);
		m_suspendMode = SticktraceDef::Mode::RUN;
		m_scriptHookCount = 0;

//----- 21.05.18 Fukushiro M. 追加始 (For AdvanceSoft ＆標準化予定)-----
		if (m_scriptHookFunc != nullptr)
			m_scriptHookFunc(L, nullptr, m_scriptHookData, m_suspendMode, this);
//----- 21.05.18 Fukushiro M. 追加終 (For AdvanceSoft ＆標準化予定)-----
	}

	void OnStopExec(lua_State* L, SticktraceDef::ExecType execType)
	{
// 21.05.07 Fukushiro M. 1行削除 ()
//		LUA_TO_TRACE().erase(L);

		::lua_sethook(L, nullptr, 0, 0);

		m_suspendMode = SticktraceDef::Mode::STOP;

//----- 21.05.18 Fukushiro M. 追加始 (For AdvanceSoft ＆標準化予定)-----
		if (m_scriptHookFunc != nullptr)
			m_scriptHookFunc(L, nullptr, m_scriptHookData, m_suspendMode, this);
//----- 21.05.18 Fukushiro M. 追加終 (For AdvanceSoft ＆標準化予定)-----

		m_sticktraceWindow->OnStop(execType);
		// m_sticktraceWindow->Jump(nullptr, -1);
	}

//----- 21.05.18 Fukushiro M. 追加始 ()-----
	void OnError(lua_State* L, const char* message)
	{
		if (m_scriptHookFunc != nullptr)
			m_scriptHookFunc(L, nullptr, m_scriptHookData, SticktraceDef::Mode::LUAERROR, this);
		OutputError(message);
	}
//----- 21.05.18 Fukushiro M. 追加終 ()-----

#if _DEBUG
	void PrintTable(lua_State* L)
	{
		lua_pushnil(L);

		while (lua_next(L, -2) != 0)
		{
			if (lua_isstring(L, -1))
				_RPTN(_CRT_WARN, "Type = string : Key = %s, Value = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
			else if (lua_isnumber(L, -1))
				_RPTN(_CRT_WARN, "Type = number : key = %s, value = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
			else if (lua_istable(L, -1))
				_RPTN(_CRT_WARN, "Type = table : key = %s\n", lua_tostring(L, -2));
			else
				_RPTN(_CRT_WARN, "Type = unknown : key = %s\n", lua_tostring(L, -2));

			lua_pop(L, 1);
		}
	}
#endif //_DEBUG

	struct VariableRec
	{
		int id;
		int valueType;
		std::string value;
	}; // struct VariableRec
	
//----- 20.06.04  削除始 ()-----
//	Stickobject GetLocalVariable(
//		lua_State* L,
//		lua_Debug* ar,
//		const std::string & varname,
//		std::unordered_map<std::string, int> & localVariableNameToId
//	)
//	{
//		if (localVariableNameToId.empty())
//		{
//			for (int id = 1;; id++)
//			{
//				// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
//				//
//				// ┌───────┐
//				// │ローカル変数値│<- nで指定されたローカル変数を積む
//				// ├───────┤
//				// │              │
//				// ├───────┤
//				// │              │
//				// ├───────┤
//				// ：              ：
//				//
//				const char* name = ::lua_getlocal(L, ar, id);
//				if (name == nullptr) break;
//				// Records local variable name to id.
//				// Two or more same name variable can be exist, so override the name to id hash.
//				if (strcmp(name, "(*temporary)") != 0)
//					localVariableNameToId[name] = id;
//				// lua_pop (lua_State *L, int n)
//				//
//				// ┌───────┐─┬
//				// │//////////////│  │
//				// ├───────┤  │
//				// │//////////////│  │ <- n個の要素を取り除く（n=3の場合）
//				// ├───────┤  │
//				// │//////////////│  │
//				// ├───────┤─┴
//				// │              │
//				// ├───────┤
//				// ：              ：
//				//
//				::lua_pop(L, 1);	// lua_getlocalで積んだ変数を削除
//			}
//		}
//		const auto i = localVariableNameToId.find(varname);
//		if (i != localVariableNameToId.end())
//		{
//			// lua_getlocal (lua_State *L, lua_Debug *ar, int n)
//			//
//			// ┌───────┐
//			// │ローカル変数値│<- nで指定されたローカル変数を積む
//			// ├───────┤
//			// │              │
//			// ├───────┤
//			// │              │
//			// ├───────┤
//			// ：              ：
//			//
//			::lua_getlocal(L, ar, i->second);
//			return Stickobject(L);
//		}
//		else
//		{
//			return Stickobject();
//		}
//	} // GetLocalVariable.
//----- 20.06.04  削除終 ()-----

	void EnumTable(
		std::vector<SticktraceDef::WatchInfo>& varIconIndentNameTypeValueArray,
		int indent,
		const std::string& piledName,
		TrAnyValue& parentTable,
		const std::unordered_set<std::string>& stExpandedName
	)
	{
		std::string icon;
		TrAnyValue key;
		TrAnyValue value;
		parentTable.BeginEnum();
		while (parentTable.NextEnum(key, value))
		{
			switch (value.type)
			{
			case TrAnyValue::TABLE:
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
				SticktraceDef::WatchInfo(
					icon,
					std::to_string(indent),
					key.GetValueAsString(),
					value.GetTypeAsString(),
					value.GetValueAsString()
				)
			);
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
	/// This function is called with the following order.
	///   lua_hook->LuaToClassHook->OnCallLuaHook.
	/// </summary>
	/// <param name="L">Lua state object.</param>
	/// <param name="ar">Lua debug object.</param>
	void OnCallLuaHook(lua_State* L, lua_Debug* ar)
	{
		//
		//    Application
		//         |
		//         |      Create
		//         |------------------------------------------------------------------------------> SticktraceWindow
		//         |                                                                                       |
		//         |      Start                                                                            |
		//         |---------------> Lua                                                                   |
		//         :                  |                                                                    |
		//         :             +--->|                                                                    |
		//         :             |    |                                                                    |
		//         :             |    |    Call                                                            |
		//         :             |    |------------> OnCallLuaHook                                         |
		//         :             |    :                    |                                               |
		//         :             |    :                    |                                               |
		//         :             |    :               +--->|                                               |
		//         :             |    :               |    |    Call                                       |
		//         :             |    :               |    |---------------> m_scriptHookFunc              |
		//         :             |    :               |    :                        |                      |
		//         :             |    :               |    :      DispatchMessage   |                      |
		//        A|<------------|----:---------------|----:------------------------V                      |
		//         |             |    :               |    :                                               |
		//         |             |    :               |    :                                               |
		//         |             |    :               |    :                                               |
		//        B|-------------|----:---------------|--->|                                               |
		//         :             |    :               |    |                                               |
		//         :             |    :               |    |      C: Get Command                           |
		//         :             |    :               |    |---------------------------------------------->|
		//         :             |    :               |    |                                               |
		//         :             |    :               +----|                                               |
		//         :             |    :                    |                                               |
		//         :             |    |<-------------------V                                               |
		//         :             |    |                                                                    |
		//         :             |    |                                                                    |
		//         :             +----|                                                                    |
		//         :                  |                                                                    |
		//
		//   |                                                                                |  |                    |
		//   |--------------------------------------------------------------------------------|  |--------------------|
		//                                 Application thread                                    SticktraceWindow thread
		//
		//
		//  The application can delete SticktraceWindow between A and B, but if it was deleted, C will be failed.
		//  So, the application must not delete SticktraceWindow between A and B.
		//

		auto command = SticktraceDef::SuspendCommand::NONE;
		try
		{
			if (m_suspendMode == SticktraceDef::Mode::STOP)
				throw std::runtime_error("Script execution was interrupted.");

			// Check the possibility of the breakpoint existing on the current line. This function responds fast.
			if (m_suspendMode == SticktraceDef::Mode::SUSPEND ||
				m_sticktraceWindow->IsBreakpoint(nullptr, ar->currentline - 1))
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
				if (m_suspendMode == SticktraceDef::Mode::SUSPEND ||
					m_sticktraceWindow->IsBreakpoint(ar->source, ar->currentline - 1))
				{
//----- 21.05.18 Fukushiro M. 追加始 ()-----
					if (m_scriptHookFunc != nullptr)
						m_scriptHookFunc(L, nullptr, m_scriptHookData, SticktraceDef::Mode::SUSPEND, this);
//----- 21.05.18 Fukushiro M. 追加終 ()-----
					m_suspendMode = SticktraceDef::Mode::SUSPENDING;
					StickString paramA;
					command = m_sticktraceWindow->WaitCommandIsSet(paramA, 0);
					// If Stop command, throw the lua exception and stop the running of the script.
					if (command == SticktraceDef::SuspendCommand::STOP)
						throw std::runtime_error("Script execution was interrupted.");
					// Notify the sticktrace window that the script is suspended.
					m_sticktraceWindow->OnSuspended();
					// Jump the source code editor to the suspended point.
					m_sticktraceWindow->Jump(ar->source, ar->currentline - 1);

					Stickvar stickvar(L, ar);
					while (m_suspendMode == SticktraceDef::Mode::SUSPENDING)
					{
						if (m_suspendMode == SticktraceDef::Mode::RUN || command == SticktraceDef::SuspendCommand::RESUME)
						{
							m_suspendMode = SticktraceDef::Mode::RUN;
							command = SticktraceDef::SuspendCommand::NONE;
						}
						else if (m_suspendMode == SticktraceDef::Mode::STOP || command == SticktraceDef::SuspendCommand::STOP)
						{
							m_suspendMode = SticktraceDef::Mode::STOP;
							command = SticktraceDef::SuspendCommand::STOP;
							throw std::runtime_error("Script execution was interrupted.");
						}
						else if (m_suspendMode == SticktraceDef::Mode::PROCEED_NEXT || command == SticktraceDef::SuspendCommand::PROCEED_NEXT)
						{
							m_suspendMode = SticktraceDef::Mode::SUSPEND;
							command = SticktraceDef::SuspendCommand::NONE;
						}
						else if (command == SticktraceDef::SuspendCommand::GET_VARIABLE)
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
							//  { "WARN",		 "0"	   "varE"	   "Error"		 "Error message"	},     <--- Case of parsing error.
							//  { "MEMBER",		 "2"	   "varF"	   "string"		"mine"	},
							//  { "VARIABLE",	 "0"	   "varG"	   "string"		"hello"	},
							// }

							command = SticktraceDef::SuspendCommand::NONE;

							std::vector<SticktraceDef::WatchInfo> varIconIndentNameTypeValueArray;
							std::vector<std::string> vTopLevelName;
							std::unordered_set<std::string> stExpandedName;
							std::string sandbox;
							auto receivedata = paramA.c_str();

							Stickutil::Unserialize(sandbox, receivedata);
							Stickutil::Unserialize(vTopLevelName, receivedata);
							Stickutil::Unserialize(stExpandedName, receivedata);
							for (size_t number = 0; number != vTopLevelName.size(); number++)
							{
								const auto strNumber = std::to_string(number);
								const auto & topvar = vTopLevelName[number];

								// try~catch must be exist inside of for-loop.
								try
								{
									auto xxx = stickvar.GetVariableValue(true, topvar.c_str());
									if (xxx.type == TrAnyValue::NONE)
									{
										if (sandbox.empty())
											xxx = stickvar.GetVariableValue(false, topvar.c_str());
										else
											xxx = stickvar.GetVariableValue(false, (sandbox + "." + topvar).c_str());
									}
									std::string icon;
									switch (xxx.type)
									{
									case TrAnyValue::TABLE:
										if (stExpandedName.find(strNumber + '\b' + topvar) == stExpandedName.end())
											icon = "CLOSED";
										else
											icon = "OPENED";
										break;
									default:
										icon = "VARIABLE";
										break;
									}
									varIconIndentNameTypeValueArray.emplace_back(
										SticktraceDef::WatchInfo(
											icon,
											"0",
											topvar,
											xxx.GetTypeAsString(),
											xxx.GetValueAsString()
										)
									);
									if (xxx.type == TrAnyValue::TABLE && icon == "OPENED")
									{
										EnumTable(varIconIndentNameTypeValueArray, 1, strNumber + '\b' + topvar, xxx, stExpandedName);
									}
								}
								catch (std::exception & e)
								{
									varIconIndentNameTypeValueArray.emplace_back(
										SticktraceDef::WatchInfo(
											"WARN",
											"0",
											topvar,
											"Error",
											std::string("Get variable : ") + e.what()
										)
									);
								}
								catch (...)
								{
									varIconIndentNameTypeValueArray.emplace_back(
										SticktraceDef::WatchInfo(
											"WARN",
											"0",
											topvar,
											"Error",
											"Unknown error : Get variable"
										)
									);
								}
							}
							std::string senddata;
							Stickutil::Serialize(senddata, varIconIndentNameTypeValueArray);
							m_sticktraceWindow->SetWatch(senddata.c_str());
						}
						else if (command == SticktraceDef::SuspendCommand::SET_VARIABLE)
						{
							try
							{
								command = SticktraceDef::SuspendCommand::NONE;

								std::string sandbox;
								std::string varName;
								__int32 type;
								std::string varValue;
								auto receivedata = paramA.c_str();
								Stickutil::Unserialize(sandbox, receivedata);
								Stickutil::Unserialize(varName, receivedata);
								Stickutil::Unserialize(type, receivedata);
								Stickutil::Unserialize(varValue, receivedata);

								TrAnyValue newValue;
								switch (type)
								{
								case LUA_TNIL:
									newValue.SetNil();
									break;
								case LUA_TBOOLEAN:
								{
									bool b = (varValue == "true");
									newValue.Set(b);
									break;
								}
								case LUA_TNUMBER:
								{
									double v = strtod(varValue.c_str(), nullptr);
									newValue.Set(v);
									break;
								}
								case LUA_TSTRING:
								{
									newValue.Set(varValue.c_str());
									break;
								}
								default:
									throw std::runtime_error("System error : SET_VARIABLE");
								}

								auto ret = stickvar.SetVariableValue(true, varName.c_str(), newValue);

								if (!ret)
								{
									if (sandbox.empty())
										stickvar.SetVariableValue(false, varName.c_str(), newValue);
									else
										stickvar.SetVariableValue(false, (sandbox + "." + varName).c_str(), newValue);
								}
								m_sticktraceWindow->SetVariableNotify(true);
							}
							catch (std::exception & e)
							{
								OutputError((std::string("Set variable : ") + e.what()).c_str());
								m_sticktraceWindow->SetVariableNotify(false);
							}
							catch (...)
							{
								OutputError("Unknown error : Set variable");
								m_sticktraceWindow->SetVariableNotify(false);
							}
						}
						else
						{
							if (m_scriptHookFunc != nullptr)
								m_scriptHookFunc(L, ar, m_scriptHookData, m_suspendMode, this);
							command = m_sticktraceWindow->WaitCommandIsSet(paramA, 100);
						}
					}
					// Notify the sticktrace window that the script is resumed.
					m_sticktraceWindow->OnResumed();
					m_sticktraceWindow->Jump(nullptr, -1);
				}
			}
			if (m_scriptHookFunc != nullptr)
			{
				if (m_scriptHookCount == m_scriptHookInterval)
				{
					m_scriptHookFunc(L, ar, m_scriptHookData, m_suspendMode, this);
					m_scriptHookCount = 0;
				}
				else
				{
					m_scriptHookCount++;
				}
			}
		}
		catch (std::exception & e)
		{
			luaL_error(L, e.what());
		}
		catch (...)
		{
			luaL_error(L, "Unknown error : OnCallLuaHook");
		}
	}

	void SetSource(const char * sandbox, const char * name, const char * source)
	{
		m_sticktraceWindow->SetSource(sandbox, name, source);
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
		{
			auto scriptInfo = (Stickrun::ScriptInfo *)data;
			((Sticktrace*)userData)->SetSource(
				scriptInfo->sandbox.c_str(),
				scriptInfo->name.c_str(),
				scriptInfo->source.c_str()
			);
			break;
		}
		case Stickrun::CallbackType::NEW_SESSION:
			((Sticktrace*)userData)->NewSession();
			break;
		case Stickrun::CallbackType::ON_START_EXEC_ON_LOAD:
			((Sticktrace*)userData)->OnStartExec(L, SticktraceDef::ExecType::ON_LOAD);
			break;
		case Stickrun::CallbackType::ON_STOP_EXEC_ON_LOAD:
			((Sticktrace*)userData)->OnStopExec(L, SticktraceDef::ExecType::ON_LOAD);
			break;
		case Stickrun::CallbackType::ON_START_EXEC_ON_CALL:
			((Sticktrace*)userData)->OnStartExec(L, SticktraceDef::ExecType::ON_CALL);
			break;
		case Stickrun::CallbackType::ON_STOP_EXEC_ON_CALL:
			((Sticktrace*)userData)->OnStopExec(L, SticktraceDef::ExecType::ON_CALL);
			break;
		case Stickrun::CallbackType::ON_ERROR:
// 21.05.18 Fukushiro M. 1行変更 ()
//			((Sticktrace*)userData)->OutputError((const char*)data);
			((Sticktrace*)userData)->OnError(L, (const char*)data);
			break;
		default:
			break;
		}
	}

private:
	SticktraceWindow* m_sticktraceWindow;
	Stickrun* m_stickrun;
	LuaToClassHook<Sticktrace> m_luaToClassHook;
	SticktraceDef::ScriptHookFunc m_scriptHookFunc;
	void* m_scriptHookData;

	/// <summary>
	/// Interval when m_scriptHookFunc is called by line executing.
	/// For example, m_scriptHookInterval = 10 then m_scriptHookFunc is called once every ten lines execution.
	/// </summary>
	size_t m_scriptHookInterval;
	
	/// <summary>
	/// Work variable. It's used when line execution.
	/// </summary>
	size_t m_scriptHookCount;

	SticktraceDef::Mode m_suspendMode;
	
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
};

template<typename T>
Sticktrace::_TrAnyValue<T>::_TrAnyValue()
	: type(_TrAnyValue::NONE)
	, num(0.0)
	, obj(Stickobject())
{}

#endif // _STICKTRACE
