#pragma once

#include <functional>
#include <sstream>
#include <unordered_map>
#if defined(_CPLUS_LUA_)
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#else
#include "lua.hpp"
#endif
#include "lauxlib.h"
#include "lualib.h"
#include "Sticklib.h"

enum StickrunError : int
{
	STICKERR_FUNCTION_NOT_FOUND = 2000,
};

constexpr const char* STICKERR_FUNCTION_NOT_FOUND_MSG = "Function was not found";

class Stickrun
{
public:
	/// <summary>
	/// Maximum length of script name.
	/// </summary>
	static constexpr const size_t SCRIPT_NAME_MAX = 50;

protected:
	class Util
	{
	public:
		static std::string& TrimLeft(std::string& str)
		{
			std::string::iterator i;
			for (i = str.begin(); i != str.end(); i++)
			{
				if (*i != ' ' &&
					*i != '\t' &&
					*i != '\r' &&
					*i != '\n') break;
			}
			str.erase(str.begin(), i);
			return str;
		}

		static std::string& TrimRight(std::string& str)
		{
			int i;
			for (i = (int)str.length() - 1; i != -1; i--)
			{
				if (str.at(i) != ' ' &&
					str.at(i) != '\t' &&
					str.at(i) != '\r' &&
					str.at(i) != '\n') break;
			}
			str.erase(i + 1);
			return str;
		}

		static std::string& Trim(std::string& str)
		{
			Util::TrimLeft(str);
			Util::TrimRight(str);
			return str;
		}
	};

	template<typename T> class AutoBackup
	{
	public:
		AutoBackup (T& ver)
			{
				m_ver = &ver;
				m_backup = ver;
			}
		AutoBackup (T& ver, const T& newVal)
			{
				m_ver = &ver;
				m_backup = ver;
				*m_ver = newVal;
			}
		~AutoBackup ()
			{
				*m_ver = m_backup;
			}
		void operator = (const T& newVal)
			{
				*m_ver = newVal;
			}
	private:
		T*	m_ver;
		T	m_backup;
	}; // template<typename T> class AutoBackup.

	enum class EndCallType
	{
		EXEC_ON_LOAD,
		CALL_FUNCTION,
	};

public:
	enum class CallbackType
	{
		NEW_SESSION,			// Notify the beginning of new session to the callback function.
		ON_LOAD_SCRIPT,			// Called when script script was loaded.
		ON_START_EXEC_ON_LOAD,	// Called when script is going to start, on script loading.
		ON_STOP_EXEC_ON_LOAD,	// Called when script was stopped, on script loading.
		ON_START_EXEC_ON_CALL,	// Called when script is going to start, on function calling.
		ON_STOP_EXEC_ON_CALL,	// Called when script was stopped, on function calling.
		ON_ERROR,				// Called when script running encounter the error.
	};

	/// <summary>
	/// Callback data for Stickrun::CallbackType::ON_LOAD_SCRIPT command.
	/// </summary>
	struct ScriptInfo
	{
		ScriptInfo(
			const std::string& _sandbox,
			const std::string& _name,
			const std::string& _source
		)
			: sandbox(_sandbox)
			, name(_name)
			, source(_source)
		{}
		const std::string& sandbox;
		const std::string& name;
		const std::string& source;
	};

	/// <summary>
	/// <para> Callback function.                                           </para>
	/// <para> param callbackType: Specifies callback type.                 </para>
	/// <para> param data:                                                  </para>
	/// <para>   If callbackType=ON_ERROR then data returns an error message. (const char*)    </para>
	/// <para>   If callbackType=ON_LOAD_SCRIPT then data returns sandbox name, script name and source code.    </para>
	/// <para>     (const ScriptInfo*)                                      </para>
	/// <para>   If callbackType=ON_START_EXEC_XXX then data returns nullptr.   </para>
	/// <para>   callbackType=ON_STOP_EXEC_XXX then data returns nullptr.       </para>
	/// <para> param luaState: lua_State object.                            </para>
	/// <para> param userData: User data given when SetHook called.         </para>
	/// <para> param stickrun: This class object.                           </para>
	/// </summary>
	using HookFunc = void (*)(CallbackType callbackType, void* data, lua_State* luaState, void* userData, Stickrun* stickrun);

	/// <summary>
	/// Wrapper for return value. See the 'CallFunction' comments.
	/// </summary>
	class Out
	{
		friend class Stickrun;
	private:
		enum class Type
		{
			NONE,
			BOOLEAN,
			INT32,
			INT64,
			DOUBLE,
			STRING,
			POINTER,
			DBL_ARRAY,
			I64_ARRAY,
			STR_ARRAY,
			DBL_DBL_HASH,
			DBL_I64_HASH,
			DBL_STR_HASH,
			I64_DBL_HASH,
			I64_I64_HASH,
			I64_STR_HASH,
			STR_DBL_HASH,
			STR_I64_HASH,
			STR_STR_HASH,
		};
	public:
		Out() : type(Out::Type::NONE), data(nullptr) {}
		Out(bool & v) : type(Out::Type::BOOLEAN), data(&v) {}
		Out(__int32 & v) : type(Out::Type::INT32), data(&v) {}
		Out(__int64 & v) : type(Out::Type::INT64), data(&v) {}
		Out(double & v) : type(Out::Type::DOUBLE), data(&v) {}
		Out(std::string & v) : type(Out::Type::STRING), data(&v) {}
		Out(void* & v) : type(Out::Type::POINTER), data(&v) {}
		Out(std::vector<double> & v) : type(Out::Type::DBL_ARRAY), data(&v) {}
		Out(std::vector<__int64> & v) : type(Out::Type::I64_ARRAY), data(&v) {}
		Out(std::vector<std::string> & v) : type(Out::Type::STR_ARRAY), data(&v) {}
		Out(std::unordered_map<double, double> & v) : type(Out::Type::DBL_DBL_HASH), data(&v) {}
		Out(std::unordered_map<double, __int64> & v) : type(Out::Type::DBL_I64_HASH), data(&v) {}
		Out(std::unordered_map<double, std::string> & v) : type(Out::Type::DBL_STR_HASH), data(&v) {}
		Out(std::unordered_map<__int64, double> & v) : type(Out::Type::I64_DBL_HASH), data(&v) {}
		Out(std::unordered_map<__int64, __int64> & v) : type(Out::Type::I64_I64_HASH), data(&v) {}
		Out(std::unordered_map<__int64, std::string> & v) : type(Out::Type::I64_STR_HASH), data(&v) {}
		Out(std::unordered_map<std::string, double> & v) : type(Out::Type::STR_DBL_HASH), data(&v) {}
		Out(std::unordered_map<std::string, __int64> & v) : type(Out::Type::STR_I64_HASH), data(&v) {}
		Out(std::unordered_map<std::string, std::string> & v) : type(Out::Type::STR_STR_HASH), data(&v) {}
	private:
		Out::Type type;
		void* data;
	};

public:	
	/// <summary>
	/// Initializes a new instance of the <see cref="Stickrun"/> class.
	/// </summary>
	/// <param name="luastick_init_func">It's a lua initializing function. It is generated by LuaStick.exe.</param>
	Stickrun(std::function<void(lua_State*)> luastick_init_func)
	{
		m_lua_state = luaL_newstate();
		luaL_openlibs(m_lua_state);
		const auto top = lua_gettop(m_lua_state);
		// Gets debug.traceback.
		lua_getglobal(m_lua_state, "debug");
		lua_getfield(m_lua_state, -1, "traceback");
		m_traceback_func = lua_tocfunction(m_lua_state, -1);
		lua_settop(m_lua_state, top);
		m_hookFunc = nullptr;
		m_hookUserData = nullptr;
		m_is_executing = false;
		// Initializes Stickrun.
		luastick_init_func(m_lua_state);
	}
	
