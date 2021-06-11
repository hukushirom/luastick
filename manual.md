# Using LuaStick  

************************************************************  

## Structures of Directory  

**LuaStick**
- C\+\+ source code of LuaStick.exe. I wrote it using standard C\+\+11, as possible.  

**lua-5.3.5**
- lua-5.3.5 source code and Visual Studio project.

**lua-5.4.3**
- lua-5.4.3 Visual Studio project.

**Examples_5_3**  
- Examples for lua 5.3

**Examples_5_4**  
- Examples for lua 5.4

Followings are directories in the Examples_X_X.  

**StickSimpleExample**
- A simple C\+\+ program example exporting C\+\+ functions to Lua.  

**StickClassExample**
- A simple C\+\+ program example exporting C\+\+ class and namespace to Lua.  

**Sticktrace**
- A library of Lua debug tool. It enables to step through Lua code, set breakpoints and watch variables.  
- Alpha software.  
- Using MFC.  

<img src="https://github.com/hukushirom/luastick/blob/master/Examples_5_3/Sticktrace/image01.png">

**StickTest**
- A program example which provides Lua debug functions using Sticktrace library.  
- See Appendix 'How to debug' at the bottom of this document.

************************************************************  

## Softwares for building  

**LuaStick**
- Visual Studio 2017 C\+\+  
- No need Lua for building.  

**StickSimpleExample**
- Visual Studio 2017 C\+\+  
- Lua5.3  

**StickClassExample**
- Visual Studio 2017 C\+\+  
- Lua5.3  
- MFC  

**Sticktrace**
- Visual Studio 2017 C\+\+  
- MFC  

**StickTest**
- Visual Studio 2017 C\+\+  
- Lua5.3  

************************************************************  

## Building steps  

For Lua 5.3.5

1. Start Visual Studio 2017.  
2. Open the solution 'BuildAll.sln'.  
3. Build.  

For Lua 5.4.3

1. Open the solution 'Build_5_4.sln'.  
2. Build.  

************************************************************  

## Executing LuaStick  

Executes like the following command line.  
```  
>LuaStick.exe -out Stick -lang en extinsert.html Sticklib.h main.h srcx.h  
```  
**-out**&emsp;Specify the name of output files.
**-lang**&emsp;Specify the language. See 'Specify the language' below for further details.

Components example of application files and binding source files created by LuaStick.exe.  
```  
                                              +--------------------+ 
                                              |       Manual       | 
                                              |                    | 
                                              |   +------------+   |  Generate   
                                              |   | Stick.html |<--|-----------+  
                                              |   +------------+   |           |  
     +--------------------------------+       |   +------------+   |           |
     |        Your Application        |       |   | Stick.css  |   |           |
     |                                |       |   +------------+   |           |
     |   +----------+  +----------+   |       +--------------------+           |
     |   |Stickrun.h|  |Sticklib.h|---|---+                                    |
     |   +----------+  +----------+   |   |       +--------------+             |
     |   +----------+  +----------+   |   +------>|              |-------------+
     |   | main.cpp |  | main.h   |---|---------->| LuaStick.exe |------+  
     |   +----------+  +----------+   |   +------>|              |---+  |  
     |   +----------+  +----------+   |   |       +--------------+   |  |  
     |   | srcx.cpp |  | srcx.h   |---|---+                          |  |  
     |   +----------+  +----------+   |                              |  |  
     |   +----------+  +----------+   |      Generate                |  |  
     |   |liblua.lib|  | Stick.h  |<--|------------------------------+  |  
     |   +----------+  +----------+   |                                 |  
     |        :        +----------+   |      Generate                   |  
     |                 |Stick.cpp |<--|---------------------------------+  
     |                 +----------+   |  
     +--------------------------------+  
```  
**extinsert.html**
- HTML text that will be inserted in the HTML API manual.  
- Application developer must write them, but it does not need to be described in most cases.  

**Sticklib.h**
- C\+\+ libraries which are used in Stick.cpp. It must always be described in the parameter.  
- It exists in LuaStick/files directory.  
- Application developer does not have to modify it.  

**main.h, srcx.h**
- C\+\+ header files in which exporting functions are written.  
- Application developer must write them.  
- stick tag must be used for the exporting functions.  

**Stickrun.h**
- C\+\+ libraries which include functions for executing Lua.  
- It exists in LuaStick/files directory.  
- Application developer does not have to modify it.  

**Stick.h, Stick.cpp**
- C\+\+ source code which includes binding functions for exporting C\+\+ functions to Lua.  
- LuaStick.exe generates them.  
- Application developer does not have to modify it.  

**Stick.html**
- HTML file describes C\+\+ functions which are exported to Lua.  
- LuaStick.exe generates them.  
- Application developer can modify it, if needed.  

**Stick.css**
- Style sheet for Stick.html.  
- It exists in LuaStick/files directory.  
- Application developer can modify it, if needed.  

************************************************************  

## Tags LuaStick using  

Tags used by LuaStick must be start with three slashes "///".  
LuaStick uses the following tags.  
- \<stick\>
- \<param\>
- \<returns\>
- \<exception\>
- \<sticktype\>
- \<stickconv\>
- \<stickdef\>
- \<summary\>
- \<para\>
- \<code\>

If you want to export some functions, you must write <stick\>, \<param\>, and \<exception\>.  
If you use std::string, int, and other ordinary types, you don't have to write \<sticktype\>, \<stickconv\>, and \<stickdef\>.  
For manual output, write \<summary\>, \<para\>, and \<code\>.  

<br/><br/>

### \<stick\>  

Exports the function which exists just below the tag. You can use this with the following.  
- class  
- struct  
- namespace  
- enum  
- static const  
- constexpr  
- \#define  
- function  
- variable[^1]

[^1]: Only struct member variables can be exported. Class member variables, global variables cannot be exported.  

###### Example  

```  
/// <stick export="true" />  
class A  
{  
  :  
```  

#### Attributes  

##### export  

Required. Specifies "true" or "false".  
If "true" is specified, the function which exists under this tag will be exported.    

###### Example  

```  
/// <stick export="true" />  
namespace sample  
{  
  :  
```  

##### type  

Optional. You can change the exporting type of class, struct and namespace.  

