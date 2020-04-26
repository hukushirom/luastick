# LuaStick

LuaStick is a C++ program which enables exporting C++ function to Lua and calling Lua program from C++. You can export C++ classes, structs, namespaces, enums, constants and functions. To export them, you just add 3 slashes comment `/// <stick export="true" />` into the C++ header.  
For example, you can export the class 'X' and member function 'Func' like the following:  
```C++
// main.h

/// <stick export="true" />
class X
{
    /// <stick export="true" />
    /// <param name="x" io="in" />
    void Func(int x)
    {
        std::cout << x * 2;
    }
};
```

To generate binding sources, run LuaStick.exe like the follwing command line:  
```
>LuaStick.exe -out Stick Sticklib.h main.h
```
LuaStick.exe generates C++ source, header and HTML files.
```
     +--------------------------------+          Manual
     |          Application           |      +------------+  Generate
     |                                |      | Stick.html |<---------+
     |   +----------+  +----------+   |      +------------+          |
     |   |Stickrun.h|  |Sticklib.h|---|---+                          |
     |   +----------+  +----------+   |   |       +--------------+   |
     |   +----------+  +----------+   |   +------>|              |---+
     |   | main.cpp |  | main.h   |---|---------->| LuaStick.exe |------+
     |   +----------+  +----------+   |           |              |---+  |
     |                                |           +--------------+   |  |
     |   +----------+  +----------+   |      Generate                |  |
     |   |liblua.lib|  | Stick.h  |<--|------------------------------+  |
     |   +----------+  +----------+   |                                 |
     |        :        +----------+   |      Generate                   |
     |                 |Stick.cpp |<--|---------------------------------+
     |                 +----------+   |
     +--------------------------------+
```

The following is a simple program to make the class 'X' instance and call its method 'Func'.
```C++
// main.cpp

#include "Stick.h"

int main()
{
    Stickrun stickrun(luastick_init);
    std::string lua_src =
      "obj=X.New();"
      "obj:Func(10);";
    stickrun.DoString(nullptr, lua_src, "myscript");
}
```
If you write `<summary>` and other tag text contents, LuaStick.exe generates an API manual HTML file.
```C++
// main.h

/// <stick export="true" />
/// <summary>
/// This is class X.
/// </summary>
class X
{
    /// <stick export="true" />
    /// <summary>
    /// This is function Func.
    /// </summary>
    /// <param name="x" io="in">Parameter X</param>
    /// <returns>It returns double x</returns>
    int Func(int x)
    {
        return x * 2;
    }
};
```
The manual describes summary of functions and Lua example codes.

For details, see [luastick/manual.md](https://github.com/hukushirom/luastick/blob/master/manual.md).  
You can download Windows executable from [this page](https://github.com/hukushirom/luastick/releases).