	/// <summary>
	/// Finalizes an instance of the <see cref="Stickrun"/> class.
	/// </summary>
	~Stickrun()
	{
		::lua_close(m_lua_state);
		m_lua_state = nullptr;
	}

public:	
	/// <summary>
	/// Sets the hook function.
	/// The function will be called when the following.
	/// 1. When script is loaded.
	/// 2. When script is going to start.
	/// 3. When script is stopped.
	/// 4. When running script encounters an error.
	/// </summary>
	/// <param name="hookFunc">The hook function.</param>
	/// <param name="userData">The user data.</param>
	void SetHook(Stickrun::HookFunc hookFunc, void* userData)
	{
		m_hookFunc = hookFunc;
		m_hookUserData = userData;
	}
	
	/// <summary>
	/// Gets lua_State object.
	/// </summary>
	/// <returns></returns>
	lua_State* GetLuaState() const
	{
		return m_lua_state;
	}
	
	/// <summary>
	/// Gets the user data given when the SetHook function was called.
	/// </summary>
	/// <returns></returns>
	void* GetHookUserData() const
	{
		return m_hookUserData;
	}

	/// <summary>
	/// Clear the error message.
	/// </summary>
	void ClearError()
	{
		m_error_message.clear();
	}

public:
	/// <summary>
	/// Notify the beginning of new session to the callback function.
	/// e.g. The callback function reset the error message window when getting this notification.
	/// </summary>
	void NewSession()
	{
		if (m_hookFunc != nullptr)
		{
			m_hookFunc(
				Stickrun::CallbackType::NEW_SESSION,
				nullptr,
				m_lua_state,
				m_hookUserData,
				this
			);
		}
	}

	/// <summary>
	/// <para>Loads and run the script.   </para>
	/// <para>If you call DoString twice using same name, the first source will be disappeared.     </para>
	/// </summary>
	/// <param name="error_message">Returns the error message. If nullptr is specified, the error message will be recorded in the member variable 'm_error_message'.</param>
	/// <param name="source">The source code.</param>
	/// <param name="name">Chunk name.</param>
	/// <returns>true:Succeeded/false:failed</returns>
	bool DoString(std::string* error_message, const std::string& source, const char* name)
	{
		return DoSandboxString(error_message, "", source, name);
	}

	/// <summary>
	/// Calls the script function.
	/// e.g. When there is the following Lua function. "number, string = funcA(number)
	/// double A; std::string B;
	/// stickrun.CallFunction(nullptr, "funcA", Stickrun::Out(A), Stickrun::Out(B), 12.3);
	/// e.g. If 'funcA' is a member of the table 'tableA'
	/// stickrun.CallFunction(nullptr, "tableA.funcA", Stickrun::Out(A), Stickrun::Out(B), 12.3);
	/// </summary>
	/// <param name="error_message">Returns the error message. If nullptr is specified, the error message will be recorded in the member variable 'm_error_message'.</param>
	/// <param name="funcname">The function name.</param>
	/// <param name="...args">Arguments.</param>
	/// <returns>true:Succeeded/false:failed</returns>
	template<typename ... Args>
	bool CallFunction(std::string* error_message, const char* funcname, Args&& ... args)
	{
		const auto is_executing = m_is_executing;
		const AutoBackup<bool> backup(m_is_executing, true);
		if (is_executing)
		{
			m_error_message = StickSystemErrorMessage("Application calls script from the inside of script.");
		}
		else
		{
			try
			{
				BeginCall();
				std::vector<std::string> items;
				Stickrun::Split(items, funcname, '.');
				PushFunc(items);

				//   |       |
				//   |-------|
				//  5| arg2  |
				//   |-------|
				//  4| arg1  |
				//   |-------|
				//  3| func  |
				//   |-------|
				//   :       :
				// Stickrun::pusharg(std::move(args)...);
				m_callDataStack.back().m_arg_count += stick_pusharg(m_lua_state, this, std::move(args)...);
				EndCall(Stickrun::EndCallType::CALL_FUNCTION, items.back());
			}
			catch (std::exception & e)
			{
				m_error_message = e.what();
				CancelCall();
			}
			catch (...)
			{
				m_error_message = StickSystemErrorMessage();
				CancelCall();
			}
		}
		if (!m_error_message.empty())
		{
			if (error_message != nullptr)
				*error_message = m_error_message;

			if (m_hookFunc != nullptr)
			{
				m_hookFunc(
					Stickrun::CallbackType::ON_ERROR,
					(void*)m_error_message.c_str(),
					m_lua_state,
					m_hookUserData,
					this
				);
			}
			return false;
		}
		else
		{
			return true;
		}
	}

	/// <summary>
	/// Check the script is executing or not.
	/// </summary>
	/// <returns>true:executing/false:not executing</returns>
	bool IsExecuting() const
	{
		return m_is_executing;
	}