- Specify for **class**  

    - type=**"inconstructible"** : Exports as a class which doesn't have any constructor. For the class created by factory method.  
    - type=**"struct"**：Exports as struct.  
    - type=**"namespace"**：Exports as namespace.  

- Specify for **struct**  

    - type=**"class"**：Exports as class.  
    - type=**"inconstructible"** : Exports as a class which doesn't have any constructor. For the class created by factory method.  
    - type=**"namespace"**：Exports as namespace.  

- Specify for **namespace**  

    - type=**"class"**：Exports as class.  

###### Example  

```  
/// <stick export="true" type="namespace" />  
class ABC  
{  
  :  
// The ABC class is exported to Lua as namespace. So, member functions of ABC must be static.  
```  

##### ctype  

Optional. It changes the type of constant. It's required for '\#define'.  

###### Example  

```  
/// <stick export="true" ctype="void*" />  
static const int* ARRAY;  
or
/// <stick export="true" ctype="void*" />  
constexpr int* ARRAY;  
// LuaStick cannot export int* to Lua. By declaring the type as void*, LuaStick becomes enable to export 'ARRAY' as lightuserdata.  

/// <stick export="true" ctype="char*" />  
#define ABC "hello"  
// LuaStick determines the type of #define. You have to declare the type by 'ctype'.  
```  

##### ltype  

Optional. It declares the type of Lua variable to which C\+\+ variable is converted.  

###### Example  

```  
/// <stick export="true" ltype="lightuserdata" />  
static const __int64 ABC=....;  
// Converts the type of constant 'ABC' to lightuserdata and export to Lua.  
```  

##### lname  

Optional. Specifies the name which is used in Lua script. You can specify it for the following.  
- class
- struct
- namespace
- enum
- static const
- constexpr
- \#define
- function

###### Example  

```  
/// <stick export="true" lname="HELLO" />  
static const char* ARRAY;  
// C++ 'ARRAY' is refered as 'HELLO' in Lua script.  

/// <stick export="true" lname="ABC1" />  
/// <param name="a" io="in"></param>  
void ABC(int a);  

/// <stick export="true" lname="ABC2" />  
/// <param name="a" io="in"></param>  
void ABC(const char* a);  
// Declares two polymorphic functions as different name for Lua.  
```  

##### super  

Optional. It changes the superclass of the class. If you want to omit the superclass, specify "-" .  

###### Example  

```  
class C2 : public C3 { ... }
/// <stick export="true" super="C3" />  
class C1 : public C2 { ... }
// Changes the superclass of 'C1' from 'C2' to 'C3'.  

/// <stick export="true" super="-" />  
class A1 : public A2 { ...  
// Declares 'A1' as a class which has no superclass.  
```  

<br/><br/>

### \<param\>  

It declares the parameter which is exported to Lua.  
It describes the parameter for HTML API manual.

###### Example  

```  
/// <stick export="true" />  
/// <param name="v1" io="inout">The value 1</param>  
/// <param name="v2" io="in">The value 2</param>  
/// <param name="obj" io="out" autodel="true">Create and return an object instance.</param>  
static void MyFunc(double & v1, int v2, MyObj *& obj);  
```  
#### Attributes  

##### name  

Required. Declares the name of the parameter.  

##### io  

Required. It declares the parameter receives or returns a value. Specifies one of the followings.
- "in" : It only receives a value.
- "out" : It only returns a value.
- "inout" : It receives and returns a value.

##### ctype  

Optional. It changes the type of the parameter.  

###### Example  

```  
/// <param name="array" io="in" ctype="void*"></param>  
static void MyFunc(const int* array);  
// LuaStick cannot export int* to Lua. By declaring the type as void*, LuaStick becomes enable to export 'array' as lightuserdata.  
```  

##### ltype  

Optional. It declares the type of Lua variable to which C\+\+ parameter is converted.  

###### Example  

```  
/// <param name="array" io="out" ltype="lightuserdata"></param>  
static void MyFunc(int*& array);  
// LuaStick cannot export int* to Lua. By declaring the type as void*, LuaStick becomes enable to export 'array' as lightuserdata.  
```  

##### autodel  

Optional. It's valid when io="out" and the variable is an instance of a class. The specified instance is deleted automatically by Lua.  

##### text content  

Optional. LuaStick outputs the text content to the HTML API manual. It describes the parameter.  

###### Example  

```  
/// <param name="array" io="out">Array of X</param>  
static void MyFunc(int*& array);  
// In the HTML API manual, 'Array of X' is described for the parameter 'array'. 
```  

<br/><br/>

### \<returns\>  

It decrares the return value which is exported to Lua.
It describes the return value of the function for HTML API manual.  

###### Example  

```  
/// <stick export="true" />  
/// <returns ctype="void*" ltype="lightuserdata">My value</returns>  
static int* MyFunc();  
```  

#### Attributes  

##### ctype  

Optional. It changes the type of return value.  

##### ltype  

Optional. It declares the type of return value when the function called in Lua script.  

##### autodel  

Optional. It's valid when the function returns an instance of a class. The specified instance is deleted automatically by Lua.  

##### text content  

Optional. LuaStick outputs the text content to the HTML API manual. It describes the return value of the function.  

###### Example  

```  
/// <returns>My value</returns>  
static int* MyFunc();  
// In the HTML API manual, 'My value' is described for the return value. 
```  

<br/><br/>

### \<exception\>  

It specifies the exception which the exported C\+\+ function throws. The exception must be declared with \<stickdef\> tag before.  

###### Example  

```  
/// <stickdef type="exception" cref="MyException*" message="e->GetMessage()" delete="e->Delete()" />  

/// <stick export="true" />  
/// <summary>  
/// </summary>  
/// <exception cref="MyException*"" />  
static void MyFunc();  
```  

#### Attributes  

##### cref  

Required. It specifies the type of the exception.  

<br/><br/>

### \<sticktype\>  

It declares a C\+\+ type and two functions:one function is to pull the decalred type of value from Lua stack, and another function is to push the value into Lua stack.  

###### Example  

```  
<sticktype name="boolean" ctype="bool" getfunc="Sticklib::check_lvalue" setfunc="Sticklib::push_lvalue" />  
```  

#### Attributes  

##### name  

Required. It declares the name of Lua type. It is used to output into HTML API manual. It must be unique.  

##### ctype  

