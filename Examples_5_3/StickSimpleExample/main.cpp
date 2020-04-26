// main.cpp

// Do not include "Stickrun.h". It must be included via "Stick.h".
#include <iostream>
#include "Stick.h"

void myspace::Print(char const * message)
{
	std::cout << message << std::endl;
}

std::string myspace::Add(const std::string & v1, std::string const & v2)
{
	return v1 + v2;
}

void myspace::ArrayAdd(std::vector<std::wstring>& v1, const std::wstring & v2)
{
	for (auto & v : v1)
		v += v2;
}

void myspace::HashAdd(std::map<std::wstring, int>& v1, int v2)
{
	for (auto & kv : v1)
		kv.second += v2;
}

constexpr const char* MYSCRIPT = u8R"(
msg = ""

Test1 = function(s)
	local b = 10
	msg = myspace.Add(s, b .. "")
	return msg, b
end

Test2 = function()
	myspace.Print(msg)
end

Test3 = function()
	local a = { "a", "b", "c" }
	a = myspace.ArrayAdd(a, "-hello")
	for k,v in pairs(a) do
		myspace.Print(v)
	end
end

Test4 = function(val)
	local a = { a=10, b=20, c=30 }
	a = myspace.HashAdd(a, val)
	for k,v in pairs(a) do
		myspace.Print(k .. "=" .. v)
	end
end

)";

int main()
{
	Stickrun stickrun(luastick_init);
	std::string error_message;
	// Load script in a sandbox.
	stickrun.MakeSandboxEnv("SNDBX", true);
	stickrun.DoSandboxString(&error_message, "SNDBX", MYSCRIPT, "myscript");
	// Call Lua function 'Test1' using 'CallFunction'
	std::string a;
	int b;
	stickrun.CallFunction(&error_message, "SNDBX.Test1", Stickrun::Out(a), Stickrun::Out(b), "hello");
	// Call Lua function 'Test2'
	stickrun.CallFunction(&error_message, "SNDBX.Test2");
	// Call Lua function 'Test3'
	stickrun.CallFunction(&error_message, "SNDBX.Test3");
	// Call Lua function 'Test4'
	stickrun.CallFunction(&error_message, "SNDBX.Test4", 5);
}