	/// <summary>
	/// Makes a table for start a script in a sandbox.
	/// The table can be used as an environment for the sandbox script.
	/// This function makes a Lua table as a global variable.
	/// If is_copy_global=true, the table is clone of the global table.
	/// </summary>
	/// <param name="sandboxenv">The table name.</param>
	/// <param name="is_copy_global">true : copies global table.</param>
	void MakeSandboxEnv(const char * sandboxenv, bool is_copy_global)
	{
		// Do the following process with c++ API.
		// ------------------------------------
		// myenv = {}
		// for orig_key, orig_value in pairs(_ENV) do
		//   myenv[orig_key] = orig_value
		// end
		// ------------------------------------

		//       STACK
		//    |         |
		//    |---------|      +--------+--------+
		//  -1|   _G    |----->|  Key   | Value  |
		//    +---------+      |--------|--------|
		//                     :        :        :
		lua_pushglobaltable(m_lua_state);

		// Erase the old sandboxenv table from the global table.

		//       STACK
		//    |         |
		//    |---------|         +----------+--------+      +--------+--------+
		//  -1|   nil   |     +-->|  Key     | Value  |  +-->|  Key   | Value  |
		//    |---------|     |   +----------+--------+  |   +--------+--------+
		//  -2|   _G    |-----+   |  KeyA    | ValueA |  |   |  KeyA  | ValueA |
		//    |---------|         |----------|--------|  |   |--------|--------|
		//    :         :         |  KeyB    | ValueB |  |   |  KeyB  | ValueB |
		//                        |----------|--------|  |   |--------|--------|
		//                        |  KeyC    | ValueC |  |   |  KeyC  | ValueC |
		//                        |----------|--------|  |   +--------+--------+
		//                        |sandboxenv| table  |--+
		//                        +----------+--------+
		::lua_pushnil(m_lua_state);

		//       STACK            +--------+--------+      +- - - - + - - - -+
		//    |         |     +-->|  Key   | Value  |      :  Key   : Value  :
		//    |---------|     |   +--------+--------+      +- - - - + - - - -+
		//  -1|   _G    |-----+   |  KeyA  | ValueA |      :  KeyA  : ValueA :
		//    |---------|         |--------|--------|      :- - - - : - - - -:
		//    :         :         |  KeyB  | ValueB |      :  KeyB  : ValueB :
		//                        |--------|--------|      :- - - - : - - - -:
		//                        |  KeyC  | ValueC |      :  KeyC  : ValueC :
		//                        +--------+--------+      +- - - - + - - - -+
		//                                                    disposed soon
		lua_setfield(m_lua_state, -2, sandboxenv);

		//       STACK     +---------------------------+
		//    |         |  |                           |
		//    |---------|  |      +--------+--------+  |   +--------+--------+
		//  -1|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
		//    |---------|     |   +--------+--------+      +--------+--------+
		//  -2|   _G    |------   |  KeyA  | ValueA |
		//    |---------|         |--------|--------|
		//    :         :         |  KeyB  | ValueB |
		//                        |--------|--------|
		//                        |  KeyC  | ValueC |
		//                        +--------+--------+
		::lua_newtable(m_lua_state);

		if (is_copy_global)
		{
			//       STACK
			//    |         |
			//    |---------|  +---------------------------+
			//  -1|   nil   |  |                           |
			//    |---------|  |      +--------+--------+  |   +--------+--------+
			//  -2|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
			//    |---------|     |   +--------+--------+      +--------+--------+
			//  -3|   _G    |------   |  KeyA  | ValueA |
			//    |---------|         |--------|--------|
			//    :         :         |  KeyB  | ValueB |
			//                        |--------|--------|
			//                        |  KeyC  | ValueC |
			//                        +--------+--------+
			::lua_pushnil(m_lua_state);

			//       STACK
			//    |         |
			//    |---------|
			//  -1| ValueA  |
			//    |---------|  +---------------------------+
			//  -2|  KeyA   |  |                           |
			//    |---------|  |      +--------+--------+  |   +--------+--------+
			//  -3|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
			//    |---------|     |   +--------+--------+      +--------+--------+
			//  -4|   _G    |------   |  KeyA  | ValueA |
			//    |---------|         |--------|--------|
			//    :         :         |  KeyB  | ValueB |
			//                        |--------|--------|
			//                        |  KeyC  | ValueC |
			//                        +--------+--------+
			while (::lua_next(m_lua_state, -3) != 0)
			{
				//       STACK
				//    |         |
				//    |---------|
				//  -1|  KeyA   |
				//    |---------|
				//  -2| ValueA  |
				//    |---------|  +---------------------------+
				//  -3|  KeyA   |  |                           |
				//    |---------|  |      +--------+--------+  |   +--------+--------+
				//  -4|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
				//    |---------|     |   +--------+--------+      +--------+--------+
				//  -5|   _G    |-----+   |  KeyA  | ValueA |
				//    |---------|         |--------|--------|
				//    :         :         |  KeyB  | ValueB |
				//                        |--------|--------|
				//                        |  KeyC  | ValueC |
				//                        +--------+--------+
				::lua_pushvalue(m_lua_state, -2);

				//       STACK
				//    |         |
				//    |---------|
				//  -1| ValueA  |
				//    |---------|
				//  -2|  KeyA   |
				//    |---------|  +---------------------------+
				//  -3|  KeyA   |  |                           |
				//    |---------|  |      +--------+--------+  |   +--------+--------+
				//  -4|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
				//    |---------|     |   +--------+--------+      +--------+--------+
				//  -5|   _G    |-----+   |  KeyA  | ValueA |
				//    |---------|         |--------|--------|
				//    :         :         |  KeyB  | ValueB |
				//                        |--------|--------|
				//                        |  KeyC  | ValueC |
				//                        +--------+--------+
				::lua_insert(m_lua_state, -2);

				//       STACK
				//    |         |
				//    |---------|  +---------------------------+
				//  -1|  KeyA   |  |                           |
				//    |---------|  |      +--------+--------+  |   +--------+--------+
				//  -2|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
				//    |---------|     |   +--------+--------+      +--------+--------+
				//  -3|   _G    |-----+   |  KeyA  | ValueA |      |  KeyA  | ValueA |
				//    |---------|         |--------|--------|      +--------+--------+
				//    :         :         |  KeyB  | ValueB |
				//                        |--------|--------|
				//                        |  KeyC  | ValueC |
				//                        +--------+--------+
				::lua_settable(m_lua_state, -4);
			}

			//       STACK     +---------------------------+
			//    |         |  |                           |
			//    |---------|  |      +--------+--------+  |   +--------+--------+
			//  -1|  table  |--+  +-->|  Key   | Value  |  +-->|  Key   | Value  |
			//    |---------|     |   +--------+--------+      +--------+--------+
			//  -2|   _G    |-----+   |  KeyA  | ValueA |      |  KeyA  | ValueA |
			//    |---------|         |--------|--------|      |--------|--------|
			//    :         :         |  KeyB  | ValueB |      |  KeyB  | ValueB |
			//                        |--------|--------|      |--------|--------|
			//                        |  KeyC  | ValueC |      |  KeyC  | ValueC |
			//                        +--------+--------+      +--------+--------+
		}

		//       STACK            +----------+--------+      +--------+--------+
		//    |         |     +-->|  Key     | Value  |  +-->|  Key   | Value  |
		//    |---------|     |   +----------+--------+  |   +--------+--------+
		//  -1|   _G    |-----+   |  KeyA    | ValueA |  |   |  KeyA  | ValueA |
		//    |---------|         |----------|--------|  |   |--------|--------|
		//    :         :         |  KeyB    | ValueB |  |   |  KeyB  | ValueB |
		//                        |----------|--------|  |   |--------|--------|
		//                        |  KeyC    | ValueC |  |   |  KeyC  | ValueC |
		//                        |----------|--------|  |   +--------+--------+
		//                        |sandboxenv| table  |--+
		//                        +----------+--------+
		lua_setfield(m_lua_state, -2, sandboxenv);

		//       STACK            +----------+--------+      +--------+--------+
		//    |         |         |  Key     | Value  |  +-->|  Key   | Value  |
		//    |         |         +----------+--------+  |   +--------+--------+
		//    :         :         |  KeyA    | ValueA |  |   |  KeyA  | ValueA |
		//                        |----------|--------|  |   |--------|--------|
		//                        |  KeyB    | ValueB |  |   |  KeyB  | ValueB |
		//                        |----------|--------|  |   |--------|--------|
		//                        |  KeyC    | ValueC |  |   |  KeyC  | ValueC |
		//                        |----------|--------|  |   +--------+--------+
		//                        |sandboxenv| table  |--+
		//                        +----------+--------+
		lua_pop(m_lua_state, 1);
	}