Required. It declares the type of C\+\+.  

##### getfunc  

Required. It declares a function which pulls a value from Lua stack, whose type is specified by 'ctype'. Form of the function must be like the following.  
```
void function_name([ctype] & value, lua_State * L, int arg)  
```
- **value** : It is a variable whose type is specified by 'ctype'. It receives a return value.  
- **L** : Lua object.  
- **arg** : Order number of this argument in Lua stack. e.g. 1,2,...  

##### setfunc  

Required. It declares a function which pushes a value into Lua stack, whose type is specified by 'ctype'. Form of the function must be like the following.  
```
void function_name(lua_State * L, const [ctype] & value)  
```
- **L** : Lua object.  
- **value** : It is a variable whose type is specified by 'ctype'. The value will be pushed into Lua stack.  

<br/><br/>

### \<stickconv\>  

It declares two C\+\+ types and two functions:one function is a converter which converts first C\+\+ type to second C\+\+ type, and another function is a converter which converts second C\+\+ type to first C\+\+ type.  
LuaStick is able to convert one C\+\+ type value to another relaying more than two converters.  

###### Example  

```  
// Convert wchar_t* to std::string  
// [wchar_t*]---(wcharp_to_wstring)-->[std::wstring]---(wstring_to_string)-->[std::string]  

/// <stickconv type1="wchar_t*" type2="std::wstring" type1to2="wcharp_to_wstring" />  
/// <stickconv type1="std::wstring" type2="std::string" type1to2="wstring_to_string" />  

extern void wcharp_to_wstring(std::wstring& w, const wchar_t* p);
extern void wstring_to_string(std::string& w, const std::wstring & w);
```  

###### Example  

```  
// [__int32]---(T_to_U)-->[__int64]
// [__int32]<--(T_to_U)---[__int64]

/// <stickconv type1="__int64" type2="__int32" type1to2="T_to_U" type2to1="T_to_U" />  

template<typename T, typename U> static void T_to_U(U & u, T t)  
{ u = (U)t; }  
```  

#### Attributes  

##### type1  

Required. First C\+\+ type.  

##### type2  

Required. Second C\+\+ type.  

##### type1to2  

Optional. But if type2to1 is not specified, then type1to2 must be required.  
It specify the function which convert 'type1' value to 'type2' value. The form of it must be the following.  
```  
void function_name([type2] & v2, const [type1] & v1)  
```  
  or    
```  
void function_name([type2] & v2, [type1] v1)  
```  
- **v2** : A variable whose type is 'type2'. Destination of conversion.  
- **v1** : A variable whose type is 'type1'. Source of conversion.  

##### type2to1  

Optional. But if type1to2 is not specified, then type2to1 must be required.  
It specify the function which convert 'type2' value to 'type1' value. The form of it must be the following.  
```
void function_name([type1] & v1, const [type2] & v2)  
```
  or  
```
void function_name([type1] & v1, [type2] v2)  
```
- **v1** : A variable whose type is 'type1'. Destination of conversion.  
- **v2** : A variable whose type is 'type2'. Source of conversion.  

<br/><br/>

### \<stickdef\>  

It describes that how to get the error message from the specified exception, and how to delete it.  
The exception is specified in \<exception\> tag.  
But you don't have to describe 'std::exception', because it is supported by default.  

###### Example  

```  
/// <stickdef type="exception" cref="MyException*" message="e->GetMessage()" delete="e->Delete()" />  

/// <exception cref="MyException*"></exception>  
static void MyFunc()
{
    throw new MyException();
}  
```  

#### Attributes  

##### type  

Required. You must set 'exception'.  

##### cref  

Required. It specifies the type of exception.    

##### message  

Optional. It describes that how to get the error message from the exception object.  
The type of the error message must be 'std::string' or 'const char*'.  
The variable name of the exception object is fixed to 'e'.  

##### delete  

Optional. It describes how to delete the exception object.  
The variable name of the exception object is fixed to 'e'.  

<br/><br/>

### \<summary\>  

Describes functions, classes, constants and so on. LuaStick outputs the text content to the HTML API manual.  
You can use the \<summary\> tag instead of the \<stick\> tag, by writing the attributes that are used in the \<stick\> tag,

###### Example  

```  
/// <summary>  
/// This is a my function.  
/// </summary>  
static void MyFunc();  

/// <summary export="true" lname="MyFuncX">  
/// This is a my function 2.  
/// </summary>  
static void MyFunc2();  
```  

#### Attributes  

##### text content  

Optional. LuaStick outputs the text content to the HTML API manual. It describes functions, classes, constants and so on.  


###### Example  

```  
/// <summary>  
/// This is a my constant.  
/// </summary>  
constexpr int ABC = 10;  
// In the HTML API manual, 'This is a my constant' is described for the constant 'ABC'.  
```  

##### export  

Optional. See the explanation in the \<stick\> tag.

##### type  

Optional. See the explanation in the \<stick\> tag.

##### ctype  

Optional. See the explanation in the \<stick\> tag.

##### ltype  

Optional. See the explanation in the \<stick\> tag.

##### lname  

Optional. See the explanation in the \<stick\> tag.

##### super  

Optional. See the explanation in the \<stick\> tag.

<br/><br/>

### \<para\>  

The \<para\> tag is for use inside the \<summary\> tag.
You can insert a new line into the HTML API manual.

#### Attributes  

##### text content  

Optional. It describes one line content which is written in the HTML API manual.  

###### Example  

```  
/// <summary>  
/// <para>This function has following functions  </para>  
/// <para>  1. Print xxx.   </para>  
/// <para>  2. Save xxx.    </para>  
/// </summary>  
static void MyFunc();  

// The following is described for the chapter 'MyFunc' in the HTML API manual.  
This function has following functions  
1. Print xxx.  
2. Save xxx.  
```  

<br/><br/>

### \<code\>  

The \<code\> tag is for use inside the \<summary\> tag.
You can output the text content into the HTML API manual with the following form.
1. Outputs using monospaced font.  
2. Preserves the spaces and new lines.  

#### Attributes  

##### text content  

Optional. It describes content which is written in the HTML API manual.  

###### Example  

```  
/// <summary>  
/// <para>Program example for this function</para>  
/// <code>  
/// int x = 10;      // x is 10.
/// MyFunc(x);       // Call MyFunc.
/// </code>  
/// </summary>  
static void MyFunc(int x);  

The following explanation will be outputted into the HTML API manual.  

Program example for this function  
+-------------------------------------------------+  
| int x = 10;      // x is 10.                    |  
| MyFunc(x);       // Call MyFunc.                |  
+-------------------------------------------------+  
```  

************************************************************  

## Raw function  

You can define a function which has a lua_State* parameter.  
Return type of raw function must be void, and parameter must be lua_State* only.  

###### Example  

```  
/// <summary export="true">  
/// Raw function example  
/// </summary>  
/// <param io="in" name="L">lua_State object</<param>  
static void MyFunc(lua_State * L);  
```  

************************************************************  

## Specify the language  

Text contents in the \<summary\> and other tags can be described using more than two languages.  
If you describe contents using several languages, one language content which are specified with -lang option in the command-line is outputted into the HTML API manual.  

#### Syntax  

*language-code-1;* content-1 *language-code-2;* content-2...  

- The front of language-code must be top of the line, space or line feed.  
- language-code must be one of the codes that are used at the HTML lang attribute.  

#### Example  

```  
/// <summary export="true">  
/// en;
/// <para>Program example for this function</para>  
/// <code>  
/// int x = 10;      // x is 10.
/// MyFunc(x);       // Call MyFunc.
/// </code>  
/// ja;
/// <para>関数のプログラム例</para>  
/// <code>  
/// int x = 10;      // x は 10.
/// MyFunc(x);       // MyFuncの呼び出し
/// </code>  
/// </summary>  
/// <param io="in" name="x">en;Parameter x ja;パラメーター x</<param>
static void MyFunc(int x);  

Command line example  
+----------------------------------------------------+  
| >stick.exe -out Stick -lang en sticklib.h hoo.h    |  
| >stick.exe -out Stick -lang ja sticklib.h hoo.h    |  
+----------------------------------------------------+  
```  

************************************************************  

## Executing Lua  

You can execute Lua using Stickrun library.  

<br/><br/>

### Stickrun  

Constructor. Initializes Stickrun.  

#### Syntax  

Stickrun(std::function\<void(lua_State\*)\> luastick_init_func)  

#### Parameters  

##### luastick_init_func  

It is the function which exports C\+\+ functions to Lua.  
The actual name is 'luastick_init', it exists inside of Stick.cpp.  

#### Example  

```  
Stickrun stickrun(luastick_init);  
```  

<br/><br/>

### DoString  

Loads Lua script code and executes it.  

#### Syntax  

bool DoString(std::string* error_message, const std::string& source, const char* name)  

#### Parameters  

##### error_message  

If an error occurs during execution of the function, the error message will be set this variable. You can set nullptr instead of the variable.  

##### source  

Lua script code.  

##### name

Name of the chunk in which source is loaded.  

#### Return value  

- true : Succeeded  
- false : Failed  

#### Example  

```  
const std::string SCRIPT = "Test = function()\nPrint(msg)\nend";  
stickrun.DoString(nullptr, SCRIPT, "myscript");  
```  

<br/><br/>

### CallFunction  

Calls a function of Lua script.  

#### Syntax  

bool CallFunction(std::string* error_message, const char* funcname, Args&& ... args)  

#### Parameters  

##### error_message  

If an error occurs during execution of the function, the error message will be set this variable. You can set nullptr instead of the variable.  

##### funcname  

The name of the function.  

##### args  

Variables which receive the return value of the Lua function, or arguments of the Lua function.  
If you wish to receive the return value, use the following form.  
- Stickrun::Out([variable name])

#### Return value  

- true : Succeeded  
- false : Failed  

#### Example  

```  
## Lua script function.  
Test = function(a, b)  
    return a + 1, b .. ",world"  
end  

## C\+\+ code.  
int r1;  
std::string r2;  
stickrun.CallFunction(nullptr, "Test", Stickrun::Out(r1), Stickrun::Out(r2), 10, "hello");  
```  

<br/><br/>

### MakeSandboxEnv  

Creates a sandbox environment.  

#### Syntax  

void MakeSandboxEnv(const char* envname, bool is_copy_global)  

#### Parameters  

##### envname  

The name of the sandbox.  

##### is_copy_global  

- true : Copy the global environment into the sandbox environment.  
- false : Do not copy the global environment into the sandbox environment.  

If you wish to use functions, variables, and others in the global environment, you must set 'true' for 'is_copy_global'.  

#### Example  

```  
stickrun.MakeSandboxEnv("SNDBX", true);  
```  

<br/><br/>

### RemoveSandboxEnv  

Deletes the sandbox environment.  

#### Syntax  

void RemoveSandboxEnv(const char* envname)  

#### Parameters  

##### envname  

The name of the sandbox.  

#### Example  

```  
stickrun.RemoveSandboxEnv("SNDBX");  
```  

<br/><br/>

### DoSandboxString  

Loads Lua script code into the sandbox environment and executes it.  

#### Syntax  

bool DoSandboxString(std::string* error_message, const std::string& sandboxenv, std::string source, const char* name)  

#### Parameters  

##### error_message  

If an error occurs during execution of the function, the error message will be set this variable. You can set nullptr instead of the variable.  

##### sandboxenv  

The name of the sandbox.  

##### source  

Lua script code.  

##### name  

Name of the chunk in which source is loaded.  

#### Return value  

- true : Succeeded  
- false : Failed  

#### Example  

```  
const std::string SCRIPT = "Test = function()\nPrint(msg)\nend";  
stickrun.DoSandboxString(nullptr, "SANDBOX", SCRIPT, "myscript");  
stickrun.CallFunction(nullptr, "SANDBOX.Test");  
```  

<br/><br/>

### ErrorMessage  

Returns the error message when an error is occurred during the execution of the Lua script.  

#### Syntax  

const std::string & ErrorMessage() const  

#### Return value  

An error message.  

<br/><br/>

### ClearError  

Clears the error message stored in the Stickrun instance.  

#### Syntax  

void ClearError()  

<br/><br/>

### SetHook  

Set the callback function. For details, see the comment of 'HookFunc' in 'Stickrun.h'.  

#### Syntax  

void SetHook(Stickrun::HookFunc hookFunc, void* userData)  