	/// <summary>
	/// Removes the table for the sandbox.
	/// </summary>
	/// <param name="sandboxenv">The table name.</param>
	/// <returns></returns>
	void RemoveSandboxEnv(const char * sandboxenv)
	{
		// Do the following process with c++ API.
		// ------------------------------------
		// myenv = {}
		// for orig_key, orig_value in pairs(_ENV) do
		//   myenv[orig_key] = orig_value
		// end
		// ------------------------------------

		//       STACK
		//    |         |
		//    |---------|      +--------+--------+
		//  -1|   _G    |----->|  Key   | Value  |
		//    +---------+      |--------|--------|
		//                     :        :        :
		lua_pushglobaltable(m_lua_state);

		// Erase the old sandboxenv table from the global table.

		//       STACK
		//    |         |
		//    |---------|         +----------+--------+      +--------+--------+
		//    |   nil   |     +-->|  Key     | Value  |  +-->|  Key   | Value  |
		//    |---------|     |   +----------+--------+  |   +--------+--------+
		//  -1|   _G    |-----+   |  KeyA    | ValueA |  |   |  KeyA  | ValueA |
		//    |---------|         |----------|--------|  |   |--------|--------|
		//    :         :         |  KeyB    | ValueB |  |   |  KeyB  | ValueB |
		//                        |----------|--------|  |   |--------|--------|
		//                        |  KeyC    | ValueC |  |   |  KeyC  | ValueC |
		//                        |----------|--------|  |   +--------+--------+
		//                        |sandboxenv| table  |--+
		//                        +----------+--------+
		::lua_pushnil(m_lua_state);

		//       STACK            +--------+--------+      +- - - - + - - - -+
		//    |         |     +-->|  Key   | Value  |      :  Key   : Value  :
		//    |---------|     |   +--------+--------+      +- - - - + - - - -+
		//  -1|   _G    |-----+   |  KeyA  | ValueA |      :  KeyA  : ValueA :
		//    |---------|         |--------|--------|      :- - - - : - - - -:
		//    :         :         |  KeyB  | ValueB |      :  KeyB  : ValueB :
		//                        |--------|--------|      :- - - - : - - - -:
		//                        |  KeyC  | ValueC |      :  KeyC  : ValueC :
		//                        +--------+--------+      +- - - - + - - - -+
		//                                                    disposed soon
		lua_setfield(m_lua_state, -2, sandboxenv);

		//       STACK            +--------+--------+
		//    |         |         |  Key   | Value  |
		//    |         |         +--------+--------+
		//    |         |         |  KeyA  | ValueA |
		//    :         :         |--------|--------|
		//                        |  KeyB  | ValueB |
		//                        |--------|--------|
		//                        |  KeyC  | ValueC |
		//                        +--------+--------+
		//
		lua_pop(m_lua_state, 1);
	}

	/// <summary>
	/// Check the specified sandbox exists or not.
	/// In fact, this function checks existence of sandboxenv table in the global-table.
	/// </summary>
	/// <param name="sandboxenv">Environment table for the sandbox.</param>
	/// <returns>true:Exists/false:Not exist</returns>
	bool SandboxExists(const char * sandboxenv)
	{
		//       STACK
		//    |         |
		//    |---------|      +--------+--------+
		//  -1|   _G    |----->|  Key   | Value  |
		//    +---------+      |--------|--------|
		//                     :        :        :
		lua_pushglobaltable(m_lua_state);

		//           if sandboxenv exists               |           if sandboxenv does not exist
		//                                              |
		//        stack                                 |        stack
		//     |         |                              |     |         |
		//     |---------|                              |     |---------|
		//   -1| table-X |                              |   -1|   nil   |
		//     |---------|                              |     |---------|
		//   -2|   _G    |--------------+               |   -2|   _G    |--------------+
		//     |---------|              |               |     |---------|              |
		//     :         :              V               |     :         :              V
		//                      +----------+-------+    |                      +----------+-------+
		//                      |   Key    | Value |    |                      |   Key    | Value |
		//                      |----------|-------|    |                      |----------|-------|
		//                      |sandboxenv|table-X|    |                      :          :       :
		//                      |----------|-------|    |
		//                      :          :       :    |
		const auto result = lua_getfield(m_lua_state, -1, sandboxenv);

		//       stack
		//    |         |
		//    |         |
		//    :         :
		lua_pop(m_lua_state, 2);

		return (result == LUA_TTABLE);
	}