#### Parameters  

##### hookFunc  

Callback function. To unset the callback, you must set nullptr.  

##### userData  

User defined data. Its value is passed to the argument of the 'hookFunc'.  

<br/><br/>

### GetHookUserData  

Returns the 'userData' which is set with 'SetHook' function.  

#### Syntax  

void* GetHookUserData() const  

#### Return value  

User defined data.  

<br/><br/>

### GetLuaState  

Returns Lua object.  

#### Syntax  

lua_State* GetLuaState() const  

#### Return value  

Lua object.  

************************************************************  

## Exportable C\+\+ type  

### Exported directly  

You can export the following C\+\+ types to Lua directly.  
They are defined using \<sticktype ...\> tag in 'Sticklib.h'.  
```  
+--------------------------------------------------------------------------------------------------------------------------+  
|    Lua typename                    Lua type                         C++ type                                             |  
|==========================================================================================================================|  
|    boolean                         boolean                          bool                                                 |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    integer                         integer                          __int64                                              |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    number                          number                           double                                               |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    string                          string                           std::string                                          |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    lightuserdata                   lightuserdata                    void*                                                |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    classobject                     classobject                      Sticklib::classobject (void*)                        |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    array<number>                   array<number> (=table)           std::vector<double>                                  |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    array<integer>                  array<integer> (=table)          std::vector<__int64>                                 |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    array<boolean>                  array<boolean> (=table)          std::vector<bool>                                    |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    array<string>                   array<string> (=table)           std::vector<std::string>                             |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    array<lightuserdata>            array<lightuserdata> (=table)    std::vector<void*>                                   |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<number,number>             table<number,number>             std::unordered_map<double,double>                    |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<number,integer>            table<number,integer>            std::unordered_map<double,__int64>                   |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<number,boolean>            table<number,boolean>            std::unordered_map<double,bool>                      |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<number,string>             table<number,string>             std::unordered_map<double,std::string>               |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<integer,number>            table<integer,number>            std::unordered_map<__int64,double>                   |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<integer,integer>           table<integer,integer>           std::unordered_map<__int64,__int64>                  |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<integer,boolean>           table<integer,boolean>           std::unordered_map<__int64,bool>                     |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<integer,string>            table<integer,string>            std::unordered_map<__int64,std::string>              |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<boolean,number>            table<boolean,number>            std::unordered_map<bool,double>                      |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<boolean,integer>           table<boolean,integer>           std::unordered_map<bool,__int64>                     |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<boolean,boolean>           table<boolean,boolean>           std::unordered_map<bool,bool>                        |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<boolean,string>            table<boolean,string>            std::unordered_map<bool,std::string>                 |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<string,number>             table<string,number>             std::unordered_map<std::string,double>               |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<string,integer>            table<string,integer>            std::unordered_map<std::string,__int64>              |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<string,boolean>            table<string,boolean>            std::unordered_map<std::string,bool>                 |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<string,string>             table<string,string>             std::unordered_map<std::string,std::string>          |  
|--------------------------------------------------------------------------------------------------------------------------|  
|    hash<string,any>                table<string,any>                std::unordered_map<std::string,Sticklib::AnyValue>   |  
+--------------------------------------------------------------------------------------------------------------------------+  
```  

### Exported via intermediate C\+\+ type  

You can export the following C\+\+ types to Lua relaying intermediate C\+\+ type.  
They are defined using \<stickconv ...\> tag in 'Sticklib.h'.  
```
+-----------------------------------------------------------------------------+  
|   C++ type <---------------------[converter]--------------> Lua type        |  
|=============================================================================|  
|   __int8                                                    integer         |  
|-----------------------------------------------------------------------------|  
|   __int16                                                   integer         |  
|-----------------------------------------------------------------------------|  
|   __int32                                                   integer         |  
|-----------------------------------------------------------------------------|  
|   int                                                       integer         |  
|-----------------------------------------------------------------------------|  
|   long                                                      integer         |  
|-----------------------------------------------------------------------------|  
|   short                                                     integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned __int8                                           integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned __int16                                          integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned __int32                                          integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned __int64                                          integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned int                                              integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned long                                             integer         |  
|-----------------------------------------------------------------------------|  
|   unsigned short                                            integer         |  
|-----------------------------------------------------------------------------|  
|   size_t                                                    integer         |  
|-----------------------------------------------------------------------------|  
|   float                                                     number          |  
|-----------------------------------------------------------------------------|  
|   BOOL                                                      boolean         |  
|-----------------------------------------------------------------------------|  
|   std::wstring                                              string          |  
|-----------------------------------------------------------------------------|  
|   char*                                                     string          |  
|-----------------------------------------------------------------------------|  
|   wchar_t*                                                  string          |  
|-----------------------------------------------------------------------------|  
|   std::vector<__int8>                                       array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<__int16>                                      array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<__int32>                                      array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<int>                                          array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<long>                                         array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<short>                                        array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned __int8>                              array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned __int16>                             array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned __int32>                             array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned __int64>                             array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned int>                                 array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned long>                                array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<unsigned short>                               array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<size_t>                                       array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<float>                                        array<number>   |  
|-----------------------------------------------------------------------------|  
|   std::vector<BOOL>                                         array<boolean>  |  
|-----------------------------------------------------------------------------|  
|   std::vector<std::wstring>                                 array<string>   |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<__int64>                               array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<double>                                array<number>   |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<std::string>                           array<string>   |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<void*>                                 array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<__int8>                                array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<__int16>                               array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<__int32>                               array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<int>                                   array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<long>                                  array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<short>                                 array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned __int8>                       array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned __int16>                      array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned __int32>                      array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned __int64>                      array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned int>                          array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned long>                         array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<unsigned short>                        array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<size_t>                                array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<float>                                 array<number>   |  
|-----------------------------------------------------------------------------|  
|   std::unordered_set<std::wstring>                          array<string>   |  
|-----------------------------------------------------------------------------|  
|   std::set<__int64>                                         array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<double>                                          array<number>   |  
|-----------------------------------------------------------------------------|  
|   std::set<std::string>                                     array<string>   |  
|-----------------------------------------------------------------------------|  
|   std::set<void*>                                           array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<__int8>                                          array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<__int16>                                         array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<__int32>                                         array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<int>                                             array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<long>                                            array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<short>                                           array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned __int8>                                 array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned __int16>                                array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned __int32>                                array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned __int64>                                array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned int>                                    array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned long>                                   array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<unsigned short>                                  array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<size_t>                                          array<integer>  |  
|-----------------------------------------------------------------------------|  
|   std::set<float>                                           array<number>   |  
|-----------------------------------------------------------------------------|  
|   std::set<std::wstring>                                    array<string>   |  
+-----------------------------------------------------------------------------+  
```

************************************************************  

## Structures of binding code  

#### Global functions  

```  
 C++  
+----------------------------------+  
| extern int FuncX(int a);         |  
+----------------------------------+  


           Global table              Stick.cpp  
    +----------+-----------+        +-----------------------------------------------+  
    |   Key    |   Value   |        |    +--------------------------------------+   |  
    |----------+-----------|   +-------->|  void __FuncX__(lua_State* L)        |   |  
    :          :           :   |    |    |  {                                   |   |  
    |----------|-----------|   |    |    |      int a = luaL_checkinteger(L);   |   |  
    | "FuncX"  | FUNCTION -----+    |    |      int ret = FuncX(a);             |   |  
    |----------|-----------|        |    |      lua_pushinteger(L, ret);        |   |  
    :          :           :        |    |  }                                   |   |  
                                    |    +--------------------------------------+   |  
                                    |                        :                      |  
                                    +-----------------------------------------------+  
```  

#### Static member functions  

```  
 C++  
+----------------------------------+  
| class ClassA                     |  
| {                                |  
|     class ClassB                 |  
|     {                            |  
|         static int FuncX(int a); |  
+----------------------------------+  


           Global table              namespace ClassA table          namespace ClassB table          Stick.cpp  
    +----------+-----------+        +----------+-----------+        +----------+-----------+        +-----------------------------------------------+  
    |   Key    |   Value   |   +--->|   Key    |   Value   |   +--->|   Key    |   Value   |        |    +--------------------------------------+   |  
    |----------+-----------|   |    |----------+-----------|   |    |----------+-----------|   +-------->|  void __FuncX__(lua_State* L)        |   |  
    :          :           :   |    :          :           :   |    :          :           :   |    |    |  {                                   |   |  
    |----------|-----------|   |    |----------|-----------|   |    |----------|-----------|   |    |    |      int a = luaL_checkinteger(L);   |   |  
    | "ClassA" |   TABLE  -----+    | "ClassB" |   TABLE  -----+    | "FuncX"  | FUNCTION -----+    |    |      int ret = FuncX(a);             |   |  
    |----------|-----------|        |----------|-----------|        |----------|-----------|        |    |      lua_pushinteger(L, ret);        |   |  
    :          :           :        :          :           :        :          :           :        |    |  }                                   |   |  
                                                                                                    |    +--------------------------------------+   |  
                                                                                                    |                        :                      |  
                                                                                                    +-----------------------------------------------+  
```  

#### Constants  

```  
 C++  
+----------------------------------------+  
| class ClassA                           |  
| {                                      |  
|    static const int VarX = [valueX];   |  
|      or                                |  
|    constexpr int VarX = [valueX];      |  
+----------------------------------------+  


           Global table              namespace ClassA table  
    +----------+-----------+        +----------+-----------+  
    |   Key    |   Value   |   +--->|   Key    |   Value   |  
    |----------+-----------|   |    |----------+-----------|  
    :          :           :   |    :          :           :  
    |----------|-----------|   |    |----------|-----------|  
    | "ClassA" |   TABLE  -----+    |  "VarX"  | [valueX]  |  
    |----------|-----------|        |----------|-----------|  
    :          :           :        :          :           :  
```  

#### Enumurates  

```  
 C++  
+----------------------------------+  
| class ClassA                     |  
| {                                |  
|     enum class EnumB             |  
|     {                            |  
|         ITEM_1,                  |  
|         ITEM_2,                  |  
+----------------------------------+  


           Global table              namespace ClassA table               EnumB table  
    +----------+-----------+        +----------+-----------+        +----------+-----------+  
    |   Key    |   Value   |   +--->|   Key    |   Value   |   +--->|   Key    |   Value   |  
    |----------+-----------|   |    |----------+-----------|   |    |----------+-----------|  
    :          :           :   |    :          :           :   |    | "ITEM_1" |  ITEM_1   |  
    |----------|-----------|   |    |----------|-----------|   |    |----------|-----------|  
    | "ClassA" |   TABLE  -----+    | "EnumB"  |   TABLE  -----+    | "ITEM_2" |  ITEM_2   |  
    |----------|-----------|        |----------|-----------|        |----------|-----------|  
    :          :           :        :          :           :        :          :           :  
```  

#### Classes  