	/// <summary>
	/// <para>Loads and run the script.                                  </para>
	/// <para>This function executes the script in a sandbox.            </para>
	/// <para>If you call DoSandboxString twice using same name, the first source will be disappeared.     </para>
	/// </summary>
	/// <param name="error_message">Returns the error message. If nullptr is specified, the error message will be recorded in the member variable 'm_error_message'.</param>
	/// <param name="sandboxenv">Environment table for the sandbox. You can get it by calling MakeSandboxEnv function.</param>
	/// <param name="source">Script source.</param>
	/// <param name="name">Chunk name.</param>
	/// <returns>true:Succeeded/false:failed</returns>
	bool DoSandboxString(std::string* error_message, const std::string & sandboxenv, std::string source, const std::string & name)
	{
		const auto is_executing = m_is_executing;
		const AutoBackup<bool> backup(m_is_executing, true);
		if (is_executing)
		{
			m_error_message = StickSystemErrorMessage("Application calls script from the inside of script.");
		}
		else if (SCRIPT_NAME_MAX < name.length())
		{
			m_error_message = StickSystemErrorMessage("Script name is too long.");
		}
		else
		{
			try
			{
				// Front new line must be erased from source code. Because Lua load function trim the input source code automatically.
				// So if source code was not trimed previously, source code inside of Lua and inside of editor did not correspond.
				Util::Trim(source);
				std::string luaScript;
				if (sandboxenv.empty())
				{
					luaScript = source;
				}
				else
				{
					const std::string chunkname = name.empty() ? "nil" : std::string("\"") + name + "\"";
					luaScript = std::string() +
						R"(local func, msg1 = load([====[)" + source + R"(]====], )" + chunkname + R"(, 't', )" + sandboxenv + R"()
if not func then return msg1 end
local status, msg2 = pcall(func)
if not status then return msg2 end
return ''
)";

					// i.e.
					// if variables are:
					//   source="_OnC11=function() return 5+ end"
					//   sandbox="_CF"
					//   name=""
					// then luaScript is following.
					//   ------------[A]---------------------
					//   local func, msg1 = load([====[_OnC11=function() return 5+ end]====], nil, 't', _CF)
					//   if not func then return msg1 end
					//   local status, msg2 = pcall(func)
					//   if not status then return msg2 end
					//   return ''
					//   ------------------------------------
					//
					// Even if the script in the variable 'source' includes syntax error,
					// luaL_loadbufferx and EndCall succeeds.
					// Because the 'source' script is wrapped and the wrapping script doesn't include any errors.
				}
				BeginCall();

				//                                    stack
				//                     succeed   |              |
				//                     +----->   |--------------|
				//                     |        1|Lua chunk func| <-- compiled chunk of the loaded code.
				//       stack         |         +--------------+
				//  |              | --+
				//  |              | --+
				//  +--------------+   |              stack
				//                     |         |              |
				//                     +----->   |--------------|
				//                      error   1|error message |
				//                               +--------------+
				auto result = ::luaL_loadbufferx(m_lua_state, luaScript.c_str(), luaScript.length(), name.c_str(), nullptr);
				if (m_hookFunc != nullptr)
				{
					// Set the source code, even if syntax error.
//----- 22.09.06 Fukushiro M. 変更前 ()-----
//					m_hookFunc(
//						Stickrun::CallbackType::ON_LOAD_SCRIPT,
//						&ScriptInfo(sandboxenv, name, source),
//						m_lua_state,
//						m_hookUserData,
//						this
//						);
//----- 22.09.06 Fukushiro M. 変更後 ()-----
					ScriptInfo scriptInfo(sandboxenv, name, source);
					m_hookFunc(
						Stickrun::CallbackType::ON_LOAD_SCRIPT,
						&scriptInfo,
						m_lua_state,
						m_hookUserData,
						this
						);
//----- 22.09.06 Fukushiro M. 変更終 ()-----
				}
				if (result != 0)
				{	//----- if error -----
					// If the script was executed in the sandbox, the wrapping script doesn't cause any errors.
					if (::lua_type(m_lua_state, -1) == LUA_TSTRING)
						StickThrowScriptError(::lua_tostring(m_lua_state, -1));
					else
						StickThrowSystemError();
				}

				if (sandboxenv.empty())
				{	//----- When the script should be executed directly -----
					// Execute the script.
					EndCall(Stickrun::EndCallType::EXEC_ON_LOAD, name);
				}
				else
				{	//----- When the script should be executed in the sandbox -----
					std::string err_message;
					// Set a variable to receive the error message. Because the wrapping script returns an error message.
					AddOutArg(err_message);
					// Execute the script.
					EndCall(Stickrun::EndCallType::EXEC_ON_LOAD, name);
					if (!err_message.empty())
						m_error_message = err_message;	// Don't throw error because 'CancelCall' is called in the catch statement.
				}
			}
			catch (std::exception & e)
			{
				m_error_message = e.what();
				CancelCall();
			}
			catch (...)
			{
				m_error_message = StickSystemErrorMessage();
				CancelCall();
			}
		}

		if (!m_error_message.empty())
		{
			if (error_message != nullptr)
				*error_message = m_error_message;

			if (m_hookFunc != nullptr)
			{
				m_hookFunc(
					Stickrun::CallbackType::ON_ERROR,
					(void*)m_error_message.c_str(),
					m_lua_state,
					m_hookUserData,
					this
				);
			}
			return false;
		}
		else
		{
			return true;
		}
	}

	const std::string & ErrorMessage() const
	{
		return m_error_message;
	}

protected:
	template<typename ... Args>
	friend int stick_pusharg(lua_State* L, void* data, Stickrun::Out&& head, Args&& ... args);

	lua_State*		m_lua_state;
	lua_CFunction	m_traceback_func;
	HookFunc		m_hookFunc;
	void*			m_hookUserData;
	std::string		m_error_message;
	bool			m_is_executing;

	struct CallDataRec
	{
		CallDataRec() : m_stack_top(-1), m_arg_count(0) {}
		CallDataRec(int stack_top, int arg_count) : m_stack_top(stack_top), m_arg_count(arg_count) {}
		int m_stack_top;
		int m_arg_count;
		std::vector<Stickrun::Out> m_results;
	};
	std::vector<CallDataRec> m_callDataStack;