```  
 C++  
+----------------------------------+  
| class ClassA                     |  
| {                                |  
|     class ClassB                 |  
|     {                            |  
|         int FuncX(int a);        |  
+----------------------------------+  


           Global table              namespace ClassA table          namespace ClassB table          Stick.cpp  
    +----------+-----------+        +----------+-----------+        +----------+-----------+        +-------------------------------------------------------------------+  
    |   Key    |   Value   |   +--->|   Key    |   Value   |   +--->|   Key    |   Value   |        |    +----------------------------------------------------------+   |  
    |----------+-----------|   |    |----------+-----------|   |    |----------+-----------|    +------->|  void __ClassB_New__(lua_State* L)                       |   |  
    :          :           :   |    :          :           :   |    :          :           :    |   |    |  {                                                       |   |  
    |----------|-----------|   |    |----------|-----------|   |    |----------|-----------|    |   |    |      auto w = (StickInstanceWrapper*)lua_newuserdata(L); |   |  
    | "ClassA" |   TABLE  -----+    | "ClassB" |   TABLE  -----+    |  "New"   | FUNCTION ------+   |    |      w->ptr = new ClassA::ClassB();                      |   |  
    |----------|-----------|        |----------|-----------|        |----------|-----------|        |    |      lua_setmetatable(L, "__ClassB__");                  |   |  
    :          :           :        |  "New"   | FUNCTION -----+    :          :           :        |    |  }                                                       |   |  
                                    |----------|-----------|   |                                    |    +----------------------------------------------------------+   |  
                                    :          :           :   |                                    |    +----------------------------------------------------------+   |  
                                                               +---------------------------------------->|  void __ClassA_New__(lua_State* L)                       |   |  
                                                                                                    |    |  {                                                       |   |  
                                                                                                    |    |      :                                                   |   |  
                                                                                                    |    |  }                                                       |   |  
                                           registry                    ClassB metatable             |    +----------------------------------------------------------+   |  
                                  +------------+-----------+        +----------+-----------+        |    +----------------------------------------------------------+   |  
                                  |   Key      |   Value   |   +--->|   Key    |   Value   |    +------->|  void __ClassB_Destructor__(lua_State* L)                |   |  
                                  |------------+-----------|   |    |----------+-----------|    |   |    |  {                                                       |   |  
                                  :            :           :   |    |  "__gc"  | FUNCTION ------+   |    |      auto w = (StickInstanceWrapper*)luaL_testudata(L);  |   |  
                                  |------------|-----------|   |    |----------|-----------|        |    |      delete (ClassA::ClassB*)w->ptr;                     |   |  
                                  |"__ClassB__"|   TABLE  -----+    | "FuncX"  | FUNCTION -+----+   |    |  }                                                       |   |  
                                  |------------|-----------|        |----------|-----------|    |   |    +----------------------------------------------------------+   |  
                                  |"__ClassA__"|   TABLE  -----+    :          :           :    |   |    +----------------------------------------------------------+   |  
                                  |------------|-----------|   |                                +------->|  void __ClassB_FuncX__(lua_State* L)                     |   |  
                                  :            :           :   |       ClassA metatable             |    |  {                                                       |   |  
                                                               |    +----------+-----------+        |    |      auto w = (StickInstanceWrapper*)luaL_testudata(L);  |   |  
                                                               +--->|   Key    |   Value   |        |    |      auto p = (ClassA::ClassB*)w->ptr;                   |   |  
                                                                    |----------+-----------|        |    |      int a = luaL_checkinteger(L);                       |   |  
                                                                    |  "__gc"  | FUNCTION ------+   |    |      int ret = p->FuncX(a);                              |   |  
                                                                    |----------|-----------|    |   |    |      lua_pushinteger(L, ret);                            |   |  
                                                                    :          :           :    |   |    |  }                                                       |   |  
                                                                                                |   |    +----------------------------------------------------------+   |  
                                                                                                |   |    +----------------------------------------------------------+   |  
                                                                                                +------->|  void __ClassA_Destructor__(lua_State* L)                |   |  
                                                                                                    |    |  {                                                       |   |  
                                                                                                    |    |      :                                                   |   |  
                                                                                                    |    |  }                                                       |   |  
                                                                                                    |    +----------------------------------------------------------+   |  
                                                                                                    |                                :                                  |  
                                                                                                    +-------------------------------------------------------------------+  
```  

#### Class inheritance  

```  
 C++  
+----------------------------------+  
| class ClassA                     |  
| {                                |  
|     void FuncA1();               |  
|     void FuncA2();               |  
| };                               |  
| class ClassB : public ClassA     |  
| {                                |  
|     void FuncB1();               |  
|     void FuncB2();               |  
| };                               |  
+----------------------------------+  


             Global table              namespace ClassA table                       Stick.cpp  
      +----------+-----------+        +----------+-----------+                     +-------------------------------------------------------------------+  
      |   Key    |   Value   |   +--->|   Key    |   Value   |                     |    +----------------------------------------------------------+   |  
      |----------+-----------|   |    |----------+-----------|   +--------------------->|  void __ClassA_New__(lua_State* L)                       |   |  
      :          :           :   |    :          :           :   |                 |    |  {                                                       |   |  
      |----------|-----------|   |    |----------|-----------|   |                 |    |      :                                                   |   |  
      | "ClassA" |   TABLE  -----+    |  "New"   | FUNCTION -----+                 |    |  }                                                       |   |  
      |----------|-----------|        |----------|-----------|                     |    +----------------------------------------------------------+   |  
      | "ClassB" |   TABLE  -----+    :          :           :                     |    +----------------------------------------------------------+   |  
      |----------|-----------|   |                               +--------------------->|  void __ClassB_New__(lua_State* L)                       |   |  
      :          :           :   |     namespace ClassB table    |                 |    |  {                                                       |   |  
                                 |    +----------+-----------+   |                 |    |      :                                                   |   |  
                                 +--->|   Key    |   Value   |   |                 |    |  }                                                       |   |  
                                      |----------+-----------|   |                 |    +----------------------------------------------------------+   |  
                                      :          :           :   |                 |    +----------------------------------------------------------+   |  
                                      |----------|-----------|   |  +------------------>|  void __ClassA_Destructor__(lua_State* L)                |   |  
                                      |  "New"   | FUNCTION -----+  |              |    |  {                                                       |   |  
                                      |----------|-----------|      |              |    |      :                                                   |   |  
                                      :          :           :      |              |    |  }                                                       |   |  
                                                                    |              |    +----------------------------------------------------------+   |  
             registry                    ClassA metatable           |              |    +----------------------------------------------------------+   |  
    +------------+-----------+        +----------+-----------+      |  +--------------->|  void __ClassA_FuncA1__(lua_State* L)                    |   |  
    |   Key      |   Value   |   +--->|   Key    |   Value   |      |  |           |    |  {                                                       |   |  
    |------------+-----------|   |    |----------+-----------|      |  |  +------------>|      auto w = (StickInstanceWrapper*)luaL_testudata(L);  |   |  
    :            :           :   |    |  "__gc"  | FUNCTION --------+  |  |        |    |      ((ClassA*)w)->FuncA1();                             |   |  
    |------------|-----------|   |    |----------|-----------|         |  |        |    |  }                                                       |   |  
    |"__ClassA__"|   TABLE  -----+    | "FuncA1" | FUNCTION -----------+  |        |    +----------------------------------------------------------+   |  
    |------------|-----------|        |----------|-----------|            |        |    +----------------------------------------------------------+   |  
    |"__ClassB__"|   TABLE  -----+    | "FuncA2" | FUNCTION --------------|------------>|  void __ClassA_FuncA2__(lua_State* L)                    |   |  
    |------------|-----------|   |    |----------|-----------|            |        |    |  {                                                       |   |  
    :            :           :   |    :          :           :            |  +--------->|      :                                                   |   |  
                                 |                                        |  |     |    |  }                                                       |   |  
                                 |       ClassB metatable                 |  |     |    +----------------------------------------------------------+   |  
                                 |    +----------+-----------+            |  |     |    +----------------------------------------------------------+   |  
                                 +--->|   Key    |   Value   |   +--------|--|--------->|  void __ClassB_Destructor__(lua_State* L)                |   |  
                                      |----------+-----------|   |        |  |     |    |  {                                                       |   |  
                                      |  "__gc"  | FUNCTION -----+        |  |     |    |      :                                                   |   |  
                                      |----------|-----------|            |  |     |    |  }                                                       |   |  
                                      | "FuncA1" | FUNCTION --------------+  |     |    +----------------------------------------------------------+   |  
                                      |----------|-----------|               |     |    +----------------------------------------------------------+   |  
                                      | "FuncA2" | FUNCTION -----------------+  +------>|  void __ClassB_FuncB1__(lua_State* L)                    |   |  
                                      |----------|-----------|                  |  |    |  {                                                       |   |  
                                      | "FuncB1" | FUNCTION --------------------+  |    |      :                                                   |   |  
                                      |----------|-----------|                     |    |  }                                                       |   |  
                                      | "FuncB2" | FUNCTION -----+                 |    +----------------------------------------------------------+   |  
                                      |----------|-----------|   |                 |    +----------------------------------------------------------+   |  
                                      :          :           :   +--------------------->|  void __ClassB_FuncB2__(lua_State* L)                    |   |  
                                                                                   |    |  {                                                       |   |  
                                                                                   |    |      :                                                   |   |  
                                                                                   |    |  }                                                       |   |  
                                                                                   |    +----------------------------------------------------------+   |  
                                                                                   +-------------------------------------------------------------------+
```  

#### Class instances  

```  
 Lua                                      C++  
+----------------------------------+     +----------------------------------+  
| a = ClassA.New();                |     | class ClassA                     |  
| b = ClassA.ClassB.New();         |     | {                                |  
+----------------------------------+     |     class ClassB                 |  
                                         |     {                            |  
                                         |         int FuncX(int a);        |  
                                         +----------------------------------+  


                                          Lua-userdata  
           Global table               StickInstanceWrapper*                                        C++ Application  
    +----------+-----------+        +----------------------+                              +-------------------------------+  
    |   Key    |   Value   |   +--->|    variable "ptr" -----------------------------+    |        ClassA instance        |  
    |----------+-----------|   |    |                      |                         |    |    +----------------------+   |  
    :          :           :   |    +----------------------+                         +-------->|                      |   |  
    |----------|-----------|   |               |               Lua-userdata               |    |                      |   |  
    |    "a"   | USERDATA -----+               |           StickInstanceWrapper*          |    +----------------------+   |  
    |----------|-----------|                   |         +----------------------+         |                               |  
    |    "b"   | USERDATA ---------------------|-------->|    variable "ptr" --------+    |        ClassB instance        |  
    |----------|-----------|                   |         |                      |    |    |    +----------------------+   |  
    :          :           :                   |         +----------------------+    +-------->|                      |   |  
                                               |                    |                     |    |                      |   |  
                                               |                    |                     |    +----------------------+   |  
                                           metatable            metatable                 +-------------------------------+  
                                               |                    |  
                                               |                    +---------+  
                                               |                              |  
                                               V                              V  
            registry                    ClassA metatable                ClassB metatable  
   +------------+-----------+        +----------+-----------+        +----------+-----------+  
   |   Key      |   Value   |   +--->|   Key    |   Value   |   +--->|   Key    |   Value   |  
   |------------+-----------|   |    |----------+-----------|   |    |----------+-----------|  
   :            :           :   |    |  "__gc"  | FUNCTION  |   |    |  "__gc"  | FUNCTION  |  
   |------------|-----------|   |    |----------|-----------|   |    |----------|-----------|  
   |"__ClassA__"|   TABLE  -----+    :          :           :   |    | "FuncX"  | FUNCTION  |  
   |------------|-----------|                                   |    |----------|-----------|  
   |"__ClassB__"|   TABLE  -------------------------------------+    :          :           :  
   |------------|-----------|  
   :            :           :  
```  

#### Struct instances  

```  
 Lua                                      C++  
+----------------------------------+     +----------------------------------+  
| a = GetA();                      |     | struct StructA                   |  
+----------------------------------+     | {                                |  
                                         |     int Var1;                    |  
                                         |     std::string Var2;            |  
                                         | }                                |  
                                         | extern StructA* GetA();          |  
                                         +----------------------------------+  


           Global table                StructA instance table  
    +----------+-----------+       +------------+----------------+  
    |   Key    |   Value   |   +-->|    Key     |   Value        |  
    |----------+-----------|   |   |------------+----------------|  
    :          :           :   |   |   "Var1"   | [number value] |  
    |----------|-----------|   |   |------------|----------------|  
    |    "a"   |   TABLE  -----+   |   "Var2"   | [string value] |  
    |----------|-----------|       +------------+----------------+  
    :          :           :  
```  

************************************************************  

## Appendix  

### How to debug  

<img src="https://github.com/hukushirom/luastick/blob/master/Examples_5_3/Sticktrace/image01.png">

1. Start StickTest.exe.

2. Click (1). 'Script Editor' will be displayed.

3. Click (2) to set a breakpoint.

4. Click (3) to change to the debug mode.

5. Click (4), and start Lua function 'Test1'.

6. Lua script will be paused at the breakpoint.

7. Input the variable name 'c1' at (5).

7. Click (6) then 'c1' will be added into the watch window.

8. Click (7) to expnd the array 'c1'.

### Built-in commands for LuaStick  

**STICK.ObjectToUserdata(object)**
- Convert the object to a lightuserdata-type(void * for C++).  

**STICK.IsNullObject(object)**
- Check the object is nullptr or not.  

### Built-in commands for Sticktrace  

**STICKTRACE.DebugBreak()**
- Pause the Lua program execution on the next line.  

**STICKTRACE.OutputDebugMessage(x)**
- Send x to the message box in the debugger.  