protected:
	Stickrun & BeginCall()
	{
		m_error_message.clear();
		m_callDataStack.emplace_back(CallDataRec(::lua_gettop(m_lua_state), 0));
		return *this;
	}

	Stickrun & CancelCall()
	{
		// In some case, CancelCall is called twice. So, check m_callDataStack is empty or not.
		if (!m_callDataStack.empty())
		{
			lua_settop(m_lua_state, m_callDataStack.back().m_stack_top);
			m_callDataStack.pop_back();
		}
		// m_error_message must not be cleared.
		return *this;
	}

	Stickrun & PushFunc(const std::vector<std::string> & items)
	{
		if (items.size() == 1)
		{	//----- if function is in the global table -----
			// Push the function on the stack.
			//
			//         |              |
			//         |--------------|
			//         |   function   |
			//         +--------------+
			lua_getglobal(m_lua_state, items[0].c_str());
			if (lua_type(m_lua_state, -1) != LUA_TFUNCTION)
				StickThrowRuntimeError(STICKERR_FUNCTION_NOT_FOUND, items[0].c_str());
		}
		else
		{
			//                            +------------------------------------------------+
			//                            |                               +-------------+  |
			//                            |            global             |             |  |
			//                            |   +-----------+-----------+   |             v  v
			//         |              |   |   |    Key    |   Value   |   |            table A
			//         |--------------|   |   |-----------|-----------|   |   +-----------+-----------+
			//       -1|   table A    |---+   :           :           :   |   |    Key    |   Value   |
			//         |--------------|       |-----------|-----------|   |   |-----------|-----------|
			//         :              :       | items[0]  |  table A  |---+   :           :           :
			//                                |-----------|-----------|       |-----------|-----------|
			//                                :           :           :       | items[i]  |  table B  |
			//                                                                |-----------|-----------|
			//                                                                :           :           :
			lua_getglobal(m_lua_state, items[0].c_str());
			if (lua_type(m_lua_state, -1) != LUA_TTABLE)
				StickThrowRuntimeError(STICKERR_FUNCTION_NOT_FOUND, items[0].c_str());

			for (int i = 1; i != items.size() - 1; i++)
			{
				// スタック（最後）に積まれたテーブル内のフィールドを検索してスタックに積む。
				//                        +------------------------------------------------+
				//                        |                               +-------------+  |
				//     |              |   |            global             |             |  |
				//     |--------------|   |   +-----------+-----------+   |             v  v
				//   -1|   table B    |   |   |    Key    |   Value   |   |            table A
				//     |--------------|   |   |-----------|-----------|   |   +-----------+-----------+
				//   -2|   table A    |---+   :           :           :   |   |    Key    |   Value   |
				//     |--------------|       |-----------|-----------|   |   |-----------|-----------|
				//     :              :       | items[0]  |  table A  |---+   :           :           :
				//                            |-----------|-----------|       |-----------|-----------|
				//                            :           :           :       | items[i]  |  table B  |
				//                                                            |-----------|-----------|
				//                                                            :           :           :
				::lua_getfield(m_lua_state, -1, items[i].c_str());
				if (::lua_type(m_lua_state, -1) != LUA_TTABLE)
					StickThrowRuntimeError(STICKERR_FUNCTION_NOT_FOUND, items[i].c_str());

				// 使わなくなったテーブル名をスタックから削除。
				//                        +------------------------------------------------------ - -
				//                        |                               +-------------+
				//                        |            global             |             |
				//                        |   +-----------+-----------+   |             v
				//     |              |   |   |    Key    |   Value   |   |            table A
				//     |--------------|   |   |-----------|-----------|   |   +-----------+-----------+
				//   -1|   table B    |---+   :           :           :   |   |    Key    |   Value   |
				//     |--------------|       |-----------|-----------|   |   |-----------|-----------|
				//     :              :       | items[0]  |  table A  |---+   :           :           :
				//                            |-----------|-----------|       |-----------|-----------|
				//                            :           :           :       | items[i]  |  table B  |
				//                                                            |-----------|-----------|
				//                                                            :           :           :
				lua_replace(m_lua_state, -2);		// This removes the foo table from the stack
			}
			// 最後の関数をスタックに積む。
			//                        +------------------------------------------------------ - -
			//                        |                               +-------------+
			//     |              |   |            global             |             |
			//     |--------------|   |   +-----------+-----------+   |             v
			//   -1|   function   |   |   |    Key    |   Value   |   |            table A
			//     |--------------|   |   |-----------|-----------|   |   +-----------+-----------+
			//   -2|   table B    |---+   :           :           :   |   |    Key    |   Value   |
			//     |--------------|       |-----------|-----------|   |   |-----------|-----------|
			//     :              :       | items[0]  |  table A  |---+   :           :           :
			//                            |-----------|-----------|       |-----------|-----------|
			//                            :           :           :       | items[i]  |  table B  |
			//                                                            |-----------|-----------|
			//                                                            :           :           :
			lua_getfield(m_lua_state, -1, items.back().c_str());
			if (lua_type(m_lua_state, -1) != LUA_TFUNCTION)
				StickThrowRuntimeError(STICKERR_FUNCTION_NOT_FOUND, items.back().c_str());

			// 使わなくなったテーブル名をスタックから削除。
			//
			//                                                        +-------------+
			//                                     global             |             |
			//                            +-----------+-----------+   |             v
			//     |              |       |    Key    |   Value   |   |            table A
			//     |--------------|       |-----------|-----------|   |   +-----------+-----------+
			//   -1|   function   |       :           :           :   |   |    Key    |   Value   |
			//     |--------------|       |-----------|-----------|   |   |-----------|-----------|
			//     :              :       | items[0]  |  table A  |---+   :           :           :
			//                            |-----------|-----------|       |-----------|-----------|
			//                            :           :           :       | items[i]  |  table B  |
			//                                                            |-----------|-----------|
			//                                                            :           :           :
			lua_replace(m_lua_state, -2);		// This removes the foo table from the sta
		}
		return *this;
	}

	template<typename T>
	Stickrun & AddInArg(const T& v)
	{
		// Stickrun::pusharg(v);
		m_callDataStack.back().m_arg_count += stick_pusharg(m_lua_state, (void*)this, v);
		return *this;
	}

	template<typename T>
	Stickrun & AddOutArg(T& v)
	{
		// Stickrun::pusharg(v);
		m_callDataStack.back().m_arg_count += stick_pusharg(m_lua_state, (void*)this, Stickrun::Out(v));
		return *this;
	}

	Stickrun & EndCall(EndCallType endCallType, const std::string & funcname)
	{
		Stickrun::CallbackType startCallbackType;
		Stickrun::CallbackType stopCallbackType;
		if (endCallType == Stickrun::EndCallType::EXEC_ON_LOAD)
		{
			startCallbackType = Stickrun::CallbackType::ON_START_EXEC_ON_LOAD;	// Called when script is going to start, on script loading.
			stopCallbackType = Stickrun::CallbackType::ON_STOP_EXEC_ON_LOAD;	// Called when script was stopped, on script loading.
		}
		else	// Stickrun::EndCallType::CALL_FUNCTION
		{
			startCallbackType = Stickrun::CallbackType::ON_START_EXEC_ON_CALL;	// Called when script is going to start, on function calling.
			stopCallbackType = Stickrun::CallbackType::ON_STOP_EXEC_ON_CALL;	// Called when script was stopped, on function calling.
		}

		try
		{
			if (m_hookFunc != nullptr)
			{
				m_hookFunc(
					startCallbackType,
					nullptr,
					m_lua_state,
					m_hookUserData,
					this
				);
			}

			Stickrun::Pcall();

			//     |              |
			//     |--------------|
			//   -1|   result 3   |
			//     |--------------|
			//   -2|   result 2   |
			//     |--------------|
			//   -3|   result 1   |
			//     |--------------|
			//     :              :
			//
//----- 18.06.18 Fukushiro M. 変更前 ( )-----
//			auto iStackIdx = -(int)m_results.size();
//			for (auto & ref : m_results)
//----- 18.06.18 Fukushiro M. 変更後 ( )-----
			auto iStackIdx = -(int)m_callDataStack.back().m_results.size();
			for (auto & ref : m_callDataStack.back().m_results)
//----- 18.06.18 Fukushiro M. 変更終 ( )-----
			{
				const auto retno = (int)m_callDataStack.back().m_results.size() + iStackIdx + 1;
				switch (lua_type(m_lua_state, iStackIdx))
				{
				case LUA_TSTRING:
					switch (ref.type)
					{
					case Stickrun::Out::Type::BOOLEAN:
						*(bool*)ref.data = (std::atof(::lua_tostring(m_lua_state, iStackIdx)) != 0.0) ? true : false;
						break;
					case Stickrun::Out::Type::INT32:
						*(__int32*)ref.data = std::atol(::lua_tostring(m_lua_state, iStackIdx));
						break;
					case Stickrun::Out::Type::INT64:
						*(__int64*)ref.data = std::atoll(::lua_tostring(m_lua_state, iStackIdx));
						break;
					case Stickrun::Out::Type::DOUBLE:
						*(double*)ref.data = std::atof(::lua_tostring(m_lua_state, iStackIdx));
						break;
					case Stickrun::Out::Type::STRING:
						*(std::string*)ref.data = ::lua_tostring(m_lua_state, iStackIdx);
						break;
					default:
						StickThrowRuntimeError(
							STICKERR_INCORRECT_ARG_TYPE,
							"Cannot convert string value to the output. funcname",
							funcname.c_str(),
							" returnno",
							retno
						);
					}
					break;
				case LUA_TNUMBER:
					switch (ref.type)
					{
					case Stickrun::Out::Type::BOOLEAN:
						*(bool*)ref.data = (::lua_tonumber(m_lua_state, iStackIdx) != 0.0) ? true : false;
						break;
					case Stickrun::Out::Type::INT32:
						*(__int32*)ref.data = (__int32)std::round(::lua_tonumber(m_lua_state, iStackIdx));
						break;
					case Stickrun::Out::Type::INT64:
						*(__int64*)ref.data = (__int64)std::round(::lua_tonumber(m_lua_state, iStackIdx));
						break;
					case Stickrun::Out::Type::DOUBLE:
						*(double*)ref.data = ::lua_tonumber(m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::STRING:
						*(std::string*)ref.data = std::to_string(::lua_tonumber(m_lua_state, iStackIdx));
						break;
					default:
						StickThrowRuntimeError(
							STICKERR_INCORRECT_ARG_TYPE,
							"Cannot convert number value to the output. funcname",
							funcname.c_str(),
							" returnno",
							retno
						);
					}
					break;
				case LUA_TBOOLEAN:
					switch (ref.type)
					{
					case Stickrun::Out::Type::BOOLEAN:
						*(bool*)ref.data = ::lua_toboolean(m_lua_state, iStackIdx) ? true : false;
						break;
					case Stickrun::Out::Type::INT32:
						*(__int32*)ref.data = (__int32)::lua_toboolean(m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::INT64:
						*(__int64*)ref.data = (__int64)::lua_toboolean(m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::DOUBLE:
						*(double*)ref.data = (double)::lua_toboolean(m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::STRING:
						*(std::string*)ref.data = std::to_string(::lua_toboolean(m_lua_state, iStackIdx));
						break;
					default:
						StickThrowRuntimeError(
							STICKERR_INCORRECT_ARG_TYPE,
							"Cannot convert boolean value to the output. funcname",
							funcname.c_str(),
							" returnno",
							retno
						);
					}
					break;
				case LUA_TTABLE:
					switch (ref.type)
					{
					case Stickrun::Out::Type::DBL_ARRAY:
						Sticklib::check_array<double>(*(std::vector<double> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::I64_ARRAY:
						Sticklib::check_array<__int64>(*(std::vector<__int64> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::STR_ARRAY:
						Sticklib::check_array<std::string>(*(std::vector<std::string> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::DBL_DBL_HASH:
						Sticklib::check_hash<double, double>(*(std::unordered_map<double, double> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::DBL_I64_HASH:
						Sticklib::check_hash<double, __int64>(*(std::unordered_map<double, __int64> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::DBL_STR_HASH:
						Sticklib::check_hash<double, std::string>(*(std::unordered_map<double, std::string> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::I64_DBL_HASH:
						Sticklib::check_hash<__int64, double>(*(std::unordered_map<__int64, double> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::I64_I64_HASH:
						Sticklib::check_hash<__int64, __int64>(*(std::unordered_map<__int64, __int64> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::I64_STR_HASH:
						Sticklib::check_hash<__int64, std::string>(*(std::unordered_map<__int64, std::string> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::STR_DBL_HASH:
						Sticklib::check_hash<std::string, double>(*(std::unordered_map<std::string, double> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::STR_I64_HASH:
						Sticklib::check_hash<std::string, __int64>(*(std::unordered_map<std::string, __int64> *)ref.data, m_lua_state, iStackIdx);
						break;
					case Stickrun::Out::Type::STR_STR_HASH:
						Sticklib::check_hash<std::string, std::string>(*(std::unordered_map<std::string, std::string> *)ref.data, m_lua_state, iStackIdx);
						break;
					default:
						StickThrowRuntimeError(
							STICKERR_INCORRECT_ARG_TYPE,
							"Cannot convert table to the output. funcname",
							funcname.c_str(),
							" returnno",
							retno
						);
					}
					break;
				default:
					StickThrowRuntimeError(
						STICKERR_INCORRECT_ARG_TYPE,
						"Unsupported type value was returned.funcname",
						funcname.c_str(),
						" returnno",
						retno
					);
				}
				iStackIdx++;
			}
// 18.06.18 Fukushiro M. 1行変更 ( )
//			m_results.clear();
			CancelCall();
		}
		catch (...)
		{
			if (m_hookFunc != nullptr)
			{
				try
				{
					m_hookFunc(
						stopCallbackType,
						nullptr,
						m_lua_state,
						m_hookUserData,
						this
					);
				}
				catch (...)
				{
				}
			}
			throw;
		}
		if (m_hookFunc != nullptr)
		{
			try
			{
				m_hookFunc(
					stopCallbackType,
					nullptr,
					m_lua_state,
					m_hookUserData,
					this
				);
			}
			catch (...)
			{
			}
		}
		return *this;
	}

	void Pcall()
	{
		//         |              |
		//         |--------------|
		//        5|arg2          |
		//         |--------------|
		//        4|arg1          |
		//         |--------------|
		//        3|func          |  <-- top_pos
		//         |--------------|
		//         :              :
		// lua_gettop=5, m_arg_count=2, top_pos=5-2 = 3
// 18.06.18 Fukushiro M. 1行変更 ( )
//		const auto top_pos = lua_gettop(m_lua_state) - m_arg_count;
		const auto top_pos = lua_gettop(m_lua_state) - m_callDataStack.back().m_arg_count;
		//         |              |
		//         |--------------|
		//        6|traceback_func|
		//         |--------------|
		//        5|arg2          |
		//         |--------------|
		//        4|arg1          |
		//         |--------------|
		//        3|func          |  <-- top_pos
		//         |--------------|
		//         :              :
		lua_pushcfunction(m_lua_state, m_traceback_func);

		//         |              |
		//         |--------------|
		//        6|arg2          |
		//         |--------------|
		//        5|arg1          |
		//         |--------------|
		//        4|func          |
		//         |--------------|
		//        3|traceback_func|  <-- top_pos
		//         |--------------|
		//         :              :
		lua_insert(m_lua_state, top_pos);

		//             no error                              if error
		//                                                |              |
		//                                                |--------------|
		//         |              |                      4|error message |
		//         |--------------|                       |--------------|
		//        3|traceback_func|  <-- top_pos         3|traceback_func|  <-- top_pos
		//         |--------------|                       |--------------|
		//         :              :                       :              :
// 18.06.18 Fukushiro M. 1行変更 ( )
//		const auto result = lua_pcall(m_lua_state, m_arg_count, m_results.size(), top_pos);
		const auto result = lua_pcall(m_lua_state, m_callDataStack.back().m_arg_count, (int)m_callDataStack.back().m_results.size(), top_pos);

		//             no error                              if error
		//
		//                                                |--------------|
		//        3|              |  <-- top_pos         3|error message |  <-- top_pos
		//         |--------------|                       |--------------|
		//         :              :                       :              :
		// Remove the traceback_func.
		lua_remove(m_lua_state, top_pos);

		// The lua_pcall function returns 0 in case of success or returns one of the following error codes:
		// LUA_ERRRUN: a runtime error.
		// LUA_ERRMEM: memory allocation error.For such errors, Lua does not call the error handler function.
		// LUA_ERRERR: error while running the error handler function.

		switch (result)
		{
		case LUA_OK:	// = 0.
			break;
//----- 20.04.25  変更前 ()-----
//		case LUA_YIELD:
//		case LUA_ERRRUN:
//		case LUA_ERRSYNTAX:
//		case LUA_ERRMEM:
//		case LUA_ERRGCMM:
//		case LUA_ERRERR:
//----- 20.04.25  変更後 ()-----
		default:
//----- 20.04.25  変更終 ()-----
			StickThrowScriptError(lua_tostring(m_lua_state, -1));
			break;
		}
	}

protected:

	/*
	void pusharg() {}

	template<typename ... Args>
	void pusharg(int&& head, Args&& ... args)
	{
		m_arg_count++;
		::lua_pushinteger(m_lua_state, head);
		Stickrun::pusharg(std::move(args)...);
	}

	template<typename ... Args>
	void pusharg(double&& head, Args&& ... args)
	{
		m_arg_count++;
		::lua_pushnumber(m_lua_state, head);
		Stickrun::pusharg(std::move(args)...);
	}

	template<typename ... Args>
	void pusharg(Stickrun::ClassObj&& head, Args&& ... args)
	{
		m_arg_count++;
		::lua_pushlightuserdata(m_lua_state, head.first);
		luaL_getmetatable(m_lua_state, head.second.c_str());
		::lua_setmetatable(m_lua_state, -2);
		Stickrun::pusharg(std::move(args)...);
	}

	template<typename T, typename ... Args>
	void pusharg(T*&& head, Args&& ... args)
	{
		m_arg_count++;
		auto classobj = stick_classobj(head);
		::lua_pushlightuserdata(m_lua_state, classobj.first);
		luaL_getmetatable(m_lua_state, classobj.second.c_str());
		::lua_setmetatable(m_lua_state, -2);
		Stickrun::pusharg(std::move(args)...);
	}

	template<typename ... Args>
	void pusharg(Stickrun::Out&& head, Args&& ... args)
	{
		m_results.emplace_back(head);
		Stickrun::pusharg(std::move(args)...);
	}
	*/

	/// <summary>
	/// Splits the source into items at the separator point.
	/// e.g.
	/// 'ABC:EFG::XYZ:' -> {"ABC","EFG","","XYZ",""}
	/// </summary>
	/// <param name="items">The items.</param>
	/// <param name="source">The source.</param>
	/// <param name="separator">The separator.</param>
	/// <returns></returns>
	static std::vector<std::string> & Split(std::vector<std::string> & items, const std::string & source, char separator)
	{
		items.clear();
		if (!source.empty())
		{
			size_t pos = 0;
			for (;;)
			{
				const auto nextpos = source.find(separator, pos);
				if (nextpos == std::string::npos) break;
				items.emplace_back(source.substr(pos, nextpos - pos));
				pos = nextpos + 1;
			}
			items.emplace_back(source.substr(pos));
		}
		return items;
	}
}; // class Stickrun.

/// <summary>
/// Push the argument into Lua stack. This function is termination of sequence of arguments interpreting.
/// stick_pusharg must be the global function. Must not be the member of the Stickrun class.
/// Because LuaStick generates some stick_pusharg functions for other classes.
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
inline int stick_pusharg(lua_State* L, void* data) { return 0; }

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, bool&& head, Args&& ... args)
{
	::lua_pushboolean(L, head ? 1 : 0);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, __int32&& head, Args&& ... args)
{
	::lua_pushinteger(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, __int64&& head, Args&& ... args)
{
	::lua_pushinteger(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, double&& head, Args&& ... args)
{
	::lua_pushnumber(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const char* && head, Args&& ... args)
{
	::lua_pushstring(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, void*&& head, Args&& ... args)
{
	::lua_pushlightuserdata(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, Stickrun::Out&& head, Args&& ... args)
{
	auto stickrun = (Stickrun*)data;
	stickrun->m_callDataStack.back().m_results.emplace_back(head);
	return stick_pusharg(L, data, std::move(args)...);
}

