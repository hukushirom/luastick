#include "stdafx.h"
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <filesystem>
#include "ReadBufferedFile.h"
#include "StringBuffer.h"
#include "LeException.h"
#include "UtilFile.h"
#include "UtilMisc.h"
#include "UtilXml.h"

// LuaStick.exe -out "c:\temp\test" stdafx.h global.h main.h

// <stick export="true" type="class" />
// <stick export="true" type="class" lname="anotherclass" />
// <stick export="true"/>
// <stick export="true" lname="anotherfunction" />
// <param name="v1" io="inout">The v1.</param>
// <stickdef type="c-lua" luatype="l_string" ctype="std::string" luatoc="Sticklib::lstring_to_astring" ctolua="Sticklib::astring_to_lstring" />
// <stickdef type="c-c" ctype1="Sticklib::wbuffer" ctype2="wchar_t*" c1toc2="Sticklib::wbuffer_to_wtext" />
// <stickdef type="c-c" ctype1="Sticklib::wbuffer" ctype2="wchar_t*" c1toc2="Sticklib::wbuffer_to_wtext" c2toc1="Sticklib::wtext_to_wbuffer" />
// <stickdef type="exception" cref="MyException*" message="WStrToAStr(e->GetMessage()).get()" delete="e->Delete()" />
// <stickdef type="exception" cref="YourException" message="WStrToAStr(e.GetMessage()).get()" />
// <exception cref="MyException*"></exception>
// <exception cref="YourException"></exception>

// name:it specifies another name used in lua.
// <stick export="true" type="class" name="HELLO" />



// Stack status when calling c++ function.
//
// 1.Start c++ function. e.g. int Abc(int v1, int v2)
//
//       stack
//    +---------+
//   2|   v2    |
//    |---------|
//   1|   v1    |
//    +---------+
//
//  lua_tointeger(L,1) -> v1
//  lua_tointeger(L,2) -> v2
//
// --------------------------------------
// 2.Set the result of c++ function to the stack.
//
//       stack
//    +---------+
//   3|  result | <-- Result of Abc(v1,v2)
//    |---------|
//   2|   v2    |
//    |---------|
//   1|   v1    |
//    +---------+
//
// --------------------------------------
// 3. Return the number of result to Lua.

//
// チャンクとは
// https://qiita.com/wingsys/items/0be3e4a627622cd5b7c3
// チャンクは文の連続であり、改行によって切り離される。
// (セミコロンを連打すると怒られるのはこの都合上空行を内包できないという事情もある)
// チャンクは内部的に無名関数である。このためチャンクはローカル変数を持つことも可能。
//


//
// メタテーブルとは？
//
// table Bがtable Aのメタテーブルの場合、table Aに対する操作をtable Bで規定できる。
// 例えば下図の場合、table Aがdisposeされる時に"__gc"に設定されたc-func1が実行される。
//
//                                  registry                  +-------------+
//                               +------------+--------+      |             |
//                               |     Key    | Value  |      |             V
//    +----------+-------+       |------------|--------|      |    +----------+-------+
//    |   Key    | Value |       :            :        :      |    |   Key    | Value |
//    |----------|-------|       |------------|--------|      |    |----------|-------|
//    |   "abc"  |  10   |       |superRegName| table  |------+    | "__name" |regName|
//    |----------|-------|       |------------|--------|           |----------|-------|
//    |   "efg"  | table |       :            :        :           |"__index" | table |
//    |----------|-------|                                         |----------|-------|
//    |   "stu"  |"hello"|                metatable                | "__gc"   |c-func1|
//    |----------|-------|========================================>|----------|-------|
//    |   "xyz"  | 3.14  |                                         |"GetValue"|c-func2|
//    +----------+-------+                                         +----------+-------+
//          table A                                                       table B
//
//
//
// userdataに対してもメタテーブルが設定できる。
// 例えば下図の場合、userdataがdisposeされる時に"__gc"に設定されたc-func1が実行される。
// userdataに対しての操作、例えばa=userdataの場合、a:FuncA()、a:FuncB()でc-func2、c-func3が実行される。
//
//                              metatable
//       stack                +------------+   +----------------------------------+
//    |---------|             |            |   |                 registry         |
//  -1|userdata |---+         |            V   V              +-------+-------+   |
//    |---------|   |         |    +----------+-------+       | Key   | Value |   |
//    :         :   |         |    |   Key    | Value |       |-------|-------|   |
//                  |         |    |----------|-------|       |regName| table |---+
//                  |         |    |  "__gc"  |c-func1|<--+   +-------+-------+
//                  |         |    +----------|-------+   |   :       :       :
//                  |         |    | "FuncA"  |c-func2|   |
//                  |         |    +----------|-------+   |
//                  |         |    | "FuncB"  |c-func3|   C++ Destructor
//                  |         |    +----------+-------+
//                  |         |
//                  |    +------------------+
//                  +--->| allocated memory |
//                       +------------------+
//
//
// Caution!!
// You can assign a metatable to lightuserdata too, but only one metatable can be assigned to multiple lightuserdata.
//
// e.g. Following is not allowed.
//
//               stack
//          |--------------|
//   +------|lightuserdata |------------------------------------------------+
//   |      |--------------|               Not allowed                      |
//   |  +---|lightuserdata |--------------------------+                     |
//   |  |   |--------------|                          |                     |
//   |  |   :              :                          |                     |
//   |  |                                             V                     V
//   |  |   +------------------------+        +----------+-------+  +----------+-------+
//   |  |   | Application            |        |   Key    | Value |  |   Key    | Value |
//   |  |   |  +------------------+  |        |----------|-------|  |----------|-------|
//   |  +----->| Class instance   |  |        |  "__gc"  |c-func1|  |  "__gc"  |c-func1|
//   |      |  +------------------+  |        +----------|-------+  +----------|-------+
//   |      |  +------------------+  |        | "FuncA"  |c-func2|  | "FuncA"  |c-func2|
//   +-------->| Class instance   |  |        +----------|-------+  +----------|-------+
//          |  +------------------+  |        | "FuncB"  |c-func3|  | "FuncB"  |c-func3|
//          +------------------------+        +----------+-------+  +----------+-------+
//
// So, LuaStick doesn't use lightuserdata for class instance. Instead of it, LuaStick wraps the class instance
// with a userdata and assigns metatable to the userdata.
// See the following example.
//
//            stack
//          |--------|
//          |userdata|---------------------------------------------+
//          |--------|                                             |            metatable
//          |userdata|---------------------------+          +------|----------------------------------------+
//          |--------|                           |          |      |                                        |
//          :        :                           |          |      |      metatable                         |
//                                               |          |      |  +---------------+                     |
//                                               |          |      |  |               |                     |
//                                               V          |      |  |               V                     V
//                                         +--------------------+  |  |       +----------+-------+  +----------+-------+
//          +------------------------+     |StickInstanceWrapper|  |  |       |   Key    | Value |  |   Key    | Value |
//          | Application            |  +--|  class instance    |  |  |       |----------|-------|  |----------|-------|
//          |  +------------------+  |  |  +--------------------+  |  |       |  "__gc"  |c-func1|  |  "__gc"  |c-func1|
//          |  | Class instance   |<----+                          V  |       +----------|-------+  +----------|-------+
//          |  +------------------+  |              +--------------------+    | "FuncA"  |c-func2|  | "FuncA"  |c-func2|
//          |  +------------------+  |              |StickInstanceWrapper|    +----------|-------+  +----------|-------+
//          |  | Class instance   |<----------------|  class instance    |    | "FuncB"  |c-func3|  | "FuncB"  |c-func3|
//          |  +------------------+  |              +--------------------+    +----------+-------+  +----------+-------+
//          +------------------------+
//

#define SRCMARKER (std::string("Generated at \"") + __FILE__ + "\"(" + std::to_string(__LINE__) + ")")

#define __FMTEXP(x) x
#define __FMTNAME(_00,_01,_02,_03,_04,_05,_06,_07,_08,_09,_10,_11,_12,_13,_14,_15,_16,_17,NM,...) NM
#define __FMT00(msn,msv,s) UtilString::Trim1CR(UtilString::Replace(s,msn,msv))
#define __FMT01(msn,msv,s,v01) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01))
#define __FMT02(msn,msv,s,v01,v02) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02))
#define __FMT03(msn,msv,s,v01,v02,v03) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03))
#define __FMT04(msn,msv,s,v01,v02,v03,v04) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04))
#define __FMT05(msn,msv,s,v01,v02,v03,v04,v05) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05))
#define __FMT06(msn,msv,s,v01,v02,v03,v04,v05,v06) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06))
#define __FMT07(msn,msv,s,v01,v02,v03,v04,v05,v06,v07) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07))
#define __FMT08(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08))
#define __FMT09(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09))
#define __FMT10(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10))
#define __FMT11(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11))
#define __FMT12(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12))
#define __FMT13(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13))
#define __FMT14(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14))
#define __FMT15(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14,v15) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14,"${"#v15"}",v15))
#define __FMT16(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14,v15,v16) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14,"${"#v15"}",v15,"${"#v16"}",v16))
#define __FMT17(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14,v15,v16,v17) UtilString::Trim1CR(UtilString::Replace(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14,"${"#v15"}",v15,"${"#v16"}",v16,"${"#v17"}",v17))
// Intermediate macro '__FMTEXP' is necessary for Visual C++.
#define FORMTEXT(...) __FMTEXP(__FMTNAME(__VA_ARGS__,__FMT17,__FMT16,__FMT15,__FMT14,__FMT13,__FMT12,__FMT11,__FMT10,__FMT09,__FMT08,__FMT07,__FMT06,__FMT05,__FMT04,__FMT03,__FMT02,__FMT01,__FMT00)("${SRCMARKER}",SRCMARKER,__VA_ARGS__))


#define __FMH00(msn,msv,s) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv))
#define __FMH01(msn,msv,s,v01) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01))
#define __FMH02(msn,msv,s,v01,v02) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02))
#define __FMH03(msn,msv,s,v01,v02,v03) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03))
#define __FMH04(msn,msv,s,v01,v02,v03,v04) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04))
#define __FMH05(msn,msv,s,v01,v02,v03,v04,v05) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05))
#define __FMH06(msn,msv,s,v01,v02,v03,v04,v05,v06) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06))
#define __FMH07(msn,msv,s,v01,v02,v03,v04,v05,v06,v07) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07))
#define __FMH08(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08))
#define __FMH09(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09))
#define __FMH10(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10))
#define __FMH11(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11))
#define __FMH12(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12))
#define __FMH13(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13))
#define __FMH14(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14))
#define __FMH15(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14,v15) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14,"${"#v15"}",v15))
#define __FMH16(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14,v15,v16) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14,"${"#v15"}",v15,"${"#v16"}",v16))
#define __FMH17(msn,msv,s,v01,v02,v03,v04,v05,v06,v07,v08,v09,v10,v11,v12,v13,v14,v15,v16,v17) UtilString::Trim1CR(UtilString::ReplaceAsHtml(s,msn,msv,"${"#v01"}",v01,"${"#v02"}",v02,"${"#v03"}",v03,"${"#v04"}",v04,"${"#v05"}",v05,"${"#v06"}",v06,"${"#v07"}",v07,"${"#v08"}",v08,"${"#v09"}",v09,"${"#v10"}",v10,"${"#v11"}",v11,"${"#v12"}",v12,"${"#v13"}",v13,"${"#v14"}",v14,"${"#v15"}",v15,"${"#v16"}",v16,"${"#v17"}",v17))
// Intermediate macro '__FMTEXP' is necessary for Visual C++.
#define FORMHTML(...) __FMTEXP(__FMTNAME(__VA_ARGS__,__FMH17,__FMH16,__FMH15,__FMH14,__FMH13,__FMH12,__FMH11,__FMH10,__FMH09,__FMH08,__FMH07,__FMH06,__FMH05,__FMH04,__FMH03,__FMH02,__FMH01,__FMH00)("${SRCMARKER}",SRCMARKER,__VA_ARGS__))

constexpr const wchar_t* OPTION_OUT = L"out";
constexpr const wchar_t* OPTION_LANG = L"lang";

/// <summary>
/// Make dummy class object. Use for function's argument.
/// </summary>
template<typename T>
class Dummy
{
public:
	operator T & ()
	{
		return dummy;
	}
private:
	T dummy;
};

/// <summary>
/// Class-type. It's used in ClassRec.
/// </summary>
enum class ClassRealType
{
	NONE,
	CLASS,		// class ~
	STRUCT,		// struct ~
	UNION,		// union ~
	NAMESPACE	// namespace ~
};

/// <summary>
/// Lua-type. It's defined with <sticktype~ tag.
/// e.g. "boolean","integer","number".
/// Not only Lua's native type, but also user defined type: "array<integer>", "hash<integer,string>" etc.
/// </summary>
struct LuaType
{
	static const LuaType NIL;

	LuaType() {}
	LuaType(const char * name) : m_name(name) {}
	LuaType(const std::string & name) : m_name(name) {}
	bool IsNull() const
	{
		return m_name.empty();
	}
	bool operator == (const LuaType & luaType) const
	{
		return (m_name == luaType.m_name);
	}
	bool operator != (const LuaType & luaType) const
	{
		return (m_name != luaType.m_name);
	}
	bool operator < (const LuaType & luaType) const
	{
		return m_name < luaType.m_name;
	}
	size_t HashValue() const
	{
		return std::hash<std::string>()(m_name);
	}
	const std::string & ToString() const
	{
		return m_name;
	}

	int CompareHead(const LuaType & luaType) const
	{
		return ::strncmp(m_name.c_str(), luaType.m_name.c_str(), luaType.m_name.length());
	}

	std::string m_name;
};

namespace std
{
	template<>
	class hash<LuaType>
	{
	public:
		size_t operator () (const LuaType &luaType) const
		{
			return luaType.HashValue();
		}
	};
}

const LuaType LuaType::NIL;


/// <summary>
/// Record for Lua-type. Application stores LuaType->LuaTypeRec hash.
/// </summary>
struct LuaTypeRec
{
	LuaTypeRec() {}
	LuaTypeRec(const LuaTypeRec & rec)
		: m_ctype(rec.m_ctype)
		, m_getFunc(rec.m_getFunc)
		, m_setFunc(rec.m_setFunc)
	{}
	LuaTypeRec(const char * ctype, const char * getFunc, const char * setFunc)
		: m_ctype(ctype)
		, m_getFunc(getFunc)
		, m_setFunc(setFunc)
	{}
	LuaTypeRec(const std::string & ctype, const std::string & getFunc, const std::string & setFunc)
		: m_ctype(ctype)
		, m_getFunc(getFunc)
		, m_setFunc(setFunc)
	{}
	LuaTypeRec & operator = (const LuaTypeRec & rec)
	{
		m_ctype = rec.m_ctype;
		m_getFunc = rec.m_getFunc;
		m_setFunc = rec.m_setFunc;
		return *this;
	}

	std::string m_ctype;	// C++ type. e.g. If LuaType="number", m_ctype="double"
	std::string m_getFunc;	// Function's name to get value from Lua's stack.
	std::string m_setFunc;	// Function's name to push value on Lua's stack.
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Gets LuaType object from C++-type string.
/// </summary>
/// <param name="ctype">C++-type string</param>
/// <returns>LuaType</returns>
static LuaType CtypeToLuaType(const std::string & ctype);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Language. e.g. "ja","en","zh"...
/// </summary>
static std::string LANG;

/// <summary>
/// Lua-type -> information hash array.
/// Information includes c++ type, c++ function name which get value from Lua stack and c++ function name which push value on Lua stack.
/// e.g. "string" -> { "std::string", "Sticklib::check_lvalue", "Sticklib::push_lvalue" }
/// </summary>
static std::unordered_map<LuaType, LuaTypeRec> LTYPE_TO_REC;

/// <summary>
/// Hash from c++ type to Lua type. c++ types registered as key, are converted to Lua direct.
/// This table is set by "<sticktype>" tag.
/// e.g. {"std::string", "__int64", "double", ...}
/// </summary>
static std::unordered_map<std::string, LuaType> CTYPE_TO_LUATYPE;

/// <summary>
/// Set of c++ type to type converter. Direction is forward.
/// { type1, type2 } -> type_1_to_2_converter
/// e.g.
///
///  +-----------+   astring_to_wstring    +------------+   wstring_to_wtext    +--------+
///  |std::string| ----------------------> |std::wstring| --------------------> |wchar_t*|
///  +-----------+                         +------------+                       +--------+
///
/// {"std::string", "std::wstring"}->"astring_to_wstring",
/// {"std::wstring", "wchar_t*"}->"wstring_to_wtext"
/// </summary>
static std::map<std::pair<std::string, std::string>, std::string> CTYPE_1TO2_CONVERTER;

/// <summary>
/// Set of c++ type to type converter. Direction is backward.
/// { type1, type2 } -> type_2_to_1_converter
/// e.g.
///
///  +--------+    wstring_to_wtext   +------------+    astring_to_wstring   +-----------+
///  |wchar_t*| <-------------------- |std::wstring| <---------------------- |std::string|
///  +--------+                       +------------+                         +-----------+
///
/// {"wchar_t*", "std::wstring" }->"wstring_to_wtext"
/// {"std::wstring", "std::string" }->"astring_to_wstring",
/// </summary>
static std::map<std::pair<std::string, std::string>, std::string> CTYPE_2TO1_CONVERTER;

/// <summary>
/// Exception name -> {Getting error message method, Delete method}
/// e.g. <stickdef type="exception" cref="MyException*" message="WStrToAStr(e->GetMessage()).get()" delete="e->Delete()" />
/// </summary>
static std::unordered_map<std::string, std::tuple<std::string, std::string>> EXCEPTION_TO_MESSAGE_DELETE;

/// <summary>
/// Stream to store the contents of header file.
/// </summary>
static StringBuffer OUTPUT_H_FILE_STREAM;

/// <summary>
/// Stream to store 'luastick_init' function's description.
/// </summary>
static StringBuffer OUTPUT_INITFUNC_STREAM;

/// <summary>
/// Stream to store the description of the exported functions.
/// </summary>
static StringBuffer OUTPUT_EXPORTFUNC_STREAM;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----- 20.02.20  変更前 ()-----
//// [extern inline TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^extern\s+inline\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_EXT_INL_XML(R"(^extern\s+inline\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//
//// [extern TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^extern\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_EXT_XML(R"(^extern\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//
//// [static inline TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^static\s+inline\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_STC_INL_XML(R"(^static\s+inline\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//
//// [static TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^static\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_STC_XML(R"(^static\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//
//// [virtual TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^virtual\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_VIR_XML(R"(^virtual\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//
//// [inline TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^inline\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_INL_XML(R"(^inline\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//
//// [TT:TT *& FFF (PPP)]のTTT:TT *&,FFF,PPP)を抽出 "^([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
//static const std::regex FUNC_REGULAR_XML(R"(^([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//----- 20.02.20  変更後 ()-----
// [extern inline TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^extern\s+inline\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_EXT_INL_XML(R"(^extern\s+inline\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");

// [extern TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^extern\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_EXT_XML(R"(^extern\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");

// [static inline TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^static\s+inline\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_STC_INL_XML(R"(^static\s+inline\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");

// [static TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^static\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_STC_XML(R"(^static\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");

// [virtual TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^virtual\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_VIR_XML(R"(^virtual\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");

// [inline TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^inline\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_INL_XML(R"(^inline\s+([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");

// [TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出 "^([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\(([^\)]+)\)"
static const std::regex FUNC_REGULAR_XML(R"(^([a-zA-Z_][\w:<,>\*\& ]*)\s+([a-zA-Z_]\w*)\s*\((.+)$)");
//----- 20.02.20  変更終 ()-----

//----- 19.11.20 Fukushiro M. 変更前 ()-----
//// Extract "std::wstring *&" and "argbBack" from "std::wstring *&argbBack"
//static const std::regex ARG_XML(R"(^\s*([a-zA-Z_][\w:\*\& ]*)\b([a-zA-Z_]\w*)\s*$)");
//----- 19.11.20 Fukushiro M. 変更後 ()-----
// Extract "std::wstring *&" and "argbBack" from "std::wstring *&argbBack"
// e.g. "std::vector<int> & arg" -> "std::vector<int> & " , "arg"
// e.g. "std::map<int, string> & arg" -> "std::map<int, string> & " , "arg"
static const std::regex ARG_XML(R"(^\s*([a-zA-Z_][\w:<>,\*\&\s]*)\b([a-zA-Z_]\w*)\s*$)");
//----- 19.11.20 Fukushiro M. 変更終 ()-----

// [const]
static const std::regex CONST_DEF(R"(\bconst\b)");

// change two or more spaces to single space.
static const std::regex MULTISPACE_DEF(R"(\s+)");

// Extracts 'CCC','DDD' from "class CCC : public DDD"
static const std::regex CLASS_PUBLIC_XML(R"(^class\s+([a-zA-Z_]\w*)\s*:\s*public\s+(:*[a-zA-Z_][\w:]*)\b)");

// Extracts 'CCC','DDD' from "struct CCC : public DDD"
static const std::regex STRUCT_PUBLIC_XML(R"(^struct\s+([a-zA-Z_]\w*)\s*:\s*public\s+(:*[a-zA-Z_][\w:]*)\b)");

// Extracts 'CCC' from "class CCC ~"
static const std::regex CLASS_XML(R"(^class\s+([a-zA-Z_]\w*)\b)");

// Extracts 'CCC' from "struct CCC ~"
static const std::regex STRUCT_XML(R"(^struct\s+([a-zA-Z_]\w*)\b)");

// Extracts 'CCC' from "union CCC ~"
static const std::regex UNION_XML(R"(^union\s+([a-zA-Z_]\w*)\b)");

// Extracts 'CCC' from "namespace CCC ~"
static const std::regex NAMESPACE_XML(R"(^namespace\s+([a-zA-Z_]\w*)\b)");

// Extracts 'CCC' from "enum CCC "
static const std::regex ENUM_NAME_XML(R"(^enum\s+([a-zA-Z_]\w*)\s*$)");

// Extracts 'CCC' and 'DDD' from "enum CCC : DDD "
static const std::regex ENUM_NAME_TYPE_XML(R"(^enum\s+([a-zA-Z_]\w*)\s*:\s*([a-zA-Z_][\w\s]*)\b\s*$)");

// Check "enum {"
static const std::regex ENUM_XML(R"(^enum\s*$)");

// Extracts 'DDD' from "enum : DDD {"
static const std::regex ENUM_TYPE_XML(R"(^enum\s*:\s*([a-zA-Z_][\w\s]*)\b\s*$)");

// Extracts 'CCC' from "enum class CCC "
static const std::regex ENUM_CLASS_NAME_XML(R"(^enum\s+class\s+([a-zA-Z_]\w*)\b\s*$)");

// Extracts 'CCC' and 'DDD' from "enum class CCC : DDD "
static const std::regex ENUM_CLASS_NAME_TYPE_XML(R"(^enum\s+class\s+([a-zA-Z_]\w*)\s*:\s*([a-zA-Z_][\w\s]*)\b\s*$)");

// Extracts 'CCC' from "enum struct CCC "
static const std::regex ENUM_STRUCT_NAME_XML(R"(^enum\s+struct\s+([a-zA-Z_]\w*)\b\s*$)");

// Extracts 'CCC' and 'DDD' from "enum struct CCC : DDD "
static const std::regex ENUM_STRUCT_NAME_TYPE_XML(R"(^enum\s+struct\s+([a-zA-Z_]\w*)\s*:\s*([a-zA-Z_][\w\s]*)\b\s*$)");


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ClassRec structure
//
// e.g.
// +--------------------------------------------+
// | void FuncW(p,q);                           |
// | ClassA                                     |
// | {                                          |
// |     ClassB                                 |
// |     {                                      |
// |         enum class EnumX { A=10, B=20 }    |
// |         static const int ConstX = 30;      |
// |         void FuncX()                       |
// |         void FuncX(p,q,r)                  |
// |         static void FuncY()                |
// |         static void FuncY(p,q,r)           |
// |               :                            |
// +--------------------------------------------+
//
//
// ClassRec::CLASSREC_ARRAY --+--[0] -> ClassRec_0 --+-- id -> 0
//                            |                      |
//                            |                      +-- parentId -> -1
//                            |                      |
//                            |                      +-- classType -> Type::GLOBAL
//                            |                      |
//                            |                      +-- className -> ""
//                            |                      |
//                            |                      +-- classLuaname -> ""
//                            |                      |
//                            |                      +-- memberClassIdArray --+-[0] -> classId_1
//                            |                      |                        :
//                            |                      |
//                            |                      +-- staticFuncLuanameToFuncGroupId --+--["FuncW"] -> funcGroupId_1
//                            |                      :                                    :
//                            |
//                            |
//                            +--[classId_1] -> ClassRec_1 --+-- id -> classId_1
//                            |                              |
//                            |                              +-- parentId -> 0
//                            |                              |
//                            |                              +-- classType -> Type::CLASS
//                            |                              |
//                            |                              +-- className -> "ClassA"
//                            |                              |
//                            |                              +-- classLuaname -> "ClassA"
//                            |                              |
//                            |                              +-- memberClassIdArray --+-[0] -> classId_2
//                            |                              :                        :
//                            |
//                            +--[classId_2] -> ClassRec_2 --+-- id -> classId_2
//                            :                              |
//                                                           +-- parentId -> classId_1
//                                                           |
//                                                           +-- classType -> Type::CLASS
//                                                           |
//                                                           +-- className -> "ClassB"
//                                                           |
//                                                           +-- classLuaname -> "ClassB"
//                                                           |
//                                                           +-- memberClassIdArray --+--[0] -> classId_1
//                                                           |                        :
//                                                           |
//                                                           +-- methodFuncLuanameToFuncGroupId --+--["FuncX"] -> funcGroupId_2
//                                                           |                                    :
//                                                           |
//                                                           +-- staticFuncLuanameToFuncGroupId --+--["FuncY"] -> funcGroupId_3
//                                                           |                                    :
//                                                           |
//                                                           +-- constantLuanameToConstantId --+--["ConstX"] -> constantId_1
//                                                           |                                 :
//                                                           |
//                                                           +-- variableLuanameToVariableId --+--["VarX"] -> variableId_1
//                                                           |                                 :
//                                                           |
//                                                           +-- luanameToRegularEnumId
//                                                           |
//                                                           +-- luanameToClassEnumId --+--["EnumX"] -> enumId_1
//                                                           :                          :
//
//
// EnumRec::ENUMREC_ARRAY --+--[enumId_1] -> EnumRec_1 --+-- id -> enumId_1
//                          :                            |
//                                                       +-- parentId -> classId_2
//                                                       |
//                                                       +-- enumType -> ENUM_CLASS
//                                                       |
//                                                       +-- enumCname -> "EnumX"
//                                                       |
//                                                       +-- enumLuaname -> "EnumX"
//                                                       |
//                                                       +-- luanameToCname -> {"A"->"A","B"->"B"}
//
//
// ConstantRec::CONSTANTREC_ARRAY --+--[constantId_1] -> ConstantRec_1 --+-- id -> constantId_1
//                                  :                                    |
//                                                                       +-- parentId -> classId_2
//                                                                       |
//                                                                       +-- recType -> CONSTANT
//                                                                       |
//                                                                       +-- constantCname -> "ConstX"
//                                                                       |
//                                                                       +-- constantLuaname -> "ConstX"
//
//
// VariableRec::VARIABLEREC_ARRAY --+--[variableId_1] -> VariableRec_1 --+-- id -> variableId_1
//                                  :                                    |
//                                                                       +-- parentId -> classId_2
//                                                                       |
//                                                                       +-- variableCname -> "m_varX"
//                                                                       |
//                                                                       +-- variableLuaname -> "VarX"
//
//
// FuncGroupRec::FUNCGROUPREC_ARRAY --+--[funcGroupId_1] -> FuncGroupRec_1 --+-- id -> funcGroupId_1
//                                    |                                      |
//                                    |                                      +-- parentId -> 0
//                                    |                                      |
//                                    |                                      +-- luaname -> "FuncW"
//                                    |                                      |
//                                    |                                      +-- argCountToFuncId--+--[2] -> funcId_1
//                                    |
//                                    +--[funcGroupId_2] -> FuncGroupRec_2 --+-- id -> funcGroupId_2
//                                    |                                      |
//                                    |                                      +-- parentId -> classId_2
//                                    |                                      |
//                                    |                                      +-- luaname -> "FuncX"
//                                    |                                      |
//                                    |                                      +-- argCountToFuncId--+--[1] -> funcId_2
//                                    |                                                            |
//                                    |                                                            +--[4] -> funcId_3
//                                    |
//                                    +--[funcGroupId_3] -> FuncGroupRec_3 --+-- id -> funcGroupId_3
//                                                                           |
//                                                                           +-- parentId -> classId_2
//                                                                           |
//                                                                           +-- luaname -> "FuncY"
//                                                                           |
//                                                                           +-- argCountToFuncId--+--[0] -> funcId_4
//                                                                                                 |
//                                                                                                 +--[3] -> funcId_5
//
//
// FuncRec::FUNCREC_ARRAY --+--[funcId_1] -> FuncRec_1 --+-- id -> funcId_1
//                          |                            |
//                          |                            +-- parentId -> funcGroupId_1
//                          |                            |
//                          |                            +-- type -> STATIC
//                          |                            |
//                          |                            +-- funcCname -> "FuncW"
//                          |                            |
//                          |                            +-- funcLuaname -> "FuncW"
//                          |                            :
//                          |
//                          +--[funcId_2] -> FuncRec_2 --+-- id -> funcId_2
//                          |                            |
//                          |                            +-- parentId -> funcGroupId_2
//                          |                            |
//                          |                            +-- type -> METHOD
//                          |                            |
//                          |                            +-- funcCname -> "FuncX"
//                          |                            |
//                          |                            +-- funcLuaname -> "FuncX"
//                          |                            :
//                          |
//                          +--[funcId_3] -> FuncRec_3 --+-- id -> funcId_3
//                          |                            |
//                          |                            +-- parentId -> funcGroupId_2
//                          |                            |
//                          |                            +-- type -> METHOD
//                          |                            |
//                          |                            +-- funcCname -> "FuncX"
//                          |                            |
//                          |                            +-- funcLuaname -> "FuncX"
//                          |                            :
//                          |
//                          +--[funcId_4] -> FuncRec_4 --+-- id -> funcId_4
//                          |                            |
//                          |                            +-- parentId -> funcGroupId_3
//                          |                            |
//                          |                            +-- type -> STATIC
//                          |                            |
//                          |                            +-- funcCname -> "FuncY"
//                          |                            |
//                          |                            +-- funcLuaname -> "FuncY"
//                          |                            :
//                          |
//                          +--[funcId_5] -> FuncRec_5 --+-- id -> funcId_5
//                                                       |
//                                                       +-- parentId -> funcGroupId_3
//                                                       |
//                                                       +-- type -> STATIC
//                                                       |
//                                                       +-- funcCname -> "FuncY"
//                                                       |
//                                                       +-- funcLuaname -> "FuncY"
//                                                       :
//

struct EnumRec
{
	enum class Type
	{
		NONE,
		ENUM_CLASS,		// enum class Name { ~~
		REGULAR			// enum { ~~  / enum Name { ~~
	};

	EnumRec() : id(-1), parentId(-1), enumType(EnumRec::Type::NONE) {}
	EnumRec(int i, int p) : id(i), parentId(p), enumType(EnumRec::Type::NONE) {}
	int id;
	int parentId;
	EnumRec::Type enumType;
	std::string enumCname;		// primary enum name.
	std::string enumLuaname;	// luaname of enum name. if luaname is not specified, primary is set.
	std::string summary;		// summary of this enum.
	std::unordered_map<std::string, std::string> luanameToCname;	// Lua name -> C++ name
	std::unordered_map<std::string, std::string> luanameToSummary;	// Lua name -> Summary

	std::string GetFullpathElementCprefix() const;
	std::string GetFullpathElementLuaprefix() const;
	std::string GetFullpathCname() const;
	std::string GetFullpathLuaname() const;

	static std::vector<std::unique_ptr<EnumRec>> ENUMREC_ARRAY;
	static EnumRec & Get(int id)
	{
		return *ENUMREC_ARRAY[id].get();
	}
	static EnumRec & New(int parentId)
	{
		ENUMREC_ARRAY.emplace_back(std::make_unique<EnumRec>((int)ENUMREC_ARRAY.size(), parentId));
		return *ENUMREC_ARRAY.back().get();
	}
};
std::vector<std::unique_ptr<EnumRec>> EnumRec::ENUMREC_ARRAY;

struct ConstantRec
{
	enum class Type
	{
		NONE,
		CONSTANT,		// static const ~
		CONSTEXPR,		// constexpr ~
		DEFINE			// #define ~
	};

	ConstantRec() : id(-1), parentId(-1), recType(ConstantRec::Type::NONE) {}
	ConstantRec(int i, int p) : id(i), parentId(p), recType(ConstantRec::Type::NONE) {}
	int id;
	int parentId;
	ConstantRec::Type recType;
	std::string constantRawCtype;	// constant raw Type. e.g. "const int A=..." -> "int", "constexpr const char* A=..." -> "const char*"
	std::string constantCname;		// primary constant name.
	std::string constantLuaname;	// luaname of constant name. if luaname is not specified, primary constant name is set.
	std::string summary;
	std::vector<std::string> cToLuaConversionPath;	// e.g. "const int A=..." ->  {"int", "i32_to_i64", "__int64"}

	std::string GetFullpathCname() const;
	std::string GetFullpathLuaname() const;

	LuaType GetLuaType() const
	{
		return CtypeToLuaType(cToLuaConversionPath.back());
	}

	static std::vector<std::unique_ptr<ConstantRec>> CONSTANTREC_ARRAY;
	static ConstantRec & Get(int id)
	{
		return *CONSTANTREC_ARRAY[id].get();
	}
	static ConstantRec & New(int parentId)
	{
		CONSTANTREC_ARRAY.emplace_back(std::make_unique<ConstantRec>((int)CONSTANTREC_ARRAY.size(), parentId));
		return *CONSTANTREC_ARRAY.back().get();
	}
};

std::vector<std::unique_ptr<ConstantRec>> ConstantRec::CONSTANTREC_ARRAY;

struct VariableRec
{
	VariableRec() : id(-1), parentId(-1) {}
	VariableRec(int i, int p) : id(i), parentId(p) {}
	int id;
	int parentId;
	std::string variableRawCtype;	// variable raw Type. e.g. "int A" -> "int", "const char* A" -> "const char*"
	std::string variableCname;		// primary variable name.
	std::string variableLuaname;	// luaname of variable name. if luaname is not specified, primary variable name is set.
	std::string summary;
	std::vector<std::string> cToLuaConversionPath;	// e.g. "int A" ->  {"int", "i32_to_i64", "__int64"}
	std::vector<std::string> luaToCConversionPath;	// e.g. "int A" ->  {"__int64", "i64_to_i32", "int"}

	std::string GetFullpathCname() const;
	std::string GetFullpathLuaname() const;

	LuaType GetLuaType() const
	{
		return CtypeToLuaType(UtilString::Replace(cToLuaConversionPath.back(), "&", ""));
	}

	static std::vector<std::unique_ptr<VariableRec>> VARIABLEREC_ARRAY;
	static VariableRec & Get(int id)
	{
		return *VARIABLEREC_ARRAY[id].get();
	}
	static VariableRec & New(int parentId)
	{
		VARIABLEREC_ARRAY.emplace_back(std::make_unique<VariableRec>((int)VARIABLEREC_ARRAY.size(), parentId));
		return *VARIABLEREC_ARRAY.back().get();
	}
};

std::vector<std::unique_ptr<VariableRec>> VariableRec::VARIABLEREC_ARRAY;


/// <summary>
/// If there are two or more functions that have same Lua name, they are grouped under a FuncGroupRec.
/// </summary>
struct FuncGroupRec
{
	FuncGroupRec() : id(-1), parentId(-1) {}
	FuncGroupRec(int i, int p) : id(i), parentId(p) {}
	int id;
	int parentId;
	std::string luaname;		// luaname of function name. if luaname is not specified, primary is set.
	std::map<int, int> argCountToFuncId;	// Argument count -> Function. Polymorphic functions are distinguished with the count of arguments.

	std::string GetWrapperFunctionName() const;

	std::string GetFullpathLuaname() const;

	static std::vector<std::unique_ptr<FuncGroupRec>> FUNCGROUPREC_ARRAY;

	static FuncGroupRec & Get(int id)
	{
		return *FUNCGROUPREC_ARRAY[id].get();
	}
	static FuncGroupRec & New(int parentId)
	{
		FUNCGROUPREC_ARRAY.emplace_back(std::make_unique<FuncGroupRec>((int)FUNCGROUPREC_ARRAY.size(), parentId));
		return *FUNCGROUPREC_ARRAY.back().get();
	}
};

std::vector<std::unique_ptr<FuncGroupRec>> FuncGroupRec::FUNCGROUPREC_ARRAY;


struct FuncRec
{
	enum class Type
	{
		NONE,
		STATIC,			// Static function. e.g. static ~
		METHOD,			// Class method. e.g. virtual ~, int ~
		CONSTRUCTOR		// Class constructor.
	};

	FuncRec() : id(-1), parentId(-1) {}
	FuncRec(int i, int p) : id(i), parentId(p) {}
	int id;
	int parentId;
	Type type;
	std::string funcCname;
	std::string funcLuaname;	// luaname of function name. if luaname is not specified, primary is set.
	std::string summary;

	// Argument list. e.g. int FuncA(const char* a, double b) -> argNames={"a", "b"}. It does not include '__lstickvar_ret' : return value.
	std::vector<std::string> argNames;

	// Argument name -> c++ Type. The c++ Type is regularized.
	// e.g. const char * FuncA(const char * & a, const X *  b) ->
	// argNameToCtype={ "a" -> "char*&", "b" -> "::A::B::X*", "__lstickvar_ret" -> "char*" }. It includes '__lstickvar_ret' : return value.
	// If C++-type is specified with attribute 'ctype=', argNameToCtype is overwrite with the specified type.
	std::unordered_map<std::string, std::string> argNameToCtype;

	// Argument name -> c++ Type. The c++ Type is not regularized.
	// e.g. const char * FuncA(const char * & a, const X *  b) ->
	// argNameToRawCtype={ "a" -> "const char*&", "b" -> "const ::A::B::X*", "__lstickvar_ret" -> "const char*" }. It includes '__lstickvar_ret' : return value.
	// It's used for HTML help and definition of function's return variable.
	// e.g. const char* __lstickvar_ret = FuncX();
	//      ~~~~~~~~~~~
	std::unordered_map<std::string, std::string> argNameToRawCtype;

//----- 21.05.19 Fukushiro M. 追加始 ()-----
	// Argument name set which has autodel option.
	// e.g. <param name="hello" autodel="true"...>  <param name="world" autodel="true"...>  <returns autodel="true">
	// autoDelargNames={ "hello", "world", "__lstickvar_ret" }
	std::unordered_set<std::string> autoDelArgNames;
//----- 21.05.19 Fukushiro M. 追加終 ()-----

	// Argument name -> summary.
	// return value's summary is registered as "__lstickvar_ret".
	// It's used for HTML help and definition of function's return variable.
	std::unordered_map<std::string, std::string> argNameToSummary;
// 21.05.19 Fukushiro M. 1行削除 ()
//	std::vector<std::pair<std::string, std::string>> argNameToSummary;

	// Input Argument names.
	// e.g. int FuncA(const char* a, double b) -> inArgNames={"a", "b"}
	std::unordered_set<std::string> inArgNames;

	// Output Argument names.
	// e.g. int FuncA(const char* a, double & b) -> inArgNames={"b", "__lstickvar_ret"}
	// It includes '__lstickvar_ret' : return value.
	std::unordered_set<std::string> outArgNames;

	// Input argument name -> Lua to C++ converter path.
	// e.g. arg="int a" then  {"a" -> {"__int64", "i64_to_i32", "int"}}
	std::unordered_map<std::string, std::vector<std::string>> inArgNameToLuaToCppConversionPath;

	// Output argument name -> C++ to Lua converter path.
	// e.g. arg="int a" then  {"a" -> {"int", "i32_to_i64", "__int64"}}
	std::unordered_map<std::string, std::vector<std::string>> outArgNameToCppToLuaConversionPath;

	/// <summary>
	/// The exceptions that this function returns.
	/// </summary>
	std::vector<std::string> exceptions;

	LuaType ArgNameToLuaType(const std::string & argName) const
	{
		auto i = inArgNameToLuaToCppConversionPath.find(argName);
		if (i != inArgNameToLuaToCppConversionPath.end())
		{
			auto cType = i->second.front();
			UtilString::TrimRight(cType, '&');
			return CtypeToLuaType(cType);
		}
		i = outArgNameToCppToLuaConversionPath.find(argName);
		if (i != outArgNameToCppToLuaConversionPath.end())
		{
			auto cType = i->second.back();
			UtilString::TrimRight(cType, '&');
			return CtypeToLuaType(cType);
		}
		return LuaType::NIL;
	}
	std::string GetWrapperFunctionName() const;
	std::string GetFullpathLuaname() const;
	std::string GetFullpathLuanameForCall() const;
	std::string GetFullpathCname() const;

//----- 21.05.24 Fukushiro M. 追加始 ()-----
	int GetParentClassId() const
	{
		return FuncGroupRec::Get(parentId).parentId;
	}
//----- 21.05.24 Fukushiro M. 追加終 ()-----

	static std::vector<std::unique_ptr<FuncRec>> FUNCREC_ARRAY;

	static FuncRec & Get(int id)
	{
		return *FUNCREC_ARRAY[id].get();
	}

	static FuncRec & New(int parentId)
	{
		FUNCREC_ARRAY.emplace_back(std::make_unique<FuncRec>((int)FUNCREC_ARRAY.size(), parentId));
		return *FUNCREC_ARRAY.back().get();
	}
}; // struct FuncRec.

std::vector<std::unique_ptr<FuncRec>> FuncRec::FUNCREC_ARRAY;

struct ClassRec
{
	enum class Type
	{
		NONE,
		GLOBAL,
		CLASS,
		INCONSTRUCTIBLE,	// Created by factory method only. No constructor.
		STRUCT,
		STATICCLASS,
		NAMESPACE
	};

	ClassRec() : id(-1), parentId(-1), classType(ClassRec::Type::NONE) {}
	ClassRec(int i, int p) : id(i), parentId(p), classType(ClassRec::Type::NONE) {}

	int id;
	int parentId;
	ClassRec::Type classType;
	std::string classCname;		// primary class name.
	std::string classLuaname;	// luaname of class name. if luaname is not specified, primary is set.
	std::string summary;
	int superClassId;
	std::map<std::string, int> methodFuncLuanameToFuncGroupId;
	std::map<std::string, int> staticFuncLuanameToFuncGroupId;

	// Class registration order. The superclass must be registered before the subclass registration.
	std::vector<int> memberClassIdArray;
	std::unordered_set<std::string> memberClassLuanameSet;

	std::map<std::string, int> constantLuanameToConstantId;

	std::map<std::string, int> variableLuanameToVariableId;

	std::map<std::string, int> luanameToRegularEnumId;
	std::map<std::string, int> luanameToClassEnumId;

	std::string GetFullpathLuaname() const
	{
		if (parentId == -1)
			return std::string();
		const auto parentFullpathLuaname = ClassRec::Get(parentId).GetFullpathLuaname();
		return parentFullpathLuaname.empty() ? classLuaname : parentFullpathLuaname + "." + classLuaname;
	}

	std::string GetUniqueClassName() const
	{
		if (parentId == -1)
			return "lm__";
		else
			return ClassRec::Get(parentId).GetUniqueClassName() + classCname + "__";
	}

	std::string GetFullpathCname() const
	{
		if (parentId == -1)
			return "";
		else
			return ClassRec::Get(parentId).GetFullpathCname() + "::" + classCname;
	}

	std::string GetWrapperDestructorName() const
	{
		return GetUniqueClassName() + "Destructor";
	}

	int FindClass(const std::string & className) const
	{
//----- 19.12.10 Fukushiro M. 変更前 ()-----
//		std::vector<std::string> dummy;
//		return FindClass(UtilString::Split(dummy, className, "::"));
//----- 19.12.10 Fukushiro M. 変更後 ()-----
		return FindClass(UtilString::Split(Dummy<std::vector<std::string>>(), className, "::"));
//----- 19.12.10 Fukushiro M. 変更終 ()-----
	}

	int FindClass(const std::vector<std::string> & classNameArray) const
	{
		// empty name means this class.
		if (classNameArray.empty()) return id;
		if (classNameArray[0].empty())
		{	//----- enumNameArray[0]="" means classCname starts with "::" -----
			return ClassRec::Get(0).FindLowerClass(classNameArray, 0);
		}
		else
		{
			int classId = -1;
			const ClassRec * classRec = this;
			for (
				classId = classRec->FindLowerClass(classNameArray, -1);
				classId == -1 && classRec->parentId != -1;
				classId = classRec->FindLowerClass(classNameArray, -1)
				)
				classRec = &ClassRec::Get(classRec->parentId);
			return classId;
		}
	}

	int FindLowerClass(const std::vector<std::string> & classNameArray, int pos) const
	{
		if (pos != -1 && classCname != classNameArray[pos]) return -1;
		if (classNameArray.size() == pos + 1) return id;
		int targetId = -1;
		for (auto i = memberClassIdArray.begin(); targetId == -1 && i != memberClassIdArray.end(); i++)
			targetId = ClassRec::Get(*i).FindLowerClass(classNameArray, pos + 1);
		return targetId;
	}

	int FindEnum(const std::string & enumName) const
	{
		std::vector<std::string> enumNameArray;
		UtilString::Split(enumNameArray, enumName, "::");
		// empty name is error.
		if (enumNameArray.empty()) return -1;
		if (enumNameArray[0].empty())
		{	//----- enumNameArray[0]="" means classCname starts with "::" -----
			return ClassRec::Get(0).FindLowerEnum(enumNameArray, 0);
		}
		else
		{
			int enumId = -1;
			const ClassRec * classRec = this;
			for (
				enumId = classRec->FindLowerEnum(enumNameArray, -1);
				enumId == -1 && classRec->parentId != -1;
				enumId = classRec->FindLowerEnum(enumNameArray, -1)
				)
				classRec = &ClassRec::Get(classRec->parentId);
			return enumId;
		}
	}

	int FindLowerEnum(const std::vector<std::string> & enumNameArray, int pos) const
	{
		if (pos != -1 && classCname != enumNameArray[pos]) return -1;
		if (enumNameArray.size() == pos + 2)
		{
			for (const auto & luanameToEnumId : { luanameToRegularEnumId, luanameToClassEnumId })
			{
				for (const auto & luanameEnumId : luanameToEnumId)
				{
					const auto & enumRec = EnumRec::Get(luanameEnumId.second);
					if (enumRec.enumCname == enumNameArray[pos + 1])
						return enumRec.id;
				}
			}
			return -1;
		}
		int targetId = -1;
		for (auto i = memberClassIdArray.begin(); targetId == -1 && i != memberClassIdArray.end(); i++)
			targetId = ClassRec::Get(*i).FindLowerEnum(enumNameArray, pos + 1);
		return targetId;
	}

	//-------------------------------------------------------------

	/// <summary>
	/// Convert Fullpath style name to unique style name.
	/// e.g. "::A::B::C" -> "lm__A__B__C__"
	/// </summary>
	/// <param name="fullpathName">Name of the fullpath.</param>
	/// <returns></returns>
	static std::string FullpathNameToUniqueName(const std::string & fullpathName)
	{
		return std::string("lm") + UtilString::Replace(fullpathName, "::", "__") + "__";
	}

	//-------------------------------------------------------------

	static std::vector<std::unique_ptr<ClassRec>> CLASSREC_ARRAY;
	static ClassRec & Get(int id)
	{
		return *CLASSREC_ARRAY[id].get();
	}
	static ClassRec & New(int parentId)
	{
		CLASSREC_ARRAY.emplace_back(std::make_unique<ClassRec>((int)CLASSREC_ARRAY.size(), parentId));
		return *CLASSREC_ARRAY.back().get();
	}
}; // struct ClassRec.

std::vector<std::unique_ptr<ClassRec>> ClassRec::CLASSREC_ARRAY;

//////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Gets the fullpath element prefix for C++.
/// e.g. class A { class B { enum C {} }}  -> "::A::B::"
/// e.g. class A { class B { enum class C {} }}  -> "::A::B::C::"
/// </summary>
/// <returns></returns>
std::string EnumRec::GetFullpathElementCprefix() const
{
	auto className = ClassRec::Get(parentId).GetFullpathCname();
	if (enumType == EnumRec::Type::ENUM_CLASS)
		return className + "::" + enumCname + "::";
	else
		return className + "::";
}

/// <summary>
/// Gets the fullpath element prefix for Lua.
/// e.g. class A { class B { enum C {} }}  -> "A.B."
/// e.g. class A { class B { enum class C {} }}  -> "A.B.C."
/// </summary>
/// <returns></returns>
std::string EnumRec::GetFullpathElementLuaprefix() const
{
//----- 20.03.01  変更前 ()-----
//	auto className = ClassRec::Get(parentId).GetFullpathLuaname();
//	if (enumType == EnumRec::Type::ENUM_CLASS)
//		return className + "." + enumLuaname + ".";
//	else
//		return className + ".";
//----- 20.03.01  変更後 ()-----
	auto className = ClassRec::Get(parentId).GetFullpathLuaname();
	if (className.empty())
		return (enumType == EnumRec::Type::ENUM_CLASS) ?
			enumLuaname + "."
			:
			"";
	else
		return (enumType == EnumRec::Type::ENUM_CLASS) ?
			className + "." + enumLuaname + "."
			:
			className + ".";
//----- 20.03.01  変更終 ()-----
}

/// <summary>
/// Gets the fullpath enum name for C++.
/// e.g. class A { class B { enum C {} }}  -> "::A::B::C"
/// e.g. class A { class B { enum class C {} }}  -> "::A::B::C"
/// </summary>
/// <returns></returns>
std::string EnumRec::GetFullpathCname() const
{
	auto className = ClassRec::Get(parentId).GetFullpathCname();
	return className + "::" + enumCname;
}

/// <summary>
/// Gets the fullpath enum name for Lua.
/// e.g. class A { class B { enum C {} }}  -> error
/// e.g. class A { class B { enum class C {} }}  -> "A.B.C"
/// </summary>
/// <returns></returns>
std::string EnumRec::GetFullpathLuaname() const
{
//----- 20.03.01  変更前 ()-----
//	auto className = ClassRec::Get(parentId).GetFullpathLuaname();
//	if (enumType == EnumRec::Type::ENUM_CLASS)
//		return className + "." + enumLuaname;
//	else
//		ThrowLeSystemError();
//----- 20.03.01  変更後 ()-----
	auto className = ClassRec::Get(parentId).GetFullpathLuaname();
	if (enumType != EnumRec::Type::ENUM_CLASS)
		ThrowLeSystemError();
	return className.empty() ? enumLuaname : className + "." + enumLuaname;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Gets the fullpath constant name.
/// e.g. class A { class B { constexpr int C = 1; }}  -> "::A::B::C"
/// </summary>
/// <returns></returns>
std::string ConstantRec::GetFullpathCname() const
{
	switch (recType)
	{
	case ConstantRec::Type::CONSTANT:
	case ConstantRec::Type::CONSTEXPR:
	{
		return ClassRec::Get(parentId).GetFullpathCname() + "::" + constantCname;
	}
	case ConstantRec::Type::DEFINE:
		return constantCname;
	default:
		ThrowLeSystemError();
	}
}

std::string ConstantRec::GetFullpathLuaname() const
{
	const auto fullpathLuaname = ClassRec::Get(parentId).GetFullpathLuaname();
	return fullpathLuaname.empty() ? constantLuaname : fullpathLuaname + "." + constantLuaname;
} // ConstantRec::GetFullpathLuaname.

//////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Gets the fullpath variable name.
/// e.g. class A { class B { constexpr int C = 1; }}  -> "::A::B::C"
/// </summary>
/// <returns></returns>
std::string VariableRec::GetFullpathCname() const
{
	return ClassRec::Get(parentId).GetFullpathCname() + "::" + variableCname;
}

std::string VariableRec::GetFullpathLuaname() const
{
	const auto fullpathLuaname = ClassRec::Get(parentId).GetFullpathLuaname();
	return fullpathLuaname.empty() ? variableLuaname : fullpathLuaname + "." + variableLuaname;
} // VariableRec::GetFullpathLuaname.


//////////////////////////////////////////////////////////////////////////////////////////////////////

std::string FuncRec::GetWrapperFunctionName() const
{
// 21.05.16 Fukushiro M. 1行変更 ()
//	const int luaArgCount = (type == FuncRec::Type::METHOD) ? (int)inArgNames.size() + 1 : (int)argNames.size();
	const int luaArgCount = (type == FuncRec::Type::METHOD) ? (int)inArgNames.size() + 1 : (int)inArgNames.size();
	return FuncGroupRec::Get(parentId).GetWrapperFunctionName() + "__" + std::to_string(luaArgCount);
}

std::string FuncRec::GetFullpathLuaname() const
{
	const auto & funcGroupRec = FuncGroupRec::Get(parentId);
	const auto classFullpathLuaname = ClassRec::Get(funcGroupRec.parentId).GetFullpathLuaname();
	if (type == FuncRec::Type::METHOD)
		return classFullpathLuaname.empty() ? funcGroupRec.luaname : classFullpathLuaname + ":" + funcGroupRec.luaname;
	else
		return classFullpathLuaname.empty() ? funcGroupRec.luaname : classFullpathLuaname + "." + funcGroupRec.luaname;
}

std::string FuncRec::GetFullpathLuanameForCall() const
{
	if (type == FuncRec::Type::METHOD)
	{
		const auto & funcGroupRec = FuncGroupRec::Get(parentId);
		const auto classFullpathLuaname = ClassRec::Get(funcGroupRec.parentId).GetFullpathLuaname();
		return classFullpathLuaname.empty() ?
			funcGroupRec.luaname
			:
			std::string("[") + classFullpathLuaname + " classobject]:" + funcGroupRec.luaname;
	}
	else
		return GetFullpathLuaname();
}

std::string FuncRec::GetFullpathCname() const
{
	const auto & funcGroupRec = FuncGroupRec::Get(parentId);
	const auto classFullpathCname = ClassRec::Get(funcGroupRec.parentId).GetFullpathCname();
// 21.05.19 Fukushiro M. 1行変更 ()
//	return classFullpathCname + "::" + funcCname;
	return (type == Type::CONSTRUCTOR) ? classFullpathCname : (classFullpathCname + "::" + funcCname);
}

std::string FuncGroupRec::GetWrapperFunctionName() const
{
	return ClassRec::Get(parentId).GetUniqueClassName() + luaname;
}

std::string FuncGroupRec::GetFullpathLuaname() const
{
	const auto classFullpathLuaname = ClassRec::Get(parentId).GetFullpathLuaname();
	return classFullpathLuaname.empty() ? luaname : classFullpathLuaname + "." + luaname;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Get LuaType which has the C++ type specified by ctype.
/// If matched LuaType does not exist, it returns LuaType::NIL;
/// </summary>
/// <param name="ctype">C++ type</param>
/// <returns>LuaType</returns>
static LuaType CtypeToLuaType(const std::string & ctype)
{
	auto i = CTYPE_TO_LUATYPE.find(ctype);
	return (i != CTYPE_TO_LUATYPE.end()) ? i->second : LuaType::NIL;
}

/// <summary>
/// Converts from LuaType to c++ type name.
/// </summary>
/// <param name="luaType">LuaType</param>
/// <returns>name</returns>
static std::string LuaTypeToCTypeName(LuaType luaType)
{
	const auto i = LTYPE_TO_REC.find(luaType);
	if (i == LTYPE_TO_REC.end())
		ThrowLeException(LeError::TYPE_UNDEFINED, luaType.ToString());
	return i->second.m_ctype;
}

/// <summary>
/// Gets lua function which is used to get the Type of value specified by luaType.
/// e.g. luaType=LuaType::INT64 -> "luaL_checkinteger(L, 1)"
/// </summary>
/// <param name="luaType">Type of the lua.</param>
/// <returns></returns>
static std::string LuaTypeToGetFuncName(LuaType luaType)
{
	const auto i = LTYPE_TO_REC.find(luaType);
	if (i == LTYPE_TO_REC.end())
		ThrowLeException(LeError::TYPE_UNDEFINED, luaType.ToString());
	return i->second.m_getFunc;
}

/// <summary>
/// Gets lua pushing function for the luaType.
/// e.g. luaType=LuaType::INT64 -> lua_pushinteger
/// </summary>
/// <param name="luaType">Type of the lua.</param>
/// <returns></returns>
static std::string LuaTypeToSetFuncName(LuaType luaType)
{
	const auto i = LTYPE_TO_REC.find(luaType);
	if (i == LTYPE_TO_REC.end())
		ThrowLeException(LeError::TYPE_UNDEFINED, luaType.ToString());
	return i->second.m_setFunc;
}

/*
/// <summary>
/// Extract typename from type description, and make full-typename.
/// e.g.
/// varTypeSource="const unsigned   int *" -> typeNameBefore="", typeNameAfter="", return false
/// varTypeSource=" class X * &" -> typeNameBefore="X", typeNameAfter="::A::B::X", return true
/// varTypeSource=" const enum X *" -> typeNameBefore="X", typeNameAfter="::A::B::X", return true
/// </summary>
/// <param name="typeNameBefore">typename</param>
/// <param name="typeNameAfter">full typename</param>
/// <param name="varTypeSource">Source text of variable's type description.</param>
/// <param name="classRec">Class record.</param>
/// <returns>true:replacement was fond/false:replacement was not fond</returns>
static bool NormalizeVarTypeSubSub(
	std::string & typeNameBefore,
	std::string & typeNameAfter,
	const std::string & varTypeSource,
	const ClassRec & classRec
)
{
	static const std::regex REX_CONST(R"(\bconst\b)");

	typeNameBefore.clear();
	typeNameAfter.clear();

	// e.g. "const enum class  TypeU * & "
	auto typeName = varTypeSource;

	// e.g. "const enum class  TypeU * & " -> "const enum class  TypeU   "
	typeName = UtilString::Replace(typeName, "*", "", "&", "");

	// e.g. "const enum class  TypeU   " -> " enum class  TypeU   "
	typeName = std::regex_replace(typeName, REX_CONST, "");

	// e.g. " enum class  TypeU   " -> " enum class TypeU "
	typeName = std::regex_replace(typeName, MULTISPACE_DEF, " ");

	// e.g. " enum class TypeU " -> "enum class TypeU"
	UtilString::Trim(typeName);

	// e.g. typeName="enum class TypeU", "struct TypeU", "TypeU", etc.
	
	if (classRec.classType != ClassRec::Type::NONE)
	{
		std::string fullTypeName;	// e.g. "::A::B::X"
		std::smatch results;
		if (
			// Extracts 'CCC' from "enum CCC "
			// ENUM_NAME_XML(R"(^enum\s+([a-zA-Z_]\w*)\s*$)");
			// Extracts 'CCC' from "enum class CCC "
			// ENUM_CLASS_NAME_XML(R"(^enum\s+class\s+([a-zA-Z_]\w*)\b\s*$)");

			std::regex_search(typeName, results, ENUM_NAME_XML) ||
			std::regex_search(typeName, results, ENUM_CLASS_NAME_XML)
			)
		{	// if "enum ~".
			// e.g. "enum class TypeU" -> "TypeU"
			typeName = results[1].str();
			const auto enumId = classRec.FindEnum(typeName);
			if (enumId == -1)
				ThrowLeException(LeError::TYPE_UNDEFINED, typeName);
			const auto & enumRec = EnumRec::Get(enumId);
			// e.g. "TypeU" -> "::A::B::TypeU"
			fullTypeName = enumRec.GetFullpathCname();
		}
		else if (
			// Extracts 'CCC' from "class CCC ~"
			// CLASS_XML(R"(^class\s+([a-zA-Z_]\w*)\b)");
			// Extracts 'CCC' from "struct CCC ~"
			// STRUCT_XML(R"(^struct\s+([a-zA-Z_]\w*)\b)");
			// Extracts 'CCC' from "union CCC ~"
			// UNION_XML(R"(^union\s+([a-zA-Z_]\w*)\b)");

			std::regex_search(typeName, results, CLASS_XML) ||
			std::regex_search(typeName, results, STRUCT_XML) ||
			std::regex_search(typeName, results, UNION_XML)
			)
		{	// if "class ~", "struct ~"
			// e.g. "class TypeU" -> "TypeU"
			typeName = results[1].str();
			const auto classId = classRec.FindClass(typeName);
			if (classId == -1)
				ThrowLeException(LeError::TYPE_UNDEFINED, typeName);
			const auto & classRec = ClassRec::Get(classId);
			// e.g. "TypeU" -> "::A::B::TypeU"
			fullTypeName = classRec.GetFullpathCname();
		}
		else
		{
			const auto enumId = classRec.FindEnum(typeName);
			if (enumId != -1)
			{
				// e.g. "TypeU" <--- if "TypeU" of "enum TypeU"
				const auto & enumRec = EnumRec::Get(enumId);
				// e.g. "TypeU" -> "::A::B::TypeU"
				fullTypeName = enumRec.GetFullpathCname();
			}
			else
			{
				const auto classId = classRec.FindClass(typeName);
				if (classId != -1)
				{
					// e.g. "TypeU" <--- if "TypeU" of "class TypeU"
					const auto & classRec = ClassRec::Get(classId);
					// e.g. "TypeU" -> "::A::B::TypeU"
					fullTypeName = classRec.GetFullpathCname();
				}
			}
		}

		if (!fullTypeName.empty())
		{
			typeNameBefore = typeName;
			typeNameAfter = fullTypeName;
		}
	}
	return !typeNameBefore.empty();
} // NormalizeVarTypeSubSub.
*/

/// <summary>
/// Normalizes the Type of the variable.
/// 1. Change two or more spaces to single space.
/// 2. Change Class name and enum name to full name. e.g. X -> ::A::B::X   ( If class A { class B { class X ... }}} )
/// 3. Remove spaces in front of * and &.
/// varType only.
/// 4. Remove "class", "enum", "struct", "union" and "const".
/// e.g.
/// varTypeSource="const unsigned   int *" -> varRawType="const unsigned int*", varType="unsigned int*"
/// varTypeSource=" class X * &" -> varRawType="class ::A::B::X*&", varType="::A::B::X*"
/// varTypeSource=" const enum X *" -> varRawType="const enum ::A::B::X*", varType="::A::B::X*"
/// </summary>
/// <param name="varType">Normalized type of the variable.</param>
/// <param name="varRawType">Normalized raw type of the variable.</param>
/// <param name="varTypeSource">Source text of variable's type definition.</param>
/// <param name="classRec">Class record.</param>
static void NormalizeVarTypeSub(
	std::string & varType,
	std::string & varRawType,
	const std::string & varTypeSource,
	const ClassRec & classRec
)
{
	static const std::regex REX_ENUM(R"(\benum\b)");
	static const std::regex REX_CLASS(R"(\bclass\b)");
	static const std::regex REX_STRUCT(R"(\bstruct\b)");
	static const std::regex REX_UNION(R"(\bunion\b)");
	static const std::regex REX_CONST(R"(\bconst\b)");
	static const std::regex SPACE_AMP(R"(\s+\&)");
	static const std::regex SPACE_ASTAR(R"(\s+\*)");

	// e.g. "const enum class  TypeU * & "
	auto typeName = varTypeSource;

	// e.g. "const enum class  TypeU * & " -> "const enum class  TypeU   "
	typeName = UtilString::Replace(typeName, "*", "", "&", "");

	// e.g. "const enum class  TypeU   " -> " enum class  TypeU   "
	typeName = std::regex_replace(typeName, REX_CONST, "");

	// e.g. " enum class  TypeU   " -> " enum class TypeU "
	typeName = std::regex_replace(typeName, MULTISPACE_DEF, " ");

	// e.g. " enum class TypeU " -> "enum class TypeU"
	UtilString::Trim(typeName);

	// e.g. typeName="enum class TypeU", "struct TypeU", "TypeU", etc.

	// e.g. "const enum class  TypeU * & "
	varRawType = varTypeSource;

	if (!typeName.empty() && classRec.classType != ClassRec::Type::NONE)
	{
		std::string fullTypeName;	// e.g. "::A::B::X"
		std::smatch results;
		if (
			// Extracts 'CCC' from "enum CCC "
			// ENUM_NAME_XML(R"(^enum\s+([a-zA-Z_]\w*)\s*$)");
			// Extracts 'CCC' from "enum class CCC "
			// ENUM_CLASS_NAME_XML(R"(^enum\s+class\s+([a-zA-Z_]\w*)\b\s*$)");

			std::regex_search(typeName, results, ENUM_NAME_XML) ||
			std::regex_search(typeName, results, ENUM_CLASS_NAME_XML)
			)
		{	// if "enum ~".
			// e.g. "enum class TypeU" -> "TypeU"
			typeName = results[1].str();
			const auto enumId = classRec.FindEnum(typeName);
			if (enumId == -1)
				ThrowLeException(LeError::TYPE_UNDEFINED, typeName);
			const auto & enumRec = EnumRec::Get(enumId);
			// e.g. "TypeU" -> "::A::B::TypeU"
			fullTypeName = enumRec.GetFullpathCname();
		}
		else if (
			// Extracts 'CCC' from "class CCC ~"
			// CLASS_XML(R"(^class\s+([a-zA-Z_]\w*)\b)");
			// Extracts 'CCC' from "struct CCC ~"
			// STRUCT_XML(R"(^struct\s+([a-zA-Z_]\w*)\b)");
			// Extracts 'CCC' from "union CCC ~"
			// UNION_XML(R"(^union\s+([a-zA-Z_]\w*)\b)");

			std::regex_search(typeName, results, CLASS_XML) ||
			std::regex_search(typeName, results, STRUCT_XML) ||
			std::regex_search(typeName, results, UNION_XML)
			)
		{	// if "class ~", "struct ~"
			// e.g. "class TypeU" -> "TypeU"
			typeName = results[1].str();
			const auto classId = classRec.FindClass(typeName);
			if (classId == -1)
				ThrowLeException(LeError::TYPE_UNDEFINED, typeName);
			const auto & classRec = ClassRec::Get(classId);
			// e.g. "TypeU" -> "::A::B::TypeU"
			fullTypeName = classRec.GetFullpathCname();
		}
		else
		{
			const auto enumId = classRec.FindEnum(typeName);
			if (enumId != -1)
			{
				// e.g. "TypeU" <--- if "TypeU" of "enum TypeU"
				const auto & enumRec = EnumRec::Get(enumId);
				// e.g. "TypeU" -> "::A::B::TypeU"
				fullTypeName = enumRec.GetFullpathCname();
			}
			else
			{
				const auto classId = classRec.FindClass(typeName);
				if (classId != -1)
				{
					// e.g. "TypeU" <--- if "TypeU" of "class TypeU"
					const auto & classRec = ClassRec::Get(classId);
					// e.g. "TypeU" -> "::A::B::TypeU"
					fullTypeName = classRec.GetFullpathCname();
				}
			}
		}

		if (!fullTypeName.empty())
		{
			// e.g. typeName="TypeU" -> REX_TYPE="\bTypeU\b"
			const std::regex REX_TYPE(std::string("\\b") + typeName + "\\b");
			// e.g. "const enum class  TypeU * & " -> "const enum class  ::A::B::TypeU * & "
			varRawType = std::regex_replace(varRawType, REX_TYPE, fullTypeName);
		}
	}

	// e.g. "const enum class  ::A::B::TypeU * & " -> "const enum class  ::A::B::TypeU *& "
	varRawType = std::regex_replace(varRawType, SPACE_AMP, "&");

	// e.g. "const enum class  ::A::B::TypeU *& " -> "const enum class  ::A::B::TypeU*& "
	varRawType = std::regex_replace(varRawType, SPACE_ASTAR, "*");

	// e.g. "const enum class  ::A::B::TypeU*& " -> "const enum class ::A::B::TypeU*& "
	varRawType = std::regex_replace(varRawType, MULTISPACE_DEF, " ");

	// e.g. "const enum class ::A::B::TypeU*& " -> "const enum class ::A::B::TypeU*&"
	UtilString::Trim(varRawType);

	// e.g. "const enum class ::A::B::TypeU*&"
	varType = varRawType;

	// e.g. "const enum class ::A::B::TypeU*" -> "const  class ::A::B::TypeU*"
	varType = std::regex_replace(varType, REX_ENUM, "");

	// e.g. "const  class ::A::B::TypeU*" -> "const   ::A::B::TypeU*"
	varType = std::regex_replace(varType, REX_CLASS, "");

	// e.g. "const  struct ::A::B::TypeU*" -> "const   ::A::B::TypeU*"
	varType = std::regex_replace(varType, REX_STRUCT, "");

	// e.g. "const  union ::A::B::TypeU*" -> "const   ::A::B::TypeU*"
	varType = std::regex_replace(varType, REX_UNION, "");

	// e.g. "const   ::A::B::TypeU*" -> "   ::A::B::TypeU*"
	// e.g. "char const*" -> "char *"
	varType = std::regex_replace(varType, REX_CONST, "");

	varType = std::regex_replace(varType, SPACE_AMP, "&");

	// e.g. "char *" -> "char*"
	varType = std::regex_replace(varType, SPACE_ASTAR, "*");

	// e.g. "   ::A::B::TypeU*" -> " ::A::B::TypeU*"
	varType = std::regex_replace(varType, MULTISPACE_DEF, " ");

	// e.g. " ::A::B::TypeU*" -> "::A::B::TypeU*"
	UtilString::Trim(varType);
} // NormalizeVarType.

/// <summary>
/// Normalizes the Type of the variable.
/// 1. Change two or more spaces to single space.
/// 2. Change Class name and enum name to full name. e.g. X -> ::A::B::X   ( If class A { class B { class X ... }}} )
/// 3. Remove spaces in front of * and &.
/// varType only.
/// 4. Remove "class", "enum", "struct", "union" and "const".
/// e.g.
/// varTypeSource="const unsigned   int *" -> varRawType="const unsigned int*", varType="unsigned int*"
/// varTypeSource=" class X * &" -> varRawType="class ::A::B::X*&", varType="::A::B::X*"
/// varTypeSource=" const enum X *" -> varRawType="const enum ::A::B::X*", varType="::A::B::X*"
/// </summary>
/// <param name="varType">Normalized type of the variable.</param>
/// <param name="varRawType">Normalized raw type of the variable.</param>
/// <param name="varTypeSource">Source text of variable's type definition.</param>
/// <param name="classRec">Class record.</param>
static void NormalizeVarType(
	std::string & varType,
	std::string & varRawType,
	const std::string varTypeSource,
	const ClassRec & classRec
)
{
	varType.clear();
	varRawType.clear();
	auto source = varTypeSource;
	while (!source.empty())
	{
		// varTypeSource : "  const  type1  < type2< type3 *, const type4 > , type5 > * & "
		// -> left : "  const  type1  "
		// -> right : " type2< type3 *, const type4 > , type5 > * & "

		std::string left;
		const auto separator = UtilString::Split(left, source, std::string(source), "<>,");
		std::string varTypeTmp;
		std::string varRawTypeTmp;
		NormalizeVarTypeSub(varTypeTmp, varRawTypeTmp, left, classRec);
		varType += varTypeTmp;
		varRawType += varRawTypeTmp;
		if (separator != '\0')
		{
			varType += separator;
			varRawType += separator;
		}
	}
}

/// <summary>
/// This is sub-function for GetCppToLuaConverter and GetLuaToCppConverter. It's a recursive function.
/// This function find the shortest path from C++ type to Lua type.
/// </summary>
/// <param name="isForwardSearch">true:Find C++ to Lua conversion path/false:Find Lua to C++ conversion path. But successPath is reversed.</param>
/// <param name="goalCType">empty:Do not specify the goal C++ type/not empty:Goal C++ type. End of conversion path must be this type.</param>
/// <param name="targetConverter">Converter before targetType</param>
/// <param name="targetType">Target type</param>
/// <param name="tempPath">Temporary path from the first type to the type before targetConverter</param>
/// <param name="passedSet">Type set that are passed through.</param>
/// <param name="successPath">Success path</param>
/// <param name="isSuccessPathUpdated">true:Success path is updated/false:not updated</param>
/// <returns>NONE:Succeeded/ErrorCode:Error.</returns>
static LeError::ErrorCode MySearchC_LuaPath(
	bool isForwardSearch,
	const std::string & goalCType,
	const std::string & targetConverter,
	const std::string & targetType,
	std::vector<std::string> & tempPath,
	std::unordered_set<std::string> & passedSet,
	std::vector<std::string> & successPath,
	bool & isSuccessPathUpdated
)
{
	// <sticktype name="Ltyp3" ctype="type3" getfunc="getluavalue3" setfunc="pushluavalue3" />
	// <sticktype name="Ltyp6" ctype="type6" getfunc="getluavalue6" setfunc="pushluavalue6" />
	// <stickconv type1="type3" type2="type2" type1to2="converter3" type2to1="converter3" />
	// <stickconv type1="type4" type2="type2" type1to2="converter4" type2to1="converter4" />
	// <stickconv type1="type6" type2="type5" type1to2="converter6" type2to1="converter6" />
	// <stickconv type1="type7" type2="type5" type1to2="converter7" type2to1="converter7" />
	// <stickconv type1="type2" type2="type1" type1to2="converter2" type2to1="converter2" />
	// <stickconv type1="type5" type2="type1" type1to2="converter5" type2to1="converter5" />
	// <stickconv type1="type1" type2="type0" type1to2="converter1" type2to1="converter1" />
	//
	// e.g. Forward search
	//
	//                                                                       converter3 +---------+
	//                                                                     +----------->|Lua Ltyp3|
	//                                           converter2 +---------+    |            +---------+
	//                                         +----------->|C++ type2|----+
	//                                         |            +---------+    | converter4 +---------+
	//                                         |                           +----------->|C++ type4|
	//  +---------+  converter1 +---------+    |                                        +---------+
	//  |C++ type0|------------>|C++ type1|----+
	//  +---------+             +---------+    |                             converter6 +---------+
	//                                         |                           +----------->|Lua Ltyp6|
	//                                         | converter5 +---------+    |            +---------+
	//                                         +----------->|C++ type5|----+
	//                                         |            +---------+    | converter7 +---------+
	//                                         :                 A         +----------->|C++ type7|
	//                                         :                 |                      +---------+
	//                                                         target
	//
	// targetConverter="converter5"
	// targetType="type5"
	// tempPath={ "", "type0", "converter1", "type1" }
	// successPath={ "", "type0", "converter1", "type1", "converter2", "type2", "converter3", "Ltyp3" }
	// passedSet={ "type0", "type1" }
	//
	//
	// e.g. Backward search
	//                                                                       converter3 +---------+
	//                                                                     +------------|Lua Ltyp3|
	//                                           converter2 +---------+    |            +---------+
	//                                         +------------|C++ type2|<---|
	//                                         |            +---------+    | converter4 +---------+
	//                                         |                           +------------|C++ type4|
	//  +---------+  converter1 +---------+    |                                        +---------+
	//  |C++ type0|<------------|C++ type1|<---|
	//  +---------+             +---------+    |                             converter6 +---------+
	//                                         |                           +------------|Lua Ltyp6|
	//                                         | converter5 +---------+    |            +---------+
	//                                         +------------|C++ type5|<---|
	//                                         |            +---------+    | converter7 +---------+
	//                                         :                 A         +------------|C++ type7|
	//                                         :                 |                      +---------+
	//                                                         target
	//
	// targetConverter="converter5"
	// targetType="type5"
	// tempPath={ "", "type0", "converter1", "type1" }
	// successPath={ "", "type0", "converter1", "type1", "converter2", "type2", "converter3", "Ltyp3" }
	// passedSet={ "type0", "type1" }

	LeError::ErrorCode errorCode = LeError::ErrorCode::NONE;

	// If the searching process has already passed through targetType then stop this search. It is to avoid nest search.
	if (!passedSet.insert(targetType).second)
		return errorCode;

	// e.g. passedSet={ "type0", "type1", "type5" }

	tempPath.emplace_back(targetConverter);
	tempPath.emplace_back(targetType);
	// e.g. tempPath={ "", "type0", "converter1", "type1", "converter5", "type5" }

	for (;;)
	{
		if (targetType == goalCType)
		{	//----- Reaches goal C++ type of Lua type -----
			successPath = tempPath;
			// e.g. successPath={ "", "type0", "converter1", "type1", "converter5", "type5" }
			isSuccessPathUpdated = true;
			break;
		}

		if (goalCType.empty() && !CtypeToLuaType(targetType).IsNull())
		{	//----- Reaches C++ type of Lua type -----
			successPath = tempPath;
			// e.g. successPath={ "", "type0", "converter1", "type1", "converter5", "type5" }
			isSuccessPathUpdated = true;
			break;
		}

		//----- Not reach Lua-type -----
		const auto & ctypeToConverter = isForwardSearch ? CTYPE_1TO2_CONVERTER : CTYPE_2TO1_CONVERTER;
		for (auto i = ctypeToConverter.lower_bound(std::make_pair(targetType, std::string("")));
			 i != ctypeToConverter.end();
			 i++)
		{
			if (i->first.first != targetType) break;

			// tempPath needs 2 more elements. So, if tempPath.size+2 is equal or greater than successPath.size,
			// it is decided that successPath is better than tempPath.
			if (!successPath.empty() && successPath.size() <= tempPath.size() + 2)
				break;

			auto tmpErrCode = MySearchC_LuaPath(
				isForwardSearch,
				goalCType,
				i->second,			// Next target converter. e.g. "converter6"
				i->first.second,	// Next target type. e.g. "Lua Ltyp6"
				tempPath,			// e.g. tempPath={ "", "type0", "converter1", "type1", "converter5", "type5" }
				passedSet,			// e.g. passedSet={ "type0", "type1", "type5" }
				successPath,		// e.g. successPath={ "", "type0", "converter1", "type1", "converter2", "type2", "converter3", "Ltyp3" }
				isSuccessPathUpdated
			);
			if (errorCode == LeError::ErrorCode::NONE)
				errorCode = tmpErrCode;
		}
		break;
	}

	tempPath.pop_back();
	tempPath.pop_back();
	// e.g. tempPath={ "", "type0", "converter1", "type1" }

	passedSet.erase(targetType);
	// e.g. passedSet={ "type0", "type1" }

	if (!successPath.empty())
		errorCode = LeError::ErrorCode::NONE;
	return errorCode;
} // MySearchC_LuaPath

/// <summary>
/// This function receive the goal C++ type and find the shortest Lua-to-C++ conversion path.
/// </summary>
/// <param name="luaType">Start Lua type. Not required. if LuaType::NIL, do not specify start Lua type</param>
/// <param name="cType">Goal C++ type.Required. e.g. "::A::B::X*", "int&". It must be the result:varType of NormalizeVarType.</param>
/// <param name="successPath">Shortest conversion path from Lua-type to C++-type.</param>
/// <exception cref="LeException"></exception>
static void GetLuaToCppConverter(
	const LuaType & luaType,
	const std::string & cType,
	std::vector<std::string> & successPath
)
{
	// e.g.
	//                                                                       converter3 +---------+
	//                                                                     +------------|C++ type3|<- - - -
	//                                           converter2 +---------+    |            +---------+
	//                                         +------------|C++ type2|<---|
	//                                         |            +---------+    | converter4 +---------+
	//                                         |                           +------------|C++ type4|<- - - -
	//  +---------+  converter1 +---------+    |                                        +---------+
	//  |C++ type0|<------------|C++ type1|<---|
	//  +---------+             +---------+    |                             converter6 +---------+
	//                                         |                           +------------|Lua Ltyp6|
	//                                         | converter5 +---------+    |            +---------+
	//                                         +------------|C++ type5|<---|
	//                                         |            +---------+    | converter7 +---------+
	//                                         :                           +------------|Lua Ltyp7|
	//                                         :                                        +---------+
	//
	// Set shortest path to successPath.
	// successPath = { "Ltyp6", "converter6", "type5", "converter5", "type1", "converter1", "type0" }
	//             =>{ "Ltyp6", "converter6", "type5", "converter5", "type1", "converter1", "type0", "&", "type0*" }
	// +---------------------------+
	// | e.g. void Func(type0* x); |
	// | ------------------------- |
	// | Ltyp6 a;                  |
	// | get_lvalue(a);            |
	// | type5 b;                  |
	// | converter6(b, a);         |
	// | type1 c;                  |
	// | converter5(c, b);         |
	// | type0 d;                  |
	// | converter1(d, c);         |
	// | type0* e = &d;            |
	// | Func(e);                  |
	// +---------------------------+
	// Note:Last type of conversion path must be same with the type of argument.
	//      Because, when io="inout", Lua->C++ conversion path's last type must match with
	//      first type of C++->Lua conversion path.

	// + Searching Conversion path.
	//
	// + Lua -> C++
	//
	// Conversion path search method for type T.
	// +----------------------------------------------------------------------+
	// | 1. Search path from start point T.                                   |
	// |   1. If path exists.                                                 |
	// |     1. path={ LuaType -> converter -> ... T }        <-- PathType[1] |
	// |   2. If path doesn't exist.                                          |
	// |     1. If T is S*                                                    |
	// |       1. Search path from start point S.                             |
	// |         1. If path exists.                                           |
	// |           1. path={ LuaType -> converter -> ... S }  <-- PathType[2] |
	// |     2. If T is not S*.                                               |
	// |       1. Search path from start point T*.                            |
	// |         1. If path exists.                                           |
	// |           1. path={ LuaType -> converter -> ... T* } <-- PathType[3] |
	// +----------------------------------------------------------------------+
	//
	// *** Case : Arguments of function ******************************************************
	//
	// Rule-1
	// Type of argument is "T&"
	//
	//   [1] path={ LuaType -> converter -> ... T }
	//          =>{ LuaType -> converter -> ... T }  Not change
	//   [2] path={ LuaType -> converter ... -> S }  (T = S*)
	//          =>{ LuaType -> converter ... -> S -> & -> S* }  Add &->S*
	//   [3] path={ LuaType -> converter ... -> T* }
	//          =>{ LuaType -> converter ... -> T* -> * -> T& }  Add *->T&
	//
	//
	//   e.g.[1] : void Func(int& a);
	//   { __int64 -> v_to_v -> int }
	//   ------------------------
	//   __int64 a;
	//   get_lvalue(a);
	//   int b;
	//   v_to_v(b, a);
	//   Func(b)
	//   ------------------------
	//
	//   e.g.[1] : void Func(std::string& a);
	//   { std::string }
	//   ------------------------
	//   std::string a;
	//   get_lvalue(a);
	//   Func(a)
	//   ------------------------
	//
	//   e.g.[1] : void Func(A*& a);
	//   { classobj -> p_to_p -> A* }
	//   ------------------------
	//   classobj a;
	//   get_lvalue(a);
	//   A* b;
	//   p_to_p(b, a);
	//   Func(b);
	//   ------------------------
	//
	//   e.g.[2] : void Func(A*& a);
	//   <stickcnv A -> std::string>
	//   { std::string -> str_to_A -> A -> & -> A* }
	//   ------------------------
	//   std::string a;
	//   get_lvalue(a);
	//   A b;
	//   str_to_A(b, a);
	//   A* c = &b;
	//   Func(c);
	//   ------------------------
	//
	//   e.g.[2] : void Func(int*& a);
	//   { __int64 -> v_to_v -> int -> & -> int* }
	//   ------------------------
	//   __int64 a;
	//   get_lvalue(a);
	//   int b;
	//   v_to_v(b, a);
	//   int* c = &b;
	//   Func(c);
	//   ------------------------
	//
	//   e.g.[3] : void Func(A& a);
	//   { classobj -> p_to_p -> A* -> * -> A& }
	//   ------------------------
	//   classobj a;
	//   get_lvalue(a);
	//   A* b;
	//   p_to_p(b, a);
	//   A& c = *b;
	//   Func(c);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-2
	// Type of argument is "S*"
	//
	//   Same with Rule-1.
	//
	//   [1] path={ LuaType -> converter -> ... S* }
	//          =>{ LuaType -> converter -> ... S* }  Not change
	//   [2] path={ LuaType -> converter ... -> S }
	//          =>{ LuaType -> converter ... -> S -> & -> S* }  Add &->S*
	//
	//
	//   e.g.[1] : void Func(A* a);
	//   =>{ classobj -> p_to_p -> A* }
	//   ------------------------
	//   classobj a;
	//   get_lvalue(a);
	//   A* b;
	//   p_to_p(b, a);
	//   Func(b);
	//   ------------------------
	//
	//   e.g.[2] : void Func(int* a);
	//   =>{ __int64 -> v_to_v -> int -> & -> int* }
	//   ------------------------
	//   __int64 a;
	//   get_lvalue(a);
	//   int b;
	//   v_to_v(b, a);
	//   int* c = &b;
	//   Func(c);
	//   ------------------------
	//
	//   e.g.[2] : void Func(A* a);
	//   <stickcnv A -> std::string>
	//   =>{ std::string -> str_to_A -> A -> & -> A* }
	//   ------------------------
	//   std::string a;
	//   get_lvalue(a);
	//   A b;
	//   str_to_A(b, a);
	//   A* c = &b;
	//   Func(c);
	//   ------------------------
	//
	//   e.g.[2] : void Func(char* a);
	//   =>{ __int64 -> v_to_v -> char -> & -> char* }
	//   ------------------------
	//   __int64 a;
	//   get_lvalue(a);
	//   char b;
	//   v_to_v(b, a);
	//   char* c = &b;
	//   Func(c);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-3
	// Type of argument is "T"
	//
	//   Same with Rule-1.
	//
	//   [1] path={ LuaType -> converter -> ... T }
	//          =>{ LuaType -> converter -> ... T }  Not change
	//   [3] path={ LuaType -> converter ... -> T* }
	//          =>{ LuaType -> converter ... -> T* -> * -> T& }  Add *->T&
	//
	//
	//   e.g.[1] : void Func(int a);
	//   =>{ __int64 -> v_to_v -> int }
	//   ------------------------
	//   __int64 a;
	//   get_lvalue(a);
	//   int b;
	//   v_to_v(b, a);
	//   Func(b);
	//   ------------------------
	//
	//   e.g.[1] : void Func(A a);
	//   <stickcnv A -> std::string>
	//   =>{ std::string -> str_to_A -> A }
	//   ------------------------
	//   std::string a;
	//   get_lvalue(a);
	//   A b;
	//   str_to_A(b, a);
	//   Func(b);
	//   ------------------------
	//
	//   e.g.[3] : void Func(A a);
	//   =>{ classobj -> p_to_p -> A* -> * -> A& }
	//   ------------------------
	//   classobj a;
	//   get_lvalue(a);
	//   A* b;
	//   p_to_p(b, a);
	//   A& c = *b;
	//   Func(c);
	//   ------------------------
	//
	// ------------------------------------------------------------------------

	enum
	{
		PATH_NONE = 0,
		PATH_TYPE_1,		// PathType[1]
		PATH_TYPE_2,		// PathType[2]
		PATH_TYPE_3,		// PathType[3]
	} pathType = PATH_NONE;

	successPath.clear();

	std::string cTypeOfLua;
	if (!luaType.IsNull())
		cTypeOfLua = LuaTypeToCTypeName(luaType);

	// Type T above description. Remove last '&'.  e.g. "char*&" -> "char*"
	std::string cType_T = cType;
	UtilString::TrimRight(cType_T, '&');
	// Type S above description. Remove last '*'.  e.g. "char*" -> "char"
	const auto cType_S = (cType_T.back() == '*') ? cType_T.substr(0, cType_T.length() - 1) : "";

	// Get a reverse conversion path.
	// e.g. successPath = { "", "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
	bool isSuccessPathUpdated = false;
	auto errorCode = MySearchC_LuaPath(
		false,				// Backward search.
		cTypeOfLua,
		std::string(),		// First converter. Dummy.
		cType_T,			// First C++ type.
		Dummy<std::vector<std::string>>(),
		Dummy<std::unordered_set<std::string>>(),
		successPath,
		isSuccessPathUpdated
	);
	if (isSuccessPathUpdated)
	{
		pathType = PATH_TYPE_1;
	} else
	{
		LeError::ErrorCode tmpErrCode;
		if (cType_T.back() == '*')
		{
			isSuccessPathUpdated = false;
			tmpErrCode = MySearchC_LuaPath(
				false,				// Backward search.
				cTypeOfLua,
				std::string(),		// First converter. Dummy.
				cType_S,	// First C++ type.
				Dummy<std::vector<std::string>>(),
				Dummy<std::unordered_set<std::string>>(),
				successPath,
				isSuccessPathUpdated
			);
			if (isSuccessPathUpdated)
				pathType = PATH_TYPE_2;
		}
		if (cType_T.back() != '*')
		{
			isSuccessPathUpdated = false;
			tmpErrCode = MySearchC_LuaPath(
				false,				// Backward search.
				cTypeOfLua,
				std::string(),		// First converter. Dummy.
				cType_T + '*',		// First C++ type. e.g. "char" -> "char*"
				Dummy<std::vector<std::string>>(),
				Dummy<std::unordered_set<std::string>>(),
				successPath,
				isSuccessPathUpdated
			);
			if (isSuccessPathUpdated)
				pathType = PATH_TYPE_3;
		}
		if (errorCode == LeError::ErrorCode::NONE)
			errorCode = tmpErrCode;
	}
	if (pathType != PATH_NONE)
	{
		// Remove the first dummy converter.
		// e.g. successPath = { "", "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
		//                 -> { "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
		successPath.erase(successPath.begin());

		// Think about the following cases:converter is "-".
		//
		// [1] --[""]--> "::ClassX*" --["XptrToVoidPtr"]--> "void*" --["-"]--> "Luastick::lightuserdata"
		// [2] "lightuserdata" --["LuaPtrToVptr"]--> "voidptr" --["-"]--> "void* --[""]-->"
		//
		// In the case [1], eather seems to be fine to remove "void*" and "-" set, or "-" and "Luastick::lightuserdata" set.
		// But "Luastick::lightuserdata" is more important because in some case program have to find the Lua-type "lightuserdata" from "Luastick::lightuserdata".
		// So we should remove "void* and "-".
		// In the case [2], it is OK to remove eather "voidptr" and "-" set or "-" and "void*" set.
		// By the same reason with the above case, we should remove the farther from Lua-side:"-" and "void*" set.
		for (size_t i = 1; i != successPath.size();)
		{
			if (successPath[i] == "-")
				successPath.erase(successPath.begin() + i - 1, successPath.begin() + i + 1);
			else
				i += 2;
		}

		// Normailze the direction of the path.
		// e.g. successPath = { "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
		//                 -> { "Ltyp6", "converter6", "type5", "converter5", "type1", "converter1", "type0" }
		std::reverse(successPath.begin(), successPath.end());

		switch (pathType)
		{
		case PATH_TYPE_1:	// PathType[1]
			break;
//----- 20.02.20  変更前 ()-----
//		case PATH_TYPE_2:	// PathType[2]
//			successPath.insert(successPath.end(), { "&", cType_S + '*' });
//			break;
//		default: // case PATH_TYPE_3:	// PathType[3]
//			successPath.insert(successPath.end(), { "*", cType_T + '&' });
//			break;
//----- 20.02.20  変更後 ()-----
		case PATH_TYPE_2:	// PathType[2]
		{
			auto lastType = successPath.back();
			successPath.insert(successPath.end(), { "&", lastType + '*' });
			break;
		}
		default: // case PATH_TYPE_3:	// PathType[3]
		{
			auto lastType = successPath.back().substr(0, successPath.back().length() - 1);
			successPath.insert(successPath.end(), { "*", lastType + '&' });
			break;
		}
//----- 20.02.20  変更終 ()-----
		}
	}
	else if (errorCode != LeError::ErrorCode::NONE)
	{
		ThrowLeException(errorCode, cType);
	}
	else
	{
		ThrowLeException(LeError::CONVERTER_UNDEFINED, cType);
	}
} // GetLuaToCppConverter.

enum class CppToLuaConversionType
{
	FUNC_OUT_ARGUMENT,		// Output only arguments of function
	FUNC_INOUT_ARGUMENT,	// Input and output arguments of function
	FUNC_RETURN,			// Return value of function
	CONSTANT,				// static const and constexpr
	DEFINE_MACRO,			// #define macro
	VARIABLE,				// Class member variable
};

/// <summary>
/// This function receive the start C++ type and find the shortest C++-to-Lua conversion path.
/// </summary>
/// <param name="cType">Start C++ type.Required. e.g. "::A::B::X*", "int&". It must be the result:varType of NormalizeVarType.</param>
/// <param name="luaType">Goal Lua type. Not required. / LuaType::NIL : do not specify goal Lua type</param>
/// <param name="successPath">Shortest conversion path from C++-type to Lua-type.</param>
/// <exception cref="LeException"></exception>
static void GetCppToLuaConverter(
	CppToLuaConversionType cppToLuaConvType,
	const std::string & cType,
	const LuaType & luaType,
	std::vector<std::string> & successPath
)
{
	// e.g.
	//                                                                       converter3 +---------+
	//                                                                     +----------->|C++ type3|<- - - -
	//                                           converter2 +---------+    |            +---------+
	//                                         +----------->|C++ type2|----+
	//                                         |            +---------+    | converter4 +---------+
	//                                         |                           +----------->|C++ type4|<- - - -
	//  +---------+  converter1 +---------+    |                                        +---------+
	//  |C++ type0|------------>|C++ type1|----+
	//  +---------+             +---------+    |                             converter6 +---------+
	//                                         |                           +----------->|Lua Ltyp6|
	//                                         | converter5 +---------+    |            +---------+
	//                                         +----------->|C++ type5|----+
	//                                         |            +---------+    | converter7 +---------+
	//                                         :                           +----------->|Lua Ltyp7|
	//                                         :                                        +---------+
	//
	// Set shortest path to successPath.
	// successPath = { "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
	//
	// e.g. Output argument.
	// successPath =>{  "", "", "type0*", "*", "type0&", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
	//                                   ^Func(a)
	// +----------------------------+
	// | e.g. void Func(type0*& x); |
	// | -------------------------- |
	// | type0* a;                  |
	// | Func(a);                   |
	// | type0& b = *a;             |
	// | type1 c;                   |
	// | converter1(c, b);          |
	// | type5 d;                   |
	// | converter5(d, c);          |
	// | Ltyp6 e;                   |
	// | converter6(e, d);          |
	// +----------------------------+
	//
	// e.g. Return value.
	// successPath = { "type0*&", "*", "type0&", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
	// +----------------------------+
	// | e.g. type0*& Func();       |
	// | -------------------------- |
	// | type0*& a = Func();        |
	// | type0& b = *a;             |
	// | type1 c;                   |
	// | converter1(c, b);          |
	// | type5 d;                   |
	// | converter5(d, c);          |
	// | Ltyp6 e;                   |
	// | converter6(e, d);          |
	// +----------------------------+

	// + Searching Conversion path.
	//
	// + C++ -> Lua
	//
	// Conversion path search method for type T.
	// +-----------------------------------------------------------------------+
	// | 1. Search path from start point T.                                    |
	// |   1. If path exists.                                                  |
	// |     1. path={ T -> converter -> ... LuaType }         <-- PathType[1] |
	// |   2. If path doesn't exist.                                           |
	// |     1. If T is S*.                                                    |
	// |       1. Search path from start point S.                              |
	// |         1. If path exists.                                            |
	// |           1. path={ S -> converter -> ... LuaType }   <-- PathType[2] |
	// |     2. If T is not S*.                                                |
	// |       1. Search path from start point T*.                             |
	// |         1. If path exists.                                            |
	// |           1. path={ T* -> converter -> ... LuaType }  <-- PathType[3] |
	// +-----------------------------------------------------------------------+
	//
	// *** Case : Output only arguments of function ******************************************************
	//
	// Note: First 3 types are used before calling function.
	//
	// Rule-1
	// Type of argument is "T&"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ "" -> "" -> T -> converter -> ... LuaType }  Insert ""->"" into top of path.
	//   [2] path={ S -> converter ... -> LuaType }  (T = S*)
	//          =>{ "" -> "" -> S* -> * -> S& -> converter -> ... LuaType }  Remove top S and insert ""->""->S*->*->S&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ "" -> "" -> T -> & -> T* -> converter -> ... LuaType }  Insert ""->""->T->& into top of path.
	//
	//   e.g.[1] : void Func(int& a);
	//   =>{ "" -> "" -> int -> v_to_v -> __int64 }
	//   ------------------------
	//   int a;
	//   Func(a);
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(A*& a);
	//   =>{ "" -> "" -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A* a;
	//   Func(a);
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(char*& a);
	//   =>{ "" -> "" -> char* -> text_to_str -> std::string }
	//   ------------------------
	//   char* a;
	//   Func(a);
	//   std::string b;
	//   text_to_str(b, a);
	//   ------------------------
	//
	//   e.g.[2] : void Func(A*& a);
	//   <stickcnv A -> std::string>
	//   =>{ "" -> "" -> A* -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A* a;
	//   Func(a);
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	//   e.g.[3] : void Func(A& a);
	//   =>{ "" -> "" -> A -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A a;
	//   Func(a);
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-2
	// Type of argument is "S*"
	//
	//   [1] path={ S* -> converter -> ... LuaType }
	//          =>{ S -> & -> S* -> converter -> ... LuaType }  Insert S->& into top of path.
	//   [2] path={ S -> converter ... -> LuaType }
	//          =>{ S -> & -> S* -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S->&->S*->*->S&
	//
	//   e.g.[1] : void Func(A* a);
	//   =>{ A -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A x;
	//   A* a = &x;
	//   Func(a);
	//   classobj c;
	//   p_to_p(c, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(char* a);
	//   =>{ char -> & -> char* -> text_to_str -> std::string } }
	//   ------------------------
	//   // NG.
	//   char x;
	//   char* a = &x;
	//   Func(a);
	//   std::string c;
	//   text_to_str(c, a);
	//   ------------------------
	//
	//   e.g.[2] : void Func(int* a);
	//   =>{ int -> & -> int* -> * -> int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int x;
	//   int* a = &x;
	//   Func(a);
	//   int& b = *a;
	//   __int64 c;
	//   v_to_v(c, b);
	//   ------------------------
	//
	//   e.g.[2] : void Func(A* a);
	//   <stickcnv A -> std::string>
	//   =>{ A -> & -> A* -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A x;
	//   A* a = &x;
	//   Func(a);
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-3
	// Type of argument is "T"
	//
	//   Same with Rule-1.
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ "" -> "" -> T -> converter -> ... LuaType }  Insert ""->"" into top of path.
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ "" -> "" -> T -> & -> T* -> converter -> ... LuaType }  Insert ""->""->T->& into top of path.
	//
	//   e.g.[1] : void Func(int a);
	//   =>{ "" -> "" -> int -> v_to_v -> __int64 }
	//   ------------------------
	//   // NG
	//   int a;
	//   Func(a);
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(A a);
	//   <stickcnv A -> std::string>
	//   =>{ "" -> "" -> A -> A_to_str -> std::string }
	//   ------------------------
	//   A a;
	//   Func(a);
	//   std::string b;
	//   A_to_str(b, a);
	//   ------------------------
	//
	//   e.g.[3] : void Func(A a);
	//   =>{ "" -> "" -> A -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A a;
	//   Func(a);
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	//
	// *** Case : Input and output arguments of function ******************************************************
	//
	// Type of first variable must be the last type of Lua to C++ conversion path.
	//
	// Rule-1
	// Type of first variable is "T&"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&.
	//   [2] path={ S -> converter ... -> LuaType }  (T = S*)
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : void Func(int& a);
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = ???;
	//   Func(a);
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(A*& a);
	//   =>{ A*& -> p_to_p -> classobj }
	//   ------------------------
	//   A*& a = ???;
	//   Func(a);
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(char*& a);
	//   =>{ char*& -> text_to_str -> std::string }
	//   ------------------------
	//   char*& a = ???;
	//   Func(a);
	//   std::string b;
	//   text_to_str(b, a);
	//   ------------------------
	//
	//   e.g.[2] : void Func(A*& a);
	//   <stickcnv A -> std::string>
	//   =>{ A*& -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A*& a = ???;
	//   Func(a);
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	//   e.g.[3] : void Func(A& a);
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = ???;
	//   Func(a);
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-2
	// Type of first variable is "S*"
	//
	//   [1] path={ S* -> converter -> ... LuaType }
	//          =>{ S* -> converter -> ... LuaType }  Not change
	//   [2] path={ S -> converter ... -> LuaType }
	//          =>{ S* -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*->*->S&
	//
	//   e.g.[1] : void Func(A* a);
	//   =>{ A* -> p_to_p -> classobj }
	//   ------------------------
	//   A* a = ???;
	//   Func(a);
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(char* a);
	//   =>{ char* -> text_to_str -> std::string } }
	//   ------------------------
	//   char* a = ???;
	//   Func(a);
	//   std::string b;
	//   text_to_str(b, a);
	//   ------------------------
	//
	//   e.g.[2] : void Func(int* a);
	//   =>{ int* -> * -> int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int* a = ???;
	//   Func(a);
	//   int& b = *a;
	//   __int64 c;
	//   v_to_v(c, b);
	//   ------------------------
	//
	//   e.g.[2] : void Func(A* a);
	//   <stickcnv A -> std::string>
	//   =>{ A* -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A* a = ???;
	//   Func(a);
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-3
	// Type of first variable is "T"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T -> converter -> ... LuaType }  Not change
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T -> & -> T* -> converter -> ... LuaType }  Insert T->& into top of path.
	//
	//   e.g.[1] : void Func(int a);
	//   =>{ int -> v_to_v -> __int64 }
	//   ------------------------
	//   int a = ???;
	//   Func(a);
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[1] : void Func(A a);
	//   <stickcnv A -> std::string>
	//   =>{ A -> A_to_str -> std::string }
	//   ------------------------
	//   A a = ???;
	//   Func(a);
	//   std::string b;
	//   A_to_str(b, a);
	//   ------------------------
	//
	//   e.g.[3] : void Func(A a);
	//   =>{ A -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A a = ???;
	//   Func(a);
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	//
	// *** Case : Return value of function ******************************************************
	//
	// Rule-1
	// Type of return value is "T&"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&
	//   [2] path={ S -> converter ... -> LuaType }  (T = S*)
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : int& Func()
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = (int&)Func();
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[2] : A*& Func()
	//   =>{ A*& -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A*& a = (A*&)Func();
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	//   e.g.[3] : A& Func()
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = (A&)Func();
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-2
	// Type of return value is "S*"
	//
	//   [1] path={ S* -> converter -> ... LuaType }
	//          =>{ S* -> converter -> ... LuaType }  Not change
	//   [2] path={ S -> converter ... -> LuaType }
	//          =>{ S* -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*->*->S&
	//
	//   e.g.[1] : A* Func()
	//   =>{ A* -> p_to_p -> classobj }
	//   ------------------------
	//   A* a = Func();
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[2] : int* Func()
	//   =>{ int* -> * -> int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int* a = Func();
	//   int& b = *a;
	//   __int64 c;
	//   v_to_v(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-3
	// Type of return value is "T"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T -> converter -> ... LuaType }  Not change
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T -> & -> T* -> converter -> ... LuaType }  Insert T->& into top of path.
	//
	//   e.g.[1] : int Func()
	//   =>{ int -> v_to_v -> __int64 }
	//   ------------------------
	//   int a = Func();
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[3] : A Func();
	//   =>{ A -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A a = Func();
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// *** Case : Constants ******************************************************
	//
	// Rule-1
	// Type of constant is "T&"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&
	//   [2] path={ S -> converter ... -> LuaType }  (T = S*)
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : static const int& Const = ...
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = (int&)Const;
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[2] : static const A*& Const = ...
	//   =>{ A*& -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A*& a = (A*&)Const;
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	//   e.g.[3] : static const A& Const = ...
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = (A&)Const;
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-2
	// Type of constant is "S*"
	//
	//   [1] path={ S* -> converter -> ... LuaType }
	//          =>{ S*& -> converter -> ... LuaType }  Remove top S* and insert S*&
	//   [2] path={ S -> converter ... -> LuaType }
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//
	//   e.g.[1] : static const A* Const = ...
	//   =>{ A* -> p_to_p -> classobj }
	//   ------------------------
	//   A*& a = (A*&)Const;
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[2] : static const int* Const = ...
	//   =>{ int*& -> * -> int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int*& a = (int*&)Const;
	//   int& b = *a;
	//   __int64 c;
	//   v_to_v(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-3
	// Type of constant is "T"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : static const int Const = ...
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = (int&)Const;
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[3] : static const A Const = ...;
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = (A&)Const;
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// *** Case : #define macro ******************************************************
	//
	// Same with 'Case : Return value of function'.
	//
	// Rule-1
	// Type of macro is "T&"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&
	//   [2] path={ S -> converter ... -> LuaType }  (T = S*)
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : #define DefX xxx // <stick ctype=int&>
	//   { int -> v_to_v -> __int64 }
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = DefX;
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[2] : #define DefX xxx // <stick ctype=A*&>
	//   { A* -> * -> A& -> A_to_str -> std::string }
	//   =>{ A*& -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A*& a = DefX;
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	//   e.g.[3] : #define DefX xxx // <stick ctype=A&>
	//   { A -> & -> A* -> p_to_p -> classobj }
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = DefX;
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-2
	// Type of macro is "S*"
	//
	//   [1] path={ S* -> converter -> ... LuaType }
	//          =>{ S* -> converter -> ... LuaType }  Not change
	//   [2] path={ S -> converter ... -> LuaType }
	//          =>{ S* -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*->*->S&
	//
	//   e.g.[1] : #define DefX xxx // <stick ctype=A*>
	//   { A* -> p_to_p -> classobj }
	//   ------------------------
	//   A* a = DefX;
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[2] : #define DefX xxx // <stick ctype=int*>
	//   { int* -> * -> int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int* a = DefX;
	//   int& b = *a;
	//   __int64 c;
	//   v_to_v(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Rule-3
	// Type of macro is "T"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T -> converter -> ... LuaType }  Not change
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T -> & -> T* -> converter -> ... LuaType }  Insert T->& into top of path.
	//
	//   e.g.[1] : #define DefX xxx // <stick ctype=int>
	//   { int -> v_to_v -> __int64 }
	//   ------------------------
	//   int a = DefX;
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[3] : #define DefX xxx // <stick ctype=A>
	//   { A -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A a = DefX;
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// *** Case : Variables ******************************************************
	//
	// Same with 'Case : Constants'.
	//
	// Rule-1
	// Type of constant is "T&"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&
	//   [2] path={ S -> converter ... -> LuaType }  (T = S*)
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : int& m_var;
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = (int&)m_var;
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[2] : A*& m_var;
	//   =>{ A*& -> * -> A& -> A_to_str -> std::string }
	//   ------------------------
	//   A*& a = (A*&)m_var;
	//   A& b = *a;
	//   std::string c;
	//   A_to_str(c, b);
	//   ------------------------
	//
	//   e.g.[3] : A& m_var;
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = (A&)m_var;
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Same with 'Case : Constants'.
	//
	// Rule-2
	// Type of constant is "S*"
	//
	//   [1] path={ S* -> converter -> ... LuaType }
	//          =>{ S*& -> converter -> ... LuaType }  Remove top S* and insert S*&
	//   [2] path={ S -> converter ... -> LuaType }
	//          =>{ S*& -> * -> S& -> converter -> ... LuaType }  Remove top S and insert S*&->*->S&
	//
	//   e.g.[1] : A* m_var;
	//   =>{ A* -> p_to_p -> classobj }
	//   ------------------------
	//   A*& a = (A*&)m_var;
	//   classobj b;
	//   p_to_p(b, a);
	//   ------------------------
	//
	//   e.g.[2] : int* m_var;
	//   =>{ int*& -> * -> int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int*& a = (int*&)m_var;
	//   int& b = *a;
	//   __int64 c;
	//   v_to_v(c, b);
	//   ------------------------
	//
	// ------------------------------------------------------------------------
	//
	// Same with 'Case : Constants'.
	//
	// Rule-3
	// Type of constant is "T"
	//
	//   [1] path={ T -> converter -> ... LuaType }
	//          =>{ T& -> converter -> ... LuaType }  Remove top T and insert T&
	//   [3] path={ T* -> converter ... -> LuaType }
	//          =>{ T& -> & -> T* -> converter -> ... LuaType }  Insert T&->& into top of path.
	//
	//   e.g.[1] : int m_var;
	//   =>{ int& -> v_to_v -> __int64 }
	//   ------------------------
	//   int& a = (int&)m_var;
	//   __int64 b;
	//   v_to_v(b, a);
	//   ------------------------
	//
	//   e.g.[3] : A m_var;
	//   =>{ A& -> & -> A* -> p_to_p -> classobj }
	//   ------------------------
	//   A& a = (A&)m_var;
	//   A* b = &a;
	//   classobj c;
	//   p_to_p(c, b);
	//   ------------------------
	//

	enum
	{
		PATH_NONE = 0,
		PATH_TYPE_1,		// PathType[1]
		PATH_TYPE_2,		// PathType[2]
		PATH_TYPE_3,		// PathType[3]
	} pathType = PATH_NONE;

	successPath.clear();

	std::string cTypeOfLua;

	if (!luaType.IsNull())
	{
		cTypeOfLua = LuaTypeToCTypeName(luaType);
	}

	// Type T above description. Remove last '&'.  e.g. "char*&" -> "char*"
	std::string cType_T = cType;
	UtilString::TrimRight(cType_T, '&');
	// Type S above description. Remove last '*'.  e.g. "char*" -> "char"
	const auto cType_S = (cType_T.back() == '*') ? cType_T.substr(0, cType_T.length() - 1) : "";

	// Get a conversion path.
	// e.g. successPath = { "", "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
	bool isSuccessPathUpdated = false;
	auto errorCode = MySearchC_LuaPath(
		true,				// Foreward search.
		cTypeOfLua,
		std::string(),		// First converter. Dummy.
		cType_T,			// First C++ type.
		Dummy<std::vector<std::string>>(),
		Dummy<std::unordered_set<std::string>>(),
		successPath,
		isSuccessPathUpdated
	);
	if (isSuccessPathUpdated)
	{
		pathType = PATH_TYPE_1;
	} else
	{
		LeError::ErrorCode tmpErrCode;
		if (cType_T.back() == '*')
		{
			isSuccessPathUpdated = false;
			tmpErrCode = MySearchC_LuaPath(
				true,				// Foreward search.
				cTypeOfLua,
				std::string(),		// First converter. Dummy.
				cType_S,			// First C++ type.
				Dummy<std::vector<std::string>>(),
				Dummy<std::unordered_set<std::string>>(),
				successPath,
				isSuccessPathUpdated
			);
			if (isSuccessPathUpdated)
				pathType = PATH_TYPE_2;
		}
		if (cType_T.back() != '*')
		{
			isSuccessPathUpdated = false;
			tmpErrCode = MySearchC_LuaPath(
				true,				// Foreward search.
				cTypeOfLua,
				std::string(),		// First converter. Dummy.
				cType_T + '*',	// First C++ type. e.g. "char" -> "char*"
				Dummy<std::vector<std::string>>(),
				Dummy<std::unordered_set<std::string>>(),
				successPath,
				isSuccessPathUpdated
			);
			if (isSuccessPathUpdated)
				pathType = PATH_TYPE_3;
		}
		if (errorCode == LeError::ErrorCode::NONE)
			errorCode = tmpErrCode;
	}
	if (pathType != PATH_NONE)
	{
		// Remove the first dummy converter.
		// e.g. successPath = { "", "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
		//                 -> { "type0", "converter1", "type1", "converter5", "type5", "converter6", "Ltyp6" }
		successPath.erase(successPath.begin());

		// Think about the following cases:converter is "-".
		//
		// [1] "::ClassX*" --["XptrToVoidPtr"]--> "void*" --["-"]--> "Luastick::lightuserdata"
		// [2] "lightuserdata" --["LuaPtrToVptr"]--> "voidptr" --["-"]--> "void*"
		//
		// In the case [1], eather seems to be fine to remove "void*" and "-" set, or "-" and "Luastick::lightuserdata" set.
		// But "Luastick::lightuserdata" is more important because in some case program have to find the Lua-type "lightuserdata" from "Luastick::lightuserdata".
		// So we should remove "void* and "-".
		// In the case [2], it is OK to remove eather "voidptr" and "-" set or "-" and "void*" set.
		// By the same reason with the above case, we should remove the farther from Lua-side:"-" and "void*" set.
		for (size_t i = 1; i != successPath.size();)
		{
			if (successPath[i] == "-")
				successPath.erase(successPath.begin() + i - 1, successPath.begin() + i + 1);
			else
				i += 2;
		}

		switch (cppToLuaConvType)
		{
		case CppToLuaConversionType::FUNC_OUT_ARGUMENT:	// Case : Output only arguments of function
			switch (cType.back())
			{
			case '&':	// Rule-1.
				switch (pathType)
				{
				case PATH_TYPE_1:	// PathType[1]
					successPath.insert(successPath.begin(), { "", "" });
					break;
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { "", "", cType_S + '*', "*", cType_S + '&' });
//					break;
//				default: // case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { "", "", cType_T, "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { "", "", topType + '*', "*", topType + '&' });
					break;
				}
				default: // case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { "", "", topType, "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				}
				break;
			case '*':	// Rule-2.
				switch (pathType)
				{
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_1:	// PathType[1]
//					successPath.insert(successPath.begin(), { cType_S, "&" });
//					break;
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S, "&", cType_S + '*', "*", cType_S + '&' });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_1:	// PathType[1]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType, "&" });
					break;
				}
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType, "&", topType + '*', "*", topType + '&' });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_3:	// PathType[3]
					ThrowLeSystemError();
				}
				break;
			default:	// Rule-3.
				switch (pathType)
				{
				case PATH_TYPE_1:	// PathType[1]
					successPath.insert(successPath.begin(), { "", "" });
					break;
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { "", "", cType_T, "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { "", "", topType, "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_2:	// PathType[2]
					ThrowLeSystemError();
				}
				break;
			}
			break;
		case CppToLuaConversionType::FUNC_INOUT_ARGUMENT:	// Input and output arguments of function
			switch (cType.back())
			{
			case '&':	// Rule-1.
				switch (pathType)
				{
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_1:	// PathType[1]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), cType_T + '&');
//					break;
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S + "*&", "*", cType_S + '&' });
//					break;
//				default: // case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { cType_T + '&', "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_1:	// PathType[1]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), topType + '&');
					break;
				}
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType + "*&", "*", topType + '&' });
					break;
				}
				default: // case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType + '&', "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				}
				break;
			case '*':	// Rule-2.
				switch (pathType)
				{
				case PATH_TYPE_1:	// PathType[1]
					break;
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S + '*', "*", cType_S + '&' });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType + '*', "*", topType + '&' });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_3:	// PathType[3]
					ThrowLeSystemError();
				}
				break;
			default:	// Rule-3.
				switch (pathType)
				{
				case PATH_TYPE_1:	// PathType[1]
					break;
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { cType_T, "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType, "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_2:	// PathType[2]
					ThrowLeSystemError();
				}
				break;
			}
			break;
		case CppToLuaConversionType::FUNC_RETURN:
		case CppToLuaConversionType::DEFINE_MACRO:
			switch (cType.back())
			{
			case '&':	// Rule-1.
				switch (pathType)
				{
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_1:	// PathType[1]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), cType_T + '&');
//					break;
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S + "*&", "*", cType_S + '&' });
//					break;
//				default: // case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { cType_T + '&', "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_1:	// PathType[1]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), topType + '&');
					break;
				}
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType + "*&", "*", topType + '&' });
					break;
				}
				default: // case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType + '&', "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				}
				break;
			case '*':	// Rule-2.
				switch (pathType)
				{
				case PATH_TYPE_1:	// PathType[1]
					break;
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S + '*', "*", cType_S + '&' });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType + '*', "*", topType + '&' });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_3:	// PathType[3]
					ThrowLeSystemError();
				}
				break;
			default:	// Rule-3.
				switch (pathType)
				{
				case PATH_TYPE_1:	// PathType[1]
					break;
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { cType_T, "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType, "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_2:	// PathType[2]
					ThrowLeSystemError();
				}
				break;
			}
			break;
		case CppToLuaConversionType::CONSTANT:
		case CppToLuaConversionType::VARIABLE:

			switch (cType.back())
			{
			case '&':	// Rule-1.
				switch (pathType)
				{
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_1:	// PathType[1]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), cType_T + '&');
//					break;
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S + "*&", "*", cType_S + '&' });
//					break;
//				default: // case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { cType_T + '&', "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_1:	// PathType[1]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), topType + '&');
					break;
				}
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType + "*&", "*", topType + '&' });
					break;
				}
				default: // case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType + '&', "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				}
				break;
			case '*':	// Rule-2.
				switch (pathType)
				{
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_1:	// PathType[1]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), cType_S + "*&");
//					break;
//				case PATH_TYPE_2:	// PathType[2]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), { cType_S + "*&", "*", cType_S + '&' });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_1:	// PathType[1]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), topType + "*&");
					break;
				}
				case PATH_TYPE_2:	// PathType[2]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), { topType + "*&", "*", topType + '&' });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_3:	// PathType[3]
					ThrowLeSystemError();
				}
				break;
			default:	// Rule-3.
				switch (pathType)
				{
//----- 20.02.20  変更前 ()-----
//				case PATH_TYPE_1:	// PathType[1]
//					successPath.erase(successPath.begin());
//					successPath.insert(successPath.begin(), cType_T + '&');
//					break;
//				case PATH_TYPE_3:	// PathType[3]
//					successPath.insert(successPath.begin(), { cType_T + '&', "&" });
//					break;
//----- 20.02.20  変更後 ()-----
				case PATH_TYPE_1:	// PathType[1]
				{
					auto topType = successPath.front();
					successPath.erase(successPath.begin());
					successPath.insert(successPath.begin(), topType + '&');
					break;
				}
				case PATH_TYPE_3:	// PathType[3]
				{
					auto topType = successPath.front().substr(0, successPath.front().length() - 1);
					successPath.insert(successPath.begin(), { topType + '&', "&" });
					break;
				}
//----- 20.02.20  変更終 ()-----
				default: // case PATH_TYPE_2:	// PathType[2]
					ThrowLeSystemError();
				}
				break;
			}
			break;
		}
	}
	else if (errorCode != LeError::ErrorCode::NONE)
	{
		ThrowLeException(errorCode, cType);
	}
	else
	{
		ThrowLeException(LeError::CONVERTER_UNDEFINED, cType);
	}
} // GetCppToLuaConverter.

static bool ParseClassDef(
	ClassRealType & classRealType,
	std::string & className,
	std::string & superClassName,
	const std::string & text
)
{
	classRealType = ClassRealType::NONE;
	className.clear();
	superClassName.clear();
	std::smatch results;
	if (std::regex_search(text, results, CLASS_PUBLIC_XML))			// Extracts 'CCC','DDD' from "class CCC : public DDD"
		classRealType = ClassRealType::CLASS;
	else if (std::regex_search(text, results, STRUCT_PUBLIC_XML))	// Extracts 'CCC','DDD' from "struct CCC : public DDD"
		classRealType = ClassRealType::STRUCT;
	if (classRealType != ClassRealType::NONE)
	{
		className = results[1].str();
		superClassName = results[2].str();
		return true;
	}
	if (std::regex_search(text, results, CLASS_XML))			// Extracts 'CCC' from "class CCC ~"
		classRealType = ClassRealType::CLASS;
	else if (std::regex_search(text, results, STRUCT_XML))		// Extracts 'CCC' from "struct CCC ~"
		classRealType = ClassRealType::STRUCT;
	else if (std::regex_search(text, results, UNION_XML))		// Extracts 'CCC' from "union CCC ~"
		classRealType = ClassRealType::UNION;
	else if (std::regex_search(text, results, NAMESPACE_XML))	// Extracts 'CCC' from "namespace CCC ~"
		classRealType = ClassRealType::NAMESPACE;
	if (classRealType != ClassRealType::NONE)
	{	//----- results[1] = CCC -----
		className = results[1].str();
		return true;
	}
	return false;
}

/// <summary>
/// e.g. arguments="int a, double &amp; b, const char* c);"<para/>
///     mpArgNameToIO={"a"->"in", "b"->"inout", "c"->"in"}<para/>
/// -> argNameToCtype={"a"->"int", "b"->"double", "c"->"char*"}<para/>
/// -> argNames={"a", "b", "c"}<para/>
/// -> inArgNames={"a", "b", "c"}<para/>
/// -> outArgNames={"b"}
/// </summary>
/// <param name="argNameToCtype">Type of the argument name to.</param>
/// <param name="argNames">The argument names.</param>
/// <param name="inArgNames">The in argument names.</param>
/// <param name="outArgNames">The out argument names.</param>
/// <param name="argumentsText">text of arguments.</param>
/// <param name="mpArgNameToIO">The mp argument name to io.</param>
static void ParseArgments(
	std::unordered_map<std::string, std::string> & argNameToCtype,
	std::unordered_map<std::string, std::string> & argNameToRawCtype,
	std::vector<std::string> & argNames,
	std::unordered_set<std::string> & inArgNames,
	std::unordered_set<std::string> & outArgNames,
	const std::string & argumentsText,
	const ClassRec & classRec,
	const std::unordered_map<std::string, std::string> & mpArgNameToIO
)
{
	std::smatch results;
	std::string argument;
	std::string defaultValue;
	const char* argText = argumentsText.c_str();
	for (;;)
	{
		// If argText = "double & v1, __int64 v2 = defaultX(10, 20));" then
		// 1st loop : argument = "double & v1", defaultValue = "", nextPtr = ", __int64 v2 = defaultX(10, 20));"
		// 2nd loop : argument = "__int64 v2", defaultValue = "defaultX(10, 20)", nextPtr = ");"
		int lineNumber = 0;
		auto nextPtr = UtilString::ReadOneArgument(argument, defaultValue, argText, L"", lineNumber);
		argText = nextPtr + 1;

		// Extract "std::wstring *&" and "argbBack" from "std::wstring *&argbBack"
		if (std::regex_search(argument, results, ARG_XML))
		{	//----- results[1] = "std::wstring *&", results[2] = "argbBack"
			// パラメーターの型の先頭の[const]を削除し、&、空白を削除する（*は残す）。例：[const CString&] -> [CString]
			std::string rawCtype;
			std::string varType;
			NormalizeVarType(varType, rawCtype, results[1].str(), classRec);

			if (varType == "Sticklib::classobject")
				ThrowLeException(LeError::NOT_SUPPORTED, "It is prohibited to use Sticklib::classobject as the type of argument.");

			const auto varName = results[2].str();
			// Argument name -> raw c++ Type.
			argNameToRawCtype[varName] = rawCtype;
			// Argument name -> regularized c++ Type.
			argNameToCtype[varName] = varType;
			// パラメーター名リストに追加。
			argNames.emplace_back(varName);
		}
		else
		{
			break;
		}

		if (*nextPtr == ')') break;
		if (*nextPtr != ',')
			ThrowLeException(LeError::UNEXPECTED_CHAR, std::string(nextPtr, 1));
	}

	for (const auto & argName : argNames)
	{	//----- パラメーターをループ -----
		// パラメーター名からパラメーターのin/outを取得。
		const auto io = mpArgNameToIO.find(argName);
		if (io == mpArgNameToIO.end())
			ThrowLeException(LeError::INVALID_VARIABLE_IO_DEFINED, argName);
		if (io->second == "in" || io->second == "inout")
			inArgNames.insert(argName);
		if (io->second == "out" || io->second == "inout")
			outArgNames.insert(argName);
	}
}

/// <summary>
/// Parses the function definition.<para/>
/// e.g. If text = "extern int MyFunc2(double &amp; v1, __int64 v2 = defaultX(10, 20));" then<para/>
///    -> classRealType = "extern"<para/>
///    -> returnRawCtype = "int"<para/>
///    -> fullpathFuncCname = "MyFunc2"<para/>
///    -> arguments = "double &amp; v1, __int64 v2 = defaultX(10, 20));"
/// </summary>
/// <param name="funcStrType">Type of the function. "extern"/"static"/"virtusl"/"regular"</param>
/// <param name="returnRawCtype">Type of the return.</param>
/// <param name="fullpathFuncCname">Name of the function.</param>
/// <param name="arguments">The arguments.</param>
/// <param name="text">The text.</param>
/// <param name="enumNameArray">The class name.</param>
/// <returns></returns>
static bool ParseFuncDef(
	std::string & funcStrType,
	std::string & returnCtype,
	std::string & funcCname,
	std::string & arguments,
	const std::string & text,
	const std::string & className
)
{
	std::smatch results;
	if (std::regex_search(text, results, FUNC_EXT_INL_XML))		// [extern inline TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出
		funcStrType = "extern";
	else if (std::regex_search(text, results, FUNC_EXT_XML))	// [extern TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPPを抽出
		funcStrType = "extern";
	else if (std::regex_search(text, results, FUNC_STC_INL_XML))	// [static inline TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPP)を抽出
		funcStrType = "static";
	else if (std::regex_search(text, results, FUNC_STC_XML))	// [static TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPPを抽出
		funcStrType = "static";
	else if (std::regex_search(text, results, FUNC_VIR_XML))	// [virtual TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPPを抽出
		funcStrType = "virtual";
	else if (std::regex_search(text, results, FUNC_INL_XML))	// [inline TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPPを抽出
		funcStrType = "regular";
	else if (std::regex_search(text, results, FUNC_REGULAR_XML))	// [TT:TT<T,T> *& FFF (PPP)]のTT:TT<T,T> *&,FFF,PPPを抽出
		funcStrType = "regular";
	else
		funcStrType.clear();
	if (!funcStrType.empty())
	{	//----- results[1] = TT:TT<T,T>, results[2] = FFF, results[3] = PPP -----
		returnCtype = (results[1].str() == "void") ? "" : results[1].str();
		funcCname = results[2].str();
		arguments = results[3].str();
		return true;
	}
	if (!className.empty())
	{
		// if enumNameArray="MyClass" then
		// R"(^MyClass\s*\((.+)$)"
		const std::regex CONSTRUCTOR_XML(std::string("^") + className + R"(\s*\((.+)$)");
		if (std::regex_search(text, results, CONSTRUCTOR_XML))
		{	//----- results[1] = arguments -----
			// when text="MyFunc(int a, int b);" results[1].str()="int a, int b);"
			funcStrType = "constructor";
			returnCtype = className + "*";
			funcCname = className;
			arguments = results[1].str();
			return true;
		}
	}
	return false;
} // ParseFuncDef.

/// <summary>
/// Make a c++ function exporting to lua from the extern function definition.
/// e.g. Make __funcname from the following.
/// ----------------------------------------------------------------
/// extern functype funcname(argtype1 argname1, argtype2 argname2)
/// ----------------------------------------------------------------
/// static int __funcname(lua_State* L)
/// {
/// 	if (lua_gettop(L) != 2)
/// 		luaL_error(L, "incorrect argument");
/// 	argtype1 __lstickvar_v1;
/// 	luaint_to_type1(__lstickvar_v1, luaL_checkinteger(L, 1));
/// 	argtype2 __lstickvar_v2;
/// 	luaint_to_type2(__lstickvar_v2, luaL_checkinteger(L, 2));
///		functype __lstickvar_ret;
///		try
///		{
/// 		__lstickvar_ret = funcname(__lstickvar_v1, __lstickvar_v2);
///		}
///		catch (MyException* e)
///		{
///			std::string __lstickvar_message = "C function error:funcname:";
///			__lstickvar_message += WStrToAStr(e->GetMessage()).get();
///			e->Delete();
///			luaL_error(L, __lstickvar_message.c_str());
///		}
///		catch (YourException e)
///		{
///			std::string __lstickvar_message = "C function error:funcname:";
///			__lstickvar_message += WStrToAStr(e.GetMessage()).get();
///			luaL_error(L, __lstickvar_message.c_str());
///		}
///		catch (...)
///		{
///			std::string __lstickvar_message = "C function error:funcname:";
///			luaL_error(L, __lstickvar_message.c_str());
///		}
/// 	lua_Integer __lstickvar_r1;
/// 	functype_to_luaint(__lstickvar_r1, __lstickvar_ret);
/// 	lua_pushinteger(L, __lstickvar_r1);
/// 	return 1;
/// }
/// ----------------------------------------------------------------
/// </summary>
/// <param name="funcTypeReturn">Type of the c++ function. e.g. STATIC, METHOD, CONSTRUCTOR</param>
/// <param name="funcNameReturn">The function name return.</param>
/// <param name="argNamesReturn">The argument names return.</param>
/// <param name="argNameToCtypeReturn">The argument name to Type return.</param>
/// <param name="argNameToRawCtypeReturn">The argument name to Raw Type return.</param>
/// <param name="inArgNamesReturn">The in argument names return.</param>
/// <param name="outArgNamesReturn">The out argument names return.</param>
/// <param name="inArgNameToLuaToCppConversionPathReturn"></param>
/// <param name="outArgNameToCppToLuaConversionPathReturn"></param>
/// <param name="currentClassRec">The current class record.</param>
/// <param name="text">The text.</param>
/// <param name="specifiedRecType">The specified Type.</param>
/// <param name="specifiedArgNameToIO">The specified argument name to io. e.g. {"a"->"in", "b"->"inout"}</param>
/// <returns>true:if text includes a function/false:not includes</returns>
static bool CheckFunction(
	FuncRec::Type & funcTypeReturn,
	std::string & funcNameReturn,
	std::vector<std::string> & argNamesReturn,
	std::unordered_map<std::string, std::string> & argNameToCtypeReturn,
	std::unordered_map<std::string, std::string> & argNameToRawCtypeReturn,
	std::unordered_set<std::string> & inArgNamesReturn,
	std::unordered_set<std::string> & outArgNamesReturn,
	std::unordered_map<std::string, std::vector<std::string>> & inArgNameToLuaToCppConversionPathReturn,
	std::unordered_map<std::string, std::vector<std::string>> & outArgNameToCppToLuaConversionPathReturn,
	const ClassRec & currentClassRec,
	const std::string & text,
	const std::string & specifiedRecType,
	const std::unordered_map<std::string, std::string> & specifiedArgNameToIO,
	const std::unordered_map<std::string, std::string> & specifiedArgNameToAliasCtype,
	const std::unordered_map<std::string, LuaType> & specifiedArgNameToAliasLtype
)
{
	//
	// void FuncA();   <--- parentClassPrefix="", currentClassRec.enumNameArray=""
	// class A {
	//   void FuncB();   <--- parentClassPrefix="::", currentClassRec.enumNameArray="A"
	//   class B {
	//     void FuncC();   <--- parentClassPrefix="::A::", currentClassRec.enumNameArray="B"
	//
	std::string funcStrType;
	std::string funcCname;
	std::string returnCtype;
	std::string arguments;
	if (ParseFuncDef(funcStrType, returnCtype, funcCname, arguments, text, currentClassRec.classCname))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "function")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);

		switch (currentClassRec.classType)
		{
		case ClassRec::Type::GLOBAL:
			if (funcStrType == "static" || funcStrType == "virtual")
				ThrowLeGenericError("Cannot register this type of function", funcStrType);
			break;
		case ClassRec::Type::CLASS:
			if (funcStrType == "extern")
				ThrowLeGenericError("Cannot register this type of function", funcStrType);
			break;
		case ClassRec::Type::INCONSTRUCTIBLE:
			if (funcStrType == "extern" || funcStrType == "constructor")
				ThrowLeGenericError("Cannot register this type of function", funcStrType);
			break;
		case ClassRec::Type::STRUCT:
			if (funcStrType != "static")
				ThrowLeGenericError("Only static member function is allowed to export for struct.", funcStrType);
			break;
		case ClassRec::Type::STATICCLASS:
			if (funcStrType == "extern" || funcStrType == "virtual" || funcStrType == "regular" || funcStrType == "constructor")
				ThrowLeGenericError("Cannot register this type of function", funcStrType);
			break;
		case ClassRec::Type::NAMESPACE:
			if (funcStrType == "virtual" || funcStrType == "constructor")
				ThrowLeGenericError("Cannot register this type of function", funcStrType);
			break;
		default:
			ThrowLeSystemError();
		}

		FuncRec::Type funcType;
		switch (currentClassRec.classType)
		{
		case ClassRec::Type::CLASS:
		case ClassRec::Type::INCONSTRUCTIBLE:
		case ClassRec::Type::STRUCT:		// Only funcStrType == "static" is allowed.
			if (funcStrType == "static")
				funcType = FuncRec::Type::STATIC;
			else if (funcStrType == "virtual" || funcStrType == "regular")
				funcType = FuncRec::Type::METHOD;
			else if (funcStrType == "constructor")
				funcType = FuncRec::Type::CONSTRUCTOR;
			break;
		case ClassRec::Type::GLOBAL:
		case ClassRec::Type::STATICCLASS:
		case ClassRec::Type::NAMESPACE:
			funcType = FuncRec::Type::STATIC;
			break;
		default:
			ThrowLeSystemError();
		}

		// Extracts each argument name and Type from arguments string.
		std::unordered_map<std::string, std::string> argNameToCtype;
		// Argument name -> raw c++ Type.
		std::unordered_map<std::string, std::string> argNameToRawCtype;

		std::vector<std::string> argNames;
		std::unordered_set<std::string> inArgNames;
		std::unordered_set<std::string> outArgNames;
		ParseArgments(
			argNameToCtype,
			argNameToRawCtype,
			argNames,
			inArgNames,
			outArgNames,
			arguments,
			currentClassRec,
			specifiedArgNameToIO
		);
		if (!returnCtype.empty())
		{
			// returnCtype=戻りの型。戻りの型の先頭の[const]を削除し、空白を削除する（&,*は残す）。例：[const CString*&] -> [CString*&]
			std::string rawCtype;
			NormalizeVarType(returnCtype, rawCtype, returnCtype, currentClassRec);
			argNameToCtype["__lstickvar_ret"] = returnCtype;
			// Argument name -> raw c++ Type.
			argNameToRawCtype["__lstickvar_ret"] = rawCtype;
			outArgNames.insert("__lstickvar_ret");
		}

		// Copies aliasCtype to argNameToCtype.
		for (const auto & argName_Ctype : argNameToCtype)
		{
			auto i = specifiedArgNameToAliasCtype.find(argName_Ctype.first);
			if (i != specifiedArgNameToAliasCtype.end())
				argNameToCtype[argName_Ctype.first] = i->second;
		}

		for (const auto & argName_ctype : argNameToCtype)
		{
			const auto & argName = argName_ctype.first;
			auto varCtype = argName_ctype.second;
			std::vector<std::string> successPath;
			if (inArgNames.find(argName) != inArgNames.end())
			{
				auto iAliasLtype = specifiedArgNameToAliasLtype.find(argName);
				LuaType luaType = (iAliasLtype == specifiedArgNameToAliasLtype.end()) ? LuaType::NIL : iAliasLtype->second;

				GetLuaToCppConverter(luaType, varCtype, successPath);
				inArgNameToLuaToCppConversionPathReturn[argName] = successPath;
			}
			if (outArgNames.find(argName) != outArgNames.end())
			{
				CppToLuaConversionType cppToLuaConvType;
				if (!successPath.empty())
					cppToLuaConvType = CppToLuaConversionType::FUNC_INOUT_ARGUMENT;
				else if (argName == "__lstickvar_ret")
					cppToLuaConvType = CppToLuaConversionType::FUNC_RETURN;
				else
					cppToLuaConvType = CppToLuaConversionType::FUNC_OUT_ARGUMENT;

				LuaType luaType;
				if (!successPath.empty())
				{	//----- If io="inout" for argName, and Lua-to-C++ conversion path exists -----
					// If Lua-to-C++ conversion path exists, its first item must be the last item of C++-to-Lua conversion path and its last item must be the first item.
					varCtype = successPath.back();
					luaType = CtypeToLuaType(successPath.front());
				}
				else if (funcType == FuncRec::Type::CONSTRUCTOR && argName == "__lstickvar_ret")
				{	//----- If the function is constructor and argName is its return value, specifies destination Lua-type -----

// テスト。そもそもこのブロックは不要では。
//					luaType = LuaType("classobject");
					luaType = LuaType::NIL;
				}
				else
				{
					auto iAliasLtype = specifiedArgNameToAliasLtype.find(argName);
					luaType = (iAliasLtype == specifiedArgNameToAliasLtype.end()) ? LuaType::NIL : iAliasLtype->second;
				}

				std::vector<std::string> successPath;
				GetCppToLuaConverter(
					cppToLuaConvType,
					varCtype,
					luaType,
					successPath
				);
				outArgNameToCppToLuaConversionPathReturn[argName] = successPath;
			}
		}

		if (funcType == FuncRec::Type::CONSTRUCTOR)
			funcCname = "New";

		funcTypeReturn = funcType;
		funcNameReturn = funcCname;
		argNamesReturn = argNames;
		argNameToCtypeReturn = argNameToCtype;
		argNameToRawCtypeReturn = argNameToRawCtype;
		inArgNamesReturn = inArgNames;
		outArgNamesReturn = outArgNames;
		return true;
	}
	else
	{
		return false;
	}
}

/// <summary>
/// Outputs the wrapper function.
/// </summary>
/// <param name="luaArgCount">Count of arguments pushed on the Lua stack.</param>
/// <param name="funcRec">The function record.</param>
static void OutputFuncWrapper(
	int luaArgCount,
	const FuncRec & funcRec
)
{
	// Typicial Wrapper function to export C++ function to Lua.
	// +------------------------------------------------------------------------------------------------+
	// | // Generated at "c:\src\luastick\luastick\luastick.cpp"(3463)                                  |
	// | /// <summary>                                                                                  |
	// | /// ::X::A::Get()                                                                              |
	// | /// </summary>                                                                                 |
	// | static int lm__X__A__Get__1(lua_State* L)                                                      |
	// | {                                                                                              |
	// | 	// Generated at "c:\src\luastick\luastick\luastick.cpp"(3495)                               |
	// | 	try                                                                                         |
	// | 	{                                                                                           |
	// | 		// Check the count of arguments.                                                        |
	// | 		if (lua_gettop(L) != 1)                                                                 |
	// | 			throw std::invalid_argument("Count of arguments is not correct.");                  |
	// | 		// Generated at "c:\src\luastick\luastick\luastick.cpp"(3610)                           |
	// | 		// Get the class object.                                                                |
	// | 		::X::A * __lstickobj;                                                                   |
	// | 		Sticklib::check_classobject((Sticklib::classobject &)__lstickobj, L, 1);                |
	// | 		// Generated at "c:\src\luastick\luastick\luastick.cpp"(3763)                           |
	// | 		// Call the c++ function.                                                               |
	// | 		auto __lstickvar_ret = (int)__lstickobj->Get();                                         |
	// | 		__int64 _argout_0_1;                                                                    |
	// | 		Sticklib::T_to_U(_argout_0_1, __lstickvar_ret);                                         |
	// | 		// Generated at "c:\src\luastick\luastick\luastick.cpp"(3933)                           |
	// | 		Sticklib::push_lvalue(L, _argout_0_1);                                                  |
	// | 	}                                                                                           |
	// | 	catch (std::exception & e)                                                                  |
	// | 	{                                                                                           |
	// | 		// Generated at "c:\src\luastick\luastick\luastick.cpp"(4016)                           |
	// | 		luaL_error(L, (std::string("C function error:::X::A::Get:") + e.what()).c_str());       |
	// | 	}                                                                                           |
	// | 	catch (...)                                                                                 |
	// | 	{                                                                                           |
	// | 		luaL_error(L, "C function error:::X::A::Get");                                          |
	// | 	}                                                                                           |
	// | 	return 1;                                                                                   |
	// | }                                                                                              |
	// +------------------------------------------------------------------------------------------------+

	auto argNameToCtype = funcRec.argNameToCtype;
	auto argNames = funcRec.argNames;

	// When the following, funcCname="MyFunc" and requiredLuaname="FuncX".
	// -------------------
	// <stick export="true" lname="FuncX"/>
	// int MyFunc();

	// lua argument number. 1,2,3...
	int luaInArgNumber = 1;
	// lua output argument number. 1,2,3...
	int luaOutArgNumber = 1;

	// Lua function argument count. If function is class method, one number must be added : self-class object.
	// e.g. Lua side : X:Get()  -->  C++ wrapper side : void lm__X__Get__1(X* x) { x->Get(); }

// 21.05.23 Fukushiro M. 1行削除 ()
//	const int luaArgCount = (funcRec.type == FuncRec::Type::METHOD) ? (int)funcRec.inArgNames.size() + 1 : (int)funcRec.inArgNames.size();

	const ClassRec & currentClassRec = ClassRec::Get(funcRec.GetParentClassId());

	// fullpath function name. ex. "::ClassA::ClassB::ClassC::MyFunc".
	const std::string fullpathFunCname = currentClassRec.GetFullpathCname() + "::" + funcRec.funcCname;
	// Name of the c++ function exporting to lua. e.g. __A__MyFunc2__2
	const std::string wrapperFunctionName = funcRec.GetWrapperFunctionName();

	// Outputs the comment, like the following.
	// ------------------------------------------------------
	// /// <summary>
	// /// ::X::B::Add1(::X::A a)
	// /// </summary>
	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
/// <summary>
/// ${currentClassRec.GetFullpathCname()}::${funcRec.funcCname}(
)", currentClassRec.GetFullpathCname(), funcRec.funcCname);

	for (const auto & argName : argNames)
	{
		if (argName != argNames.front())
			OUTPUT_EXPORTFUNC_STREAM << ", ";
		OUTPUT_EXPORTFUNC_STREAM << argNameToCtype.at(argName) << " " << argName;
	}
	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
)
/// </summary>

)");

	// Outputs like the following.
	// ------------------------------------------------------
	// static int lm__X__B__Add1__1(lua_State* L)
	// {
	//     try
	//     {
	//         if (lua_gettop(L) != 2)
	//             throw std::invalid_argument("Count of arguments is not correct.");
	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
static int ${wrapperFunctionName}(lua_State* L)
{
	// ${SRCMARKER}
	try
	{
		// Check the count of arguments.
		if (lua_gettop(L) != ${luaArgCount})
			throw std::invalid_argument("Count of arguments is not correct.");

)", wrapperFunctionName, luaArgCount);

	// e.g. Member function.
	// +------------------------------------------------------------------------------------+
	// | class ClassA {                                                                     |
	// |     // a:in, b:inout, c:out                                                        |
	// |     const std::wstring* FuncX(std::vector<int> * a, std::wstring & b, double * c)  |
	// | }                                                                                  |
	// +------------------------------------------------------------------------------------+
	//                                   |
	//                                   V
	// +-------------------------------------------------------------------------------------+
	// |                                                                          -+         |
	// | ::ClassA* __lstickobj;                                                    | Block-1 |
	// | Sticklib::check_classobject((Sticklib::classobject&)__lstickobj, L, 1);   |         |
	// |                                                                          -+         |
	// | std::vector<__int64> _argin_1_1;                                          | Block-2 |
	// | Sticklib::check_lvalue(_argin_1_1, L, 2);                                 |         |
	// |                                                                          -+         |
	// | std::vector<__int32> _argin_1_2;                                          | Block-3 |
	// | Sticklib::vector_to_vector(_argin_1_2, _argin_1_1);                       |         |
	// |                                                                          -+         |
	// | std::vector<__int32>* a = &_argin_1_2;                                    | Block-4 |
	// |                                                                          -+         |
	// | std::string _argin_2_1;                                                   | Block-2 |
	// | Sticklib::check_lvalue(_argin_2_1, L, 3);                                 |         |
	// |                                                                          -+         |
	// | std::wstring b;                                                           | Block-3 |
	// | Sticklib::astring_to_wstring(b, _argin_2_1);                              |         |
	// |                                                                          -+         |
	// | double _argin_3_1;                                                        | Block-5 |
	// | double* c = &_argin_3_1;                                                  |         |
	// |                                                                          -+         |
	// | std::wstring* __lstickvar_ret = (std::wstring*)FuncX(a, b, c);            | Block-6 |
	// |                                                                          -+         |
	// | std::wstring& _argout0_1 = *__lstickvar_ret;                              |         |
	// | std::string _argout0_2;                                                   | Block-9 |
	// | Sticklib::wstring_to_astring(_argout0_2, _argout0_1);                     |         |
	// |                                                                          -+         |
	// | Sticklib::push_lvalue(_argout0_2, L);                                     | Block-10|
	// |                                                                          -+         |
	// | std::string _argout2_1;                                                   |         |
	// | Sticklib::wstring_to_astring(_argout2_1, b);                              | Block-9 |
	// |                                                                          -+         |
	// | Sticklib::push_lvalue(_argout2_1, L);                                     | Block-10|
	// |                                                                          -+         |
	// | double& _argout_3_1 = *c;                                                 |         |
	// | Sticklib::push_lvalue(L, _argout_3_1);                                    | Block-10|
	// |                                                                          -+         |
	// +-------------------------------------------------------------------------------------+

	// e.g. Constructor.
	// +-----------------------------------------------------------------------------+
	// | class ClassA {                                                              |
	// |     // a:in, b:inout, c:out                                                 |
	// |     ClassA(std::vector<int> * a, std::wstring & b, double * c)              |
	// | }                                                                           |
	// +-----------------------------------------------------------------------------+
	//                                   |
	//                                   V
	// +---------------------------------------------------------------------------------+
	// |                                                                      -+         |
	// | std::vector<__int64> _argin_1_1;                                      | Block-2 |
	// | Sticklib::check_lvalue(_argin_1_1, L, 2);                             |         |
	// |                                                                      -+         |
	// | std::vector<__int32> _argin_1_2;                                      | Block-3 |
	// | Sticklib::vector_to_vector(_argin_1_2, _argin_1_1);                   |         |
	// |                                                                      -+         |
	// | std::vector<__int32>* a = &_argin_1_2;                                | Block-4 |
	// |                                                                      -+         |
	// | std::string _argin_2_1;                                               | Block-2 |
	// | Sticklib::check_lvalue(_argin_2_1, L, 3);                             |         |
	// |                                                                      -+         |
	// | std::wstring b;                                                       | Block-3 |
	// | Sticklib::astring_to_wstring(b, _argin_2_1);                          |         |
	// |                                                                      -+         |
	// | double _argin_3_1;                                                    | Block-5 |
	// | double* c = &_argin_3_1;                                              |         |
	// |                                                                      -+         |
	// | auto obj = new ::ClassA(a, b, c);                                     | Block-7 |
	// |                                                                      -+         |
	// | Sticklib::push_classobject(L, true, obj, "lm__X__A__");               | Block-8 |
	// |                                                                      -+         |
	// | std::string _argout2_1;                                               |         |
	// | Sticklib::wstring_to_astring(_argout2_1, b);                          | Block-9 |
	// |                                                                      -+         |
	// | Sticklib::push_lvalue(_argout2_1, L);                                 | Block-10|
	// |                                                                      -+         |
	// | double& _argout_3_1 = *c;                                             |         |
	// | Sticklib::push_lvalue(L, _argout_3_1);                                | Block-10|
	// |                                                                      -+         |
	// +---------------------------------------------------------------------------------+


	if (funcRec.type == FuncRec::Type::METHOD)
	{
		// Output above Block-1.

//----- 21.05.24 Fukushiro M. 変更前 ()-----
//		// Func name is "Sticklib::check_lvalue"
//		const auto luaValueGetFuncName = LuaTypeToGetFuncName(LuaType("classobject"));
//		// C++ type name is "Manglib::classobject"
//		const auto luaCTypeName = LuaTypeToCTypeName(LuaType("classobject"));
//----- 21.05.24 Fukushiro M. 変更後 ()-----
// テスト。
		auto classLuaType = UtilString::Format("classobject(%s)", currentClassRec.GetFullpathCname().c_str());
		// Func name is "Sticklib::check_lvalue"
		const auto luaValueGetFuncName = LuaTypeToGetFuncName(classLuaType);
//----- 21.05.24 Fukushiro M. 変更終 ()-----

		// Outputs like the following.
		// ------------------------------------------------------
		// // Generated at "c:\src\luastick\luastick\luastick.cpp"(2222)
		// // Get the class object.
		// ::CStickTestDlg * __lstickobj = (::CStickTestDlg *)Sticklib::checklightuserdata(L, 1);
		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		// Get the class object.
		${currentClassRec.GetFullpathCname()} * __lstickobj;
		${luaValueGetFuncName}(__lstickobj, L, ${luaInArgNumber});

)", currentClassRec.GetFullpathCname(), luaValueGetFuncName, luaInArgNumber);
		luaInArgNumber++;
	}

	int argMajorNumber = 0;
	for (const auto & argName : argNames)
	{	//----- loop every argument -----
		argMajorNumber++;
		if (funcRec.inArgNameToLuaToCppConversionPath.find(argName) != funcRec.inArgNameToLuaToCppConversionPath.end())
		{	//----- if argName is for input -----
			// Input argument is expanded like following example.
			//   conversionPath={ __int64 -> v_to_v -> int -> & -> int* }
			//   ------------------------
			//   __int64 _argin_1_1;
			//   get_lvalue(_argin_1_1);
			//   int _argin_1_2;
			//   v_to_v(_argin_1_2, _argin_1_1);
			//   int* c = &_argin_1_2;
			//   Func(c);
			//   ------------------------
			//
			//   conversionPath={ classobj -> p_to_p -> A* -> * -> A& }
			//   ------------------------
			//   classobj _argin_1_1;
			//   get_lvalue(_argin_1_1);
			//   A* _argin_1_2;
			//   p_to_p(_argin_1_2, _argin_1_1);
			//   A& c = *_argin_1_2;
			//   Func(c);
			//   ------------------------

			const auto & conversionPath = funcRec.inArgNameToLuaToCppConversionPath.at(argName);
			int argMinorNumber = 1;
			std::string tmpArgName;
			{
				// Output above Block-2.

				auto varCtype = conversionPath.front();
				auto getLuaArgFunc = LuaTypeToGetFuncName(funcRec.ArgNameToLuaType(argName));
				tmpArgName = (conversionPath.size() == 1) ? argName : UtilString::Format("_argin_%d_%d", argMajorNumber, argMinorNumber);

				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${tmpArgName};
		${getLuaArgFunc}(${tmpArgName}, L, ${luaInArgNumber});

)", varCtype, tmpArgName, getLuaArgFunc, luaInArgNumber);
				luaInArgNumber++;
				argMinorNumber++;
			}
			for (size_t i = 1; i != conversionPath.size(); i += 2)
			{
				const auto & convFunc = conversionPath.at(i);		// Converter.
				const auto & varCtype = conversionPath.at(i + 1);	// Type of variable.
				auto nextTmpArgName = (i + 2 == conversionPath.size()) ? argName : UtilString::Format("_argin_%d_%d", argMajorNumber, argMinorNumber);
				if (convFunc == "*" || convFunc == "&")
				{
					// Output above Block-4.

					OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		${varCtype} ${nextTmpArgName} = ${convFunc}${tmpArgName};

)", varCtype, nextTmpArgName, convFunc, tmpArgName);
				}
				else
				{
					// Output above Block-3.

					OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		${varCtype} ${nextTmpArgName};
		${convFunc}(${nextTmpArgName}, ${tmpArgName});

)", varCtype, nextTmpArgName, convFunc, tmpArgName);
				}
				tmpArgName = nextTmpArgName;
				argMinorNumber++;
			}
		}
		else
		{	//----- if argName is not for input. For output only. -----
			const auto & conversionPath = funcRec.outArgNameToCppToLuaConversionPath.at(argName);
			{
				// Output above Block-5.

				// First 3 items in the conversionPath are used before calling function.
				// e.g.
				// conversionPath={ A -> & -> A* -> p_to_p -> classobj }
				// ------------------------
				// A _argin_1_1;
				// A* a = &_argin_1_1;
				// Func(a);
				//
				// e.g.
				// conversionPath={ "" -> "" -> A* -> p_to_p -> classobj }
				// ------------------------
				// A* a;
				// Func(a);
				//  :
				if (!conversionPath.front().empty())
				{
					// e.g.
					// conversionPath={ A -> & -> A* -> p_to_p -> classobj }
					auto argType1 = conversionPath[0];
					auto starOrAmp = conversionPath[1];
					auto argType2 = conversionPath[2];
					auto tmpArgName = UtilString::Format("_argin_%d_1", argMajorNumber);
					OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${argType1} ${tmpArgName};
		${argType2} ${argName} = ${starOrAmp}${tmpArgName};

)", argType1, tmpArgName, argType2, argName, starOrAmp);
				}
				else
				{
					// e.g.
					// conversionPath={ "" -> "" -> A* -> p_to_p -> classobj }
					auto argType2 = conversionPath[2];
					OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${argType2} ${argName};

)", argType2, argName);
				}
			}
		}
	}

	if (funcRec.type == FuncRec::Type::METHOD)
	{	//----- if regular class -----
		auto i = funcRec.outArgNameToCppToLuaConversionPath.find("__lstickvar_ret");
		if (i == funcRec.outArgNameToCppToLuaConversionPath.end())
		{	//---- if the function does not return the value -----
			// Output above Block-6.
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		// Call the c++ function.
		if (__lstickobj == nullptr)
			throw std::invalid_argument("Null pointer is specified as class object.");
		__lstickobj->${funcRec.funcCname}(
)", funcRec.funcCname);
		}
		else
		{	//---- if the function returns the value -----
			// Output above Block-6.

			const auto & conversionPath = i->second;
			// conversionPath= { int -> v_to_v -> __int64 }
			// ------------------------
			// int a = Func();

			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		// Call the c++ function.
		if (__lstickobj == nullptr)
			throw std::invalid_argument("Null pointer is specified as class object.");
		auto __lstickvar_ret = (${conversionPath.front()})__lstickobj->${funcRec.funcCname}(
)", conversionPath.front(), funcRec.funcCname);
		}
	}
	else if (funcRec.type == FuncRec::Type::STATIC)
	{	//----- if static class or namespace -----
		auto i = funcRec.outArgNameToCppToLuaConversionPath.find("__lstickvar_ret");
		if (i == funcRec.outArgNameToCppToLuaConversionPath.end())
		{	//---- if the function does not return the value -----
			// Output above Block-6.
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		// Call the c++ function.
		${fullpathFunCname}(
)", fullpathFunCname);
		}
		else
		{	//---- if the function returns the value -----
			// Output above Block-6.

			const auto & conversionPath = i->second;
			// conversionPath= { int -> v_to_v -> __int64 }
			// ------------------------
			// int a = Func();
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		// Call the c++ function.
		auto __lstickvar_ret = (${conversionPath.front()})${fullpathFunCname}(
)", conversionPath.front(), fullpathFunCname);
		}
	}
	else
	{	//----- when constructor -----
		// Output above Block-7.
		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		// Create the class object and give back it to lua.
		auto obj = new ${currentClassRec.GetFullpathCname()}(
)", currentClassRec.GetFullpathCname());
	}

	// Output above Block-6, Block-7. Output function's arguments.
	for (const auto & argName : argNames)
	{	//----- loop every argument -----
		const auto & rawCtype = funcRec.argNameToRawCtype.at(argName);
		if (argName != argNames.front())
			OUTPUT_EXPORTFUNC_STREAM << ", ";
		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
(${rawCtype})${argName}
)", rawCtype, argName);
	}
	OUTPUT_EXPORTFUNC_STREAM << u8");\n";


	if (funcRec.type == FuncRec::Type::CONSTRUCTOR)
	{	//----- When constructor -----
//----- 21.05.24 Fukushiro M. 変更前 ()-----
//		const auto pushFunc = LuaTypeToSetFuncName(LuaType("classobject"));
//----- 21.05.24 Fukushiro M. 変更後 ()-----
		const auto pushFunc = LuaTypeToSetFuncName(LuaType(UtilString::Format("classobject(%s)", currentClassRec.GetFullpathCname().c_str())));
//----- 21.05.24 Fukushiro M. 変更終 ()-----
		// Output above Block-8.
		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${pushFunc}(L, true, obj, "${currentClassRec.GetUniqueClassName()}");

)", pushFunc, currentClassRec.GetUniqueClassName());
	}

	//----- Describes the source codes for giving back the arguments to lua -----

	if (
		funcRec.outArgNameToCppToLuaConversionPath.find("__lstickvar_ret") != funcRec.outArgNameToCppToLuaConversionPath.end() &&
		funcRec.type != FuncRec::Type::CONSTRUCTOR
		)
	{	//----- if c function has the return value -----
		// insert the return variable in front of the arguments list -----
		argNames.emplace(argNames.begin(), "__lstickvar_ret");
	}
	else
	{
		// Insert dummy.
		argNames.emplace(argNames.begin(), "");
	}

	argMajorNumber = -1;
	for (const auto & argName : argNames)
	{
		argMajorNumber++;
		if (funcRec.outArgNameToCppToLuaConversionPath.find(argName) == funcRec.outArgNameToCppToLuaConversionPath.end()) continue;
		const auto & conversionPath = funcRec.outArgNameToCppToLuaConversionPath.at(argName);
		int argMinorNumber = 1;

		// Start position of conversion-path. If argName is output only argument, the path starts from 3, and if return value or input-output argument, the path starts from 1.
		const size_t convPathStart =
			(argName == "__lstickvar_ret" || funcRec.inArgNameToLuaToCppConversionPath.find(argName) != funcRec.inArgNameToLuaToCppConversionPath.end()) ?
			1 : 3;

		// Case : Return value of function
		//   conversionPath={ A*& -> * -> A& -> A_to_str -> std::string }
		//   ------------------------
		//   A*& a = (A*&)Func();
		//   A& b = *a;             <--- output from here
		//   std::string c;
		//   A_to_str(c, b);
		//   ------------------------

		// Case : Input and output arguments of function
		//   conversionPath={ A*& -> * -> A& -> A_to_str -> std::string }
		//   ------------------------
		//   A*& a = ???;
		//   Func(a);
		//   A& b = *a;               <--- output from here
		//   std::string c;
		//   A_to_str(c, b);
		//   ------------------------

		// Case : Output only arguments of function
		//   conversionPath={ int -> & -> int* -> * -> int& -> v_to_v -> __int64 }
		//   ------------------------
		//   int x;
		//   int* a = &x;
		//   Func(a);
		//   int& b = *a;          <--- output from here
		//   __int64 c;
		//   v_to_v(c, b);
		//   ------------------------

		// Output above Block-9.

		auto tmpArgName = argName;
		for (size_t i = convPathStart; i != conversionPath.size(); i += 2)
		{
			const auto & convFunc = conversionPath.at(i);
			const auto & varCtype = conversionPath.at(i + 1);
			auto nextTmpArgName = UtilString::Format("_argout_%d_%d", argMajorNumber, argMinorNumber);
			if (convFunc == "*" || convFunc == "&")
			{
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		${varCtype} ${nextTmpArgName} = ${convFunc}${tmpArgName};

)", varCtype, nextTmpArgName, convFunc, tmpArgName);
			}
			else
			{
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		${varCtype} ${nextTmpArgName};
		${convFunc}(${nextTmpArgName}, ${tmpArgName});

)", varCtype, nextTmpArgName, convFunc, tmpArgName);
			}
			tmpArgName = nextTmpArgName;
			argMinorNumber++;
		}

		if (funcRec.ArgNameToLuaType(argName) == LuaType("classobject"))
		{	//----- if class object -----
			// Output above Block-10.
			std::string uniqueName = funcRec.argNameToCtype.at(argName);
			UtilString::TrimRight(uniqueName, '&', '*');
			uniqueName = ClassRec::FullpathNameToUniqueName(uniqueName);
			const auto pushFunc = LuaTypeToSetFuncName(funcRec.ArgNameToLuaType(argName));
//----- 21.05.19 Fukushiro M. 変更前 ()-----
//			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
//		// ${SRCMARKER}
//		${pushFunc}(L, false, ${tmpArgName}, "${uniqueName}");
//
//)", pushFunc, tmpArgName, uniqueName);
//----- 21.05.19 Fukushiro M. 変更後 ()-----
			const std::string autoDel = (funcRec.autoDelArgNames.find(argName) != funcRec.autoDelArgNames.end()) ? "true" : "false";
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${pushFunc}(L, ${autoDel}, ${tmpArgName}, "${uniqueName}");

)", pushFunc, autoDel, tmpArgName, uniqueName);
//----- 21.05.19 Fukushiro M. 変更終 ()-----
		}
		else
		{	//----- if not class object -----

//----- 21.05.24 Fukushiro M. 追加始 ()-----
// テスト。
			auto argLuaType = funcRec.ArgNameToLuaType(argName);
			auto classobjLuaType = LuaType("classobject(");
			auto classobjaryLuaType = LuaType("array<classobject>(");
			if (argLuaType.CompareHead(classobjLuaType) == 0)
			{	//----- if class object array -----
				// e.g. argLuaType="classobject(ObjSet)", classobjLuaType="classobject("
				auto fullpathClassName = argLuaType.ToString().substr(classobjLuaType.ToString().length(), argLuaType.ToString().length() - classobjLuaType.ToString().length() - 1);
				auto uniqueName = ClassRec::FullpathNameToUniqueName(fullpathClassName);
				const auto pushFunc = LuaTypeToSetFuncName(argLuaType);
				const std::string autoDel = (funcRec.autoDelArgNames.find(argName) != funcRec.autoDelArgNames.end()) ? "true" : "false";
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${pushFunc}(L, ${autoDel}, ${tmpArgName}, "${uniqueName}");

)", pushFunc, autoDel, tmpArgName, uniqueName);
			}
			else
			if (argLuaType.CompareHead(classobjaryLuaType) == 0)
			{	//----- if class object array -----
				auto fullpathClassName = argLuaType.ToString().substr(classobjaryLuaType.ToString().length(), argLuaType.ToString().length() - classobjaryLuaType.ToString().length() - 1);
				auto uniqueName = ClassRec::FullpathNameToUniqueName(fullpathClassName);
				const auto pushFunc = LuaTypeToSetFuncName(argLuaType);
				const std::string autoDel = (funcRec.autoDelArgNames.find(argName) != funcRec.autoDelArgNames.end()) ? "true" : "false";
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${pushFunc}(L, ${autoDel}, ${tmpArgName}, "${uniqueName}");

)", pushFunc, autoDel, tmpArgName, uniqueName);
			}
			else
			{
//----- 21.05.24 Fukushiro M. 追加終 ()-----
				// Output above Block-10.
				const auto pushFunc = LuaTypeToSetFuncName(funcRec.ArgNameToLuaType(argName));
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${pushFunc}(L, ${tmpArgName});

)", pushFunc, tmpArgName);
// 21.05.24 Fukushiro M. 1行追加 ()
			}
		}
	}
	OUTPUT_EXPORTFUNC_STREAM << u8"	}\n";

	for (const auto & exception : funcRec.exceptions)
	{
		const auto & msgDlt = EXCEPTION_TO_MESSAGE_DELETE.find(exception);
		if (msgDlt != EXCEPTION_TO_MESSAGE_DELETE.end())
		{
			//		catch (MyException* e)
			//		{
			//			std::string message = "C function error:funcname:";
			//			message += WStrToAStr(e->GetMessage()).get();
			//			e->Delete();
			//			luaL_error(L, message.c_str());
			//		}

			const auto & msgGetFunc = std::get<0>(msgDlt->second);
			const auto & excDelFunc = std::get<1>(msgDlt->second);

			//		catch (MyException* e)
			//		{
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
	catch (${exception} e)
	{

)", exception);

			//			std::string message = "C function error:funcname:";
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		std::string message = "C function error:${fullpathFunCname}:";

)", fullpathFunCname);

			if (!msgGetFunc.empty())
			{
				//			message += WStrToAStr(e->GetMessage()).get();
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		message += ${msgGetFunc};

)", msgGetFunc);
			}
			if (!excDelFunc.empty())
			{
				//			e->Delete();
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${excDelFunc};

)", excDelFunc);
			}
			//			luaL_error(L, message.c_str());
			//		}
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		luaL_error(L, message.c_str());
	}

)");
		}
	}
	//		catch (...)
	//		{
	//			luaL_error(L, "C function error:funcname:");
	//		}
	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
	catch (std::exception & e)
	{
		// ${SRCMARKER}
		luaL_error(L, (std::string("C function error:${fullpathFunCname}:") + e.what()).c_str());
	}
	catch (...)
	{
		luaL_error(L, "C function error:${fullpathFunCname}");
	}
	return ${funcRec.outArgNameToCppToLuaConversionPath.size()};
}


)", fullpathFunCname, funcRec.outArgNameToCppToLuaConversionPath.size());
}

static void OutputFuncWrappers(const ClassRec & classRec)
{
	for (const auto & funcLuanameToFuncGroupId : { classRec.methodFuncLuanameToFuncGroupId, classRec.staticFuncLuanameToFuncGroupId })
	{
		for (const auto & nameFuncGroupId : funcLuanameToFuncGroupId)
		{
			const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
			for (const auto & argCountFuncId : funcGroupRec.argCountToFuncId)
			{
				const auto & funcRec = FuncRec::Get(argCountFuncId.second);
				OutputFuncWrapper(argCountFuncId.first, funcRec);
			}
		}
	}
	for (const auto & classId : classRec.memberClassIdArray)
		OutputFuncWrappers(ClassRec::Get(classId));
}

static bool CheckClass(
	ClassRec::Type & classType,
	std::string & className,
	std::string & superClassName,
	const std::string & text,
	const std::string & specifiedRecType
)
{
	ClassRealType classRealType;
	if (ParseClassDef(classRealType, className, superClassName, text))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "class" && specifiedRecType != "inconstructible" && specifiedRecType != "struct" && specifiedRecType != "namespace")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);

		classType = ClassRec::Type::NONE;
//----- 20.01.16 Fukushiro M. 変更前 ()-----
//		if (specifiedRecType.empty())
//		{			// depend on classRealType.
//			switch (classRealType)
//			{
//			case ClassRealType::CLASS:
//			case ClassRealType::STRUCT:
//			case ClassRealType::UNION:
//				classType = ClassRec::Type::CLASS;
//				break;
//			case ClassRealType::NAMESPACE:
//				classType = ClassRec::Type::NAMESPACE;
//				break;
//			default:	// STOP.
//				ThrowLeSystemError();
//			}
//		}
//		else if (specifiedRecType == "class")
//		{	// as class.
//			switch (classRealType)
//			{
//			case ClassRealType::CLASS:
//			case ClassRealType::STRUCT:
//			case ClassRealType::UNION:
//				classType = ClassRec::Type::CLASS;
//				break;
//			case ClassRealType::NAMESPACE:
//				ThrowLeGenericError("Type 'class' was specified for namespace.");
//			default:	// STOP.
//				ThrowLeSystemError();
//			}
//		}
//		else if (specifiedRecType == "inconstructible")
//		{	// as class.
//			switch (classRealType)
//			{
//			case ClassRealType::CLASS:
//			case ClassRealType::STRUCT:
//				classType = ClassRec::Type::INCONSTRUCTIBLE;
//				break;
//			case ClassRealType::UNION:
//				ThrowLeGenericError("Type 'inconstructible' was specified for union.");
//				break;
//			case ClassRealType::NAMESPACE:
//				ThrowLeGenericError("Type 'class' was specified for namespace.");
//			default:	// STOP.
//				ThrowLeSystemError();
//			}
//		}
//		else if (specifiedRecType == "namespace")
//		{	// as static class or namespace.
//			switch (classRealType)
//			{
//			case ClassRealType::CLASS:
//			case ClassRealType::STRUCT:
//				classType = ClassRec::Type::STATICCLASS;
//				break;
//			case ClassRealType::UNION:
//				ThrowLeException(LeError::WRONG_DEFINED_TAG, "Type 'namescape' was specified for union.");
//			case ClassRealType::NAMESPACE:
//				classType = ClassRec::Type::NAMESPACE;
//				break;
//			default:	// STOP.
//				ThrowLeSystemError();
//			}
//		}
//----- 20.01.16 Fukushiro M. 変更後 ()-----
		switch (classRealType)
		{
		case ClassRealType::CLASS:
			if (specifiedRecType.empty())
				classType = ClassRec::Type::CLASS;
			else if (specifiedRecType == "class")
				classType = ClassRec::Type::CLASS;
			else if (specifiedRecType == "inconstructible")
				classType = ClassRec::Type::INCONSTRUCTIBLE;
			else if (specifiedRecType == "struct")
				classType = ClassRec::Type::STRUCT;
			else if (specifiedRecType == "namespace")
				classType = ClassRec::Type::STATICCLASS;
			else
				ThrowLeGenericError("Unsupported type:", specifiedRecType.c_str());
			break;
		case ClassRealType::STRUCT:
			if (specifiedRecType.empty())
				classType = ClassRec::Type::STRUCT;
			else if (specifiedRecType == "class")
				classType = ClassRec::Type::CLASS;
			else if (specifiedRecType == "inconstructible")
				classType = ClassRec::Type::INCONSTRUCTIBLE;
			else if (specifiedRecType == "struct")
				classType = ClassRec::Type::STRUCT;
			else if (specifiedRecType == "namespace")
				classType = ClassRec::Type::STATICCLASS;
			else
				ThrowLeGenericError("Unsupported type:", specifiedRecType.c_str());
			break;
		case ClassRealType::UNION:
			ThrowLeGenericError("Cannot export 'union'.");
		case ClassRealType::NAMESPACE:
			if (specifiedRecType.empty())
			{
				classType = ClassRec::Type::NAMESPACE;
			}
			else if (specifiedRecType == "class")
			{
				ThrowLeGenericError("Cannot specify 'class' for namespace.");
			}
			else if (specifiedRecType == "inconstructible")
			{
				ThrowLeGenericError("Cannot specify 'inconstructible' for namespace.");
			}
			else if (specifiedRecType == "struct")
			{
				ThrowLeGenericError("Cannot specify 'struct' for namespace.");
			}
			else if (specifiedRecType == "namespace")
			{
				classType = ClassRec::Type::NAMESPACE;
			}
			else
			{
				ThrowLeGenericError("Unsupported type:", specifiedRecType.c_str());
			}
			break;
		default:	// STOP.
			ThrowLeSystemError();
		}
//----- 20.01.16 Fukushiro M. 変更終 ()-----
		return true;
	}
	else
	{
		return false;
	}
}

static bool ParseEnumDef(
	std::string & enumRealType,
	std::string & enumName,
	const std::string & text
)
{
	enumRealType.clear();
	enumName.clear();
	std::smatch results;
	if (std::regex_search(text, results, ENUM_NAME_XML))					// Extracts 'CCC' from "enum CCC "
		enumRealType = "regular";
	else if (std::regex_search(text, results, ENUM_NAME_TYPE_XML))			// Extracts 'CCC' and 'DDD' from "enum CCC : DDD "
		enumRealType = "regular";
	else if (std::regex_search(text, results, ENUM_CLASS_NAME_XML))			// Extracts 'CCC' from "enum class CCC "
		enumRealType = "class";
	else if (std::regex_search(text, results, ENUM_CLASS_NAME_TYPE_XML))	// Extracts 'CCC' and 'DDD' from "enum class CCC : DDD "
		enumRealType = "class";
	else if (std::regex_search(text, results, ENUM_STRUCT_NAME_XML))		// Extracts 'CCC' from "enum struct CCC "
		enumRealType = "class";
	else if (std::regex_search(text, results, ENUM_STRUCT_NAME_TYPE_XML))	// Extracts 'CCC' and 'DDD' from "enum struct CCC : DDD "
		enumRealType = "class";
	if (!enumRealType.empty())
	{
		enumName = results[1].str();
		return true;
	}
	if (std::regex_search(text, results, ENUM_XML))				// Check "enum {"
		enumRealType = "regular";
	else if (std::regex_search(text, results, ENUM_TYPE_XML))	// Extracts 'DDD' from "enum : DDD {"
		enumRealType = "regular";
	if (!enumRealType.empty())
		return true;
	return false;
}

static bool CheckEnum(
	EnumRec::Type & enumType,
	std::string & enumName,
	const std::string & text,
	const std::string & specifiedRecType
)
{
	std::string enumRealType;
	if (ParseEnumDef(enumRealType, enumName, text))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "enum")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);

		if (enumRealType == "regular")
			enumType = EnumRec::Type::REGULAR;
		else if (enumRealType == "class")
			enumType = EnumRec::Type::ENUM_CLASS;
		return true;
	}
	return false;
}

/// <summary>
/// Parses static constant definition. e.g. "static const int A=..."
/// </summary>
/// <param name="constantCtype">Returns C++-type. e.g. "static const char * A=..." -> "char *"</param>
/// <param name="constantName">Returns constant name. e.g. "static const int A=..." -> "A"</param>
/// <param name="text">Target text.</param>
/// <returns>true:text is static constant/false;isn't static constant</returns>
static bool ParseConstDef(
	std::string & constantCtype,
	std::string & constantName,
	const std::string & text
)
{
	// Extracts 'TTT','NNN' from "static const TTT NNN;"
	static const std::regex STATIC_CONST_XML(R"(^static\s+const\s+([a-zA-Z_][\w:\*\& ]*)\s+([a-zA-Z_]\w*)\s*;.*$)");

	constantCtype.clear();
	constantName.clear();
	std::smatch results;
	if (
		std::regex_search(text, results, STATIC_CONST_XML)			// Extracts 'TTT','NNN' from "static const TTT NNN;"
		)
	{
		constantCtype = results[1].str();
		constantName = results[2].str();
		return true;
	}
	return false;
}

/// <summary>
/// Parses constexpr definition. e.g. "constexpr int A=..."
/// </summary>
/// <param name="constantCtype">Returns C++-type. e.g. "constexpr const  char * A=..." -> "const  char *"</param>
/// <param name="constantName">Returns constant name. e.g. "constexpr int A=..." -> "A"</param>
/// <param name="text">Target text.</param>
/// <returns>true:text is constexpr/false;isn't constexpr</returns>
static bool ParseConstexprDef(
	std::string & constantCtype,
	std::string & constantName,
	const std::string & text
)
{
	// Extracts 'TTT','NNN' from "constexpr TTT NNN = VVV"
	static const std::regex CONSTEXPR_XML(R"(^constexpr\s+([a-zA-Z_][\w:\*\& ]*)\b\s+([a-zA-Z_]\w*)\s*=.*$)");
	// Extracts 'TTT','NNN' from "static constexpr TTT NNN = VVV"
	static const std::regex STATIC_CONSTEXPR_XML(R"(^static\s+constexpr\s+([a-zA-Z_][\w:\*\& ]*)\b\s+([a-zA-Z_]\w*)\s*=.*$)");

	constantCtype.clear();
	constantName.clear();
	std::smatch results;
	if (
		std::regex_search(text, results, CONSTEXPR_XML) ||		// Extracts 'TTT','NNN' from "constexpr TTT NNN = VVV"
		std::regex_search(text, results, STATIC_CONSTEXPR_XML)	// Extracts 'TTT','NNN' from "static constexpr TTT NNN = VVV"
		)
	{
		constantCtype = results[1].str();
		constantName = results[2].str();
		return true;
	}
	return false;
} // ParseConstexprDef.

static bool ParseDefineDef(
	std::string & defineName,
	const std::string & text
)
{
	// Extracts ''NNN' from "#define NNN VVV"
	static const std::regex DEFINE_XML(R"(^#define\s+([a-zA-Z_]\w*)\s+.*$)");

	defineName.clear();
	std::smatch results;
	if (
		std::regex_search(text, results, DEFINE_XML)		// Extracts ''NNN' from "#define NNN VVV"
		)
	{
		defineName = results[1].str();
		return true;
	}
	return false;
} // ParseDefineDef.

static bool CheckConstant(
	ConstantRec::Type & constantRecType,
	// std::string & constantCtype,
	std::string & constantRawCtype,
	std::string & constantName,
	std::vector<std::string> & cToLuaConversionPath,
	const ClassRec & classRec,
	const std::string & text,
	const std::string & specifiedRecType,
	const std::string & specifiedCtype,
	const LuaType & specifiedLuatype
)
{
	std::string constantCtype;
	constantRecType = ConstantRec::Type::NONE;
	if (ParseConstDef(constantCtype, constantName, text))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "constant")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);
		constantRecType = ConstantRec::Type::CONSTANT;
		// e.g. "const CString&" -> "CString"
		NormalizeVarType(constantCtype, constantRawCtype, constantCtype, classRec);
		std::string tmpCtype = constantCtype;
		if (!specifiedCtype.empty())
			NormalizeVarType(tmpCtype, Dummy<std::string>(), specifiedCtype, classRec);
		GetCppToLuaConverter(
			CppToLuaConversionType::CONSTANT,
			tmpCtype,
			specifiedLuatype.IsNull() ? LuaType::NIL : specifiedLuatype,
			cToLuaConversionPath
		);
	}
	else if (ParseConstexprDef(constantCtype, constantName, text))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "constant")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);
		constantRecType = ConstantRec::Type::CONSTEXPR;
		// e.g. "const CString&" -> "CString"
		NormalizeVarType(constantCtype, constantRawCtype, constantCtype, classRec);
		std::string tmpCtype = constantCtype;
		if (!specifiedCtype.empty())
			NormalizeVarType(tmpCtype, Dummy<std::string>(), specifiedCtype, classRec);
		GetCppToLuaConverter(
			CppToLuaConversionType::CONSTANT,
			tmpCtype,
			specifiedLuatype.IsNull() ? LuaType::NIL : specifiedLuatype,
			cToLuaConversionPath
		);
	}
	else if (ParseDefineDef(constantName, text))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "define")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);
		if (specifiedCtype.empty())
			ThrowLeException(LeError::WRONG_DEFINED_TAG, text);
		constantRecType = ConstantRec::Type::DEFINE;
		NormalizeVarType(constantCtype, constantRawCtype, specifiedCtype, classRec);
		GetCppToLuaConverter(
			CppToLuaConversionType::DEFINE_MACRO,
			constantCtype,
			specifiedLuatype.IsNull() ? LuaType::NIL : specifiedLuatype,
			cToLuaConversionPath
		);
		// <stick export ctype=ClassA>
		// #define A ClassA()
		// -> conversion path={ "ClassA*", "tptr_to_voidptr", "lightuserdata" }
		// ClassA* a = &A;   <-- Causes an error.
		//     :
		// So,
		// cToLuaConversionPath.front() must be same with constantCtype.
		if (constantCtype != cToLuaConversionPath.front())
			ThrowLeException(LeError::CONVERTER_UNDEFINED, text);

	}
	return (constantRecType != ConstantRec::Type::NONE);
} // CheckConstant.


/// <summary>
/// Parses variable definition. e.g. "int A;"
/// </summary>
/// <param name="variableCtype">Returns C++-type. e.g. "char * A;" -> "char *"</param>
/// <param name="variableName">Returns variable name. e.g. "int A;" -> "A"</param>
/// <param name="text">Target text.</param>
/// <returns>true:text is variable/false;isn't variable</returns>
static bool ParseVariableDef(
	std::string & variableCtype,
	std::string & variableName,
	const std::string & text
)
{
	// Extract "std::wstring *&" and "argbBack" from "std::wstring *&argbBack;  // hello."
	// e.g. "std::vector<int> & arg" -> "std::vector<int> & " , "arg"
	// e.g. "std::map<int, string> & arg" -> "std::map<int, string> & " , "arg"
	static const std::regex VARIABLE_XML(R"(^\s*([a-zA-Z_][\w:<>,\*\&\s]*)\b([a-zA-Z_]\w*)\s*;.*$)");

	variableCtype.clear();
	variableName.clear();
	std::smatch results;
	if (
		std::regex_search(text, results, VARIABLE_XML)
		)
	{
		variableCtype = results[1].str();
		UtilString::Trim(variableCtype);
		variableName = results[2].str();
		UtilString::Trim(variableName);
		return true;
	}
	return false;
}

static bool CheckVariable(
	std::string & variableRawCtype,
	std::string & variableName,
	std::vector<std::string> & cToLuaConversionPath,
	std::vector<std::string> & luaToCConversionPath,
	const ClassRec & classRec,
	const std::string & text,
	const std::string & specifiedRecType,
	const std::string & specifiedCtype,
	const LuaType & specifiedLuatype
)
{
	std::string variableCtype;
	if (ParseVariableDef(variableCtype, variableName, text))
	{
		if (!specifiedRecType.empty() && specifiedRecType != "variable")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, specifiedRecType);
		// e.g. "CString&" -> "CString" , "CString*&" -> "CString*"
		NormalizeVarType(variableCtype, variableRawCtype, variableCtype, classRec);
		std::string tmpCtype = variableCtype;
		if (!specifiedCtype.empty())
			NormalizeVarType(tmpCtype, Dummy<std::string>(), specifiedCtype, classRec);
		GetCppToLuaConverter(
			CppToLuaConversionType::VARIABLE,
			tmpCtype,
			specifiedLuatype.IsNull() ? LuaType::NIL : specifiedLuatype,
			cToLuaConversionPath
		);
		GetLuaToCppConverter(
			specifiedLuatype.IsNull() ? LuaType::NIL : specifiedLuatype,
			tmpCtype,
			luaToCConversionPath
		);
		return true;
	}
	return false;
} // CheckVariable.


/// <summary>
/// Handles the lua tag.
/// e.g. <stick export="true" type="namespace" />
/// </summary>
/// <param name="tag">The tag.</param>
/// <param name="isExport">true:following function must be exported/false:not.</param>
/// <param name="classStrType">Specified with "type" attributes.</param>
/// <param name="lname">Specified with "lname" attributes.</param>
/// <param name="ctype">Specified with "ctype" attributes.</param>
/// <param name="ltype">Specified with "ltype" attributes.</param>
static void HandleStickTag(
	const UtilXml::Tag & tag,
	bool & isExport,
	std::string & classStrType,
	std::string & lname,
	std::string & ctype,
	LuaType & ltype,
	std::string & specifiedSuper
)
{
	// get xxx from export="xxx"
	auto v = UtilMisc::FindMapValue(tag.attributes, "export");
//----- 20.06.15 Fukushiro M. 変更前 ()-----
//	if (v == "true")
//		isExport = true;
//	else if (v == "false")
//		isExport = false;
//	else
//		ThrowLeException(LeError::WRONG_DEFINED_TAG, tag.name);
//----- 20.06.15 Fukushiro M. 変更後 ()-----
	if (v == "true")
		isExport = true;
	else
		isExport = false;
//----- 20.06.15 Fukushiro M. 変更終 ()-----
	// get xxx from type="xxx"
	classStrType = UtilMisc::FindMapValue(tag.attributes, "type");
	// get xxx from lname="xxx"
	lname = UtilMisc::FindMapValue(tag.attributes, "lname");
	// get xxx from ctype="xxx"
	ctype = UtilMisc::FindMapValue(tag.attributes, "ctype");
	if (!ctype.empty())
		NormalizeVarType(ctype, Dummy<std::string>(), ctype, ClassRec());
	// get xxx from ltype="xxx"
	auto tmpltype = UtilMisc::FindMapValue(tag.attributes, "ltype");
	if (!tmpltype.empty())
	{
		NormalizeVarType(tmpltype, Dummy<std::string>(), tmpltype, ClassRec());
		ltype = LuaType(tmpltype);
	}
	// get xxx from super="xxx"
	specifiedSuper = UtilMisc::FindMapValue(tag.attributes, "super");
}

//----- 19.11.29 Fukushiro M. 追加始 ()-----
/// <summary>
/// Handles the luatype tag.
/// e.g. <sticktype name="boolean" ctype="bool" getfunc="Sticklib::check_lvalue" setfunc="Sticklib::push_lvalue" />
/// </summary>
/// <param name="tag">The tag.</param>
static void RegisterSticktypeTag(const UtilXml::Tag & tag)
{
	auto name = UtilMisc::FindMapValue(tag.attributes, "name");
	auto ctype = UtilMisc::FindMapValue(tag.attributes, "ctype");
	auto getfunc = UtilMisc::FindMapValue(tag.attributes, "getfunc");
	auto setfunc = UtilMisc::FindMapValue(tag.attributes, "setfunc");

	if (name.empty() || ctype.empty() || getfunc.empty() || setfunc.empty())
		ThrowLeException(LeError::WRONG_DEFINED_TAG, "Every attribute must be filled in.");

	// Remove space from name.
	UtilString::SimpleReplace(name, " ", "");

	NormalizeVarType(ctype, Dummy<std::string>(), ctype, ClassRec());

	const LuaType luaType(name);
	if (LTYPE_TO_REC.find(luaType) != LTYPE_TO_REC.end())
		ThrowLeException(LeError::REGISTER_SAME_NAME, "Cannot register the same name of lua-type twice.", name);

	LTYPE_TO_REC[luaType] = LuaTypeRec(ctype, getfunc, setfunc);
	CTYPE_TO_LUATYPE[ctype] = luaType;
}

/// <summary>
/// Handles the luaconv tag.
/// e.g. <stickconv type1="std::string" type2="std::wstring" type1to2="Sticklib::astring_to_wstring" type2to1="Sticklib::wstring_to_astring" />
/// </summary>
/// <param name="tag">The tag.</param>
/// <param name="isImporting">true:Called by importing source code/false:Called from another function</param>
static void RegisterStickconvTag(const UtilXml::Tag & tag, bool isImporting)
{
	auto type1 = UtilMisc::FindMapValue(tag.attributes, "type1");
	auto type2 = UtilMisc::FindMapValue(tag.attributes, "type2");
	auto type1to2 = UtilMisc::FindMapValue(tag.attributes, "type1to2");
	auto type2to1 = UtilMisc::FindMapValue(tag.attributes, "type2to1");

	if (type1.empty() || type2.empty())
		ThrowLeException(LeError::WRONG_DEFINED_TAG, "type1 and type2 both must be filled in.");
	if (type1to2.empty() && type2to1.empty())
		ThrowLeException(LeError::WRONG_DEFINED_TAG, "At least one of type1to2 or type2to1 must be filled in.");

	if (isImporting && (type1 == "Sticklib::classobject" || type2 == "Sticklib::classobject"))
		ThrowLeException(LeError::WRONG_DEFINED_TAG, "It is prohibited to describe Sticklib::classobject converter.");

	NormalizeVarType(type1, Dummy<std::string>(), type1, ClassRec());
	NormalizeVarType(type2, Dummy<std::string>(), type2, ClassRec());

	if (!type1to2.empty())
	{
		if (CTYPE_1TO2_CONVERTER.find(std::make_pair(type1, type2)) != CTYPE_1TO2_CONVERTER.end())
			ThrowLeException(LeError::WRONG_DEFINED_TAG, "Cannot register the same converter twice.", type1, type2);
		CTYPE_1TO2_CONVERTER[std::make_pair(type1, type2)] = type1to2;
		CTYPE_2TO1_CONVERTER[std::make_pair(type2, type1)] = type1to2;
	}
	if (!type2to1.empty())
	{
		if (CTYPE_1TO2_CONVERTER.find(std::make_pair(type2, type1)) != CTYPE_1TO2_CONVERTER.end())
			ThrowLeException(LeError::WRONG_DEFINED_TAG, "Cannot register the same converter twice.", type2, type1);
		CTYPE_1TO2_CONVERTER[std::make_pair(type2, type1)] = type2to1;
		CTYPE_2TO1_CONVERTER[std::make_pair(type1, type2)] = type2to1;
	}
}
//----- 19.11.29 Fukushiro M. 追加終 ()-----

/// <summary>
/// Handles the luadef tag.
/// </summary>
/// <param name="tag">The tag.</param>
static void RegisterStickdefTag(const UtilXml::Tag & tag)
{
	// get xxx from "Type="xxx"
	auto type = UtilMisc::FindMapValue(tag.attributes, "type");
//----- 19.12.15 Fukushiro M. 削除始 ()-----
//	if (type == "c-lua" || type.empty())
//	{
//		// get xxx from "ctype="xxx"
//		auto c_type = UtilMisc::FindMapValue(tag.attributes, "ctype");
//		if (!c_type.empty())
//			c_type = NormalizeVarType(Dummy<std::string>(), c_type, ClassRec());
//		// get xxx from "luatype="xxx"
//		auto str_luatype = UtilMisc::FindMapValue(tag.attributes, "sticktype");
//		const LuaType lua_type(str_luatype);
//		// get xxx from "luatoc="xxx"
//		auto lua_to_c = UtilMisc::FindMapValue(tag.attributes, "luatoc");
//		// get xxx from "ctolua="xxx"
//		auto c_to_lua = UtilMisc::FindMapValue(tag.attributes, "ctolua");
//		if (!lua_to_c.empty())
//			CTYPE_TO_LTYPE_L2C[c_type] = std::make_tuple(lua_type, lua_to_c);
//		if (!c_to_lua.empty())
//			CTYPE_TO_LTYPE_C2L[c_type] = std::make_tuple(lua_type, c_to_lua);
//	}
//	else if (type == "c-c")
//	{
//		// get xxx from "ctype1="xxx"
//		auto c_type_1 = UtilMisc::FindMapValue(tag.attributes, "ctype1");
//		if (!c_type_1.empty())
//			c_type_1 = NormalizeVarType(Dummy<std::string>(), c_type_1, ClassRec());
//		// get xxx from "ctype2="xxx"
//		auto c_type_2 = UtilMisc::FindMapValue(tag.attributes, "ctype2");
//		if (!c_type_2.empty())
//			c_type_2 = NormalizeVarType(Dummy<std::string>(), c_type_2, ClassRec());
//		// get xxx from "c1toc2="xxx"
//		auto c_1_to_c_2 = UtilMisc::FindMapValue(tag.attributes, "c1toc2");
//		// get xxx from "c2toc1="xxx"
//		auto c_2_to_c_1 = UtilMisc::FindMapValue(tag.attributes, "c2toc1");
//		if (!c_1_to_c_2.empty())
//		{
//			CTYPE1_CTYPE2_TO_C1TOC2[std::make_pair(c_type_1, c_type_2)] = c_1_to_c_2;
//			CTYPE1_CTYPE2_TO_C2TOC1[std::make_pair(c_type_2, c_type_1)] = c_1_to_c_2;
//		}
//		if (!c_2_to_c_1.empty())
//		{
//			CTYPE1_CTYPE2_TO_C1TOC2[std::make_pair(c_type_2, c_type_1)] = c_2_to_c_1;
//			CTYPE1_CTYPE2_TO_C2TOC1[std::make_pair(c_type_1, c_type_2)] = c_2_to_c_1;
//		}
//	}
//----- 19.12.15 Fukushiro M. 削除終 ()-----
	if (type == "exception")
	{
		// get xxx from "cref="xxx"
		auto crefParam = UtilMisc::FindMapValue(tag.attributes, "cref");
		NormalizeVarType(Dummy<std::string>(), crefParam, crefParam, ClassRec());
		// get xxx from "message="xxx"
		auto messageParam = UtilMisc::FindMapValue(tag.attributes, "message");
		// get xxx from "delete="xxx"
		auto deleteParam = UtilMisc::FindMapValue(tag.attributes, "delete");
		if (!crefParam.empty() && crefParam != "std::exception&")
		{
			EXCEPTION_TO_MESSAGE_DELETE[crefParam] = std::make_tuple(messageParam, deleteParam);
		}
	}
	else
	{
		ThrowLeException(LeError::WRONG_DEFINED_TAG, type);
	}
}

/// <summary>
/// Handles the param tag.
/// e.g. <param name="abc" io="inout" > -> Add argNameToIO { "abc" -> "inout" }
/// e.g. <param name="abc" ctype="void*" > -> Add argNameToAliasCtype { "abc" -> "void*" }
/// e.g. <param name="abc" ltype="lightuserdata" > -> Add argNameToAliasLtype { "abc" -> "lightuserdata" }
/// </summary>
/// <param name="tag">The tag.</param>
/// <param name="argNameToIO">The variable name to io hash.</param>
/// <param name="argNameToAliasCtype">The variable name to ctype hash.</param>
/// <param name="argNameToAliasLtype">The variable name to ltype hash.</param>
/// <param name="autoDelArgNames">The variable name having autodel attribute.</param>
/// <param name="xmlCommentArgNameToSummary">The variable name to summary hash.</param>
static void HandleParamTag(
	const UtilXml::Tag & tag,
	std::unordered_map<std::string, std::string> & argNameToIO,
	std::unordered_map<std::string, std::string> & argNameToAliasCtype,
	std::unordered_map<std::string, LuaType> & argNameToAliasLtype,
// 21.05.19 Fukushiro M. 1行追加 ()
	std::unordered_set<std::string> & autoDelArgNames,
	std::unordered_map<std::string, std::string> & xmlCommentArgNameToSummary
// 21.05.19 Fukushiro M. 1行削除 ()
//	std::vector<std::pair<std::string, std::string>> & xmlCommentArgNameToSummary
)
{
	// e.g. "<param name="argbBack" io="in">"
	// get xxx from "name="xxx"
	auto name = UtilMisc::FindMapValue(tag.attributes, "name");
	if (!name.empty())
	{	//----- name is required -----
		// get xxx from "io="xxx"
		auto io = UtilMisc::FindMapValue(tag.attributes, "io");
		if (io == "in" || io == "out" || io == "inout")
			argNameToIO[name] = io;
		else
			ThrowLeException(LeError::WRONG_DEFINED_TAG, tag.name, "Attribute 'io' is not defined", io);

		// get xxx from "ctype="xxx"
		auto ctype = UtilMisc::FindMapValue(tag.attributes, "ctype");
		if (!ctype.empty())
		{
			NormalizeVarType(ctype, Dummy<std::string>(), ctype, ClassRec());
			argNameToAliasCtype[name] = ctype;
		}

		// get xxx from "ltype="xxx"
		auto ltype = UtilMisc::FindMapValue(tag.attributes, "ltype");
		if (!ltype.empty())
		{
			NormalizeVarType(ltype, Dummy<std::string>(), ltype, ClassRec());
			argNameToAliasLtype[name] = LuaType(ltype);
		}

//----- 21.05.19 Fukushiro M. 追加始 (For AdvanceSoft ＆標準化予定)-----
		// get xxx from "autodel="xxx"
		auto autodel = UtilMisc::FindMapValue(tag.attributes, "autodel");
		if (!autodel.empty())
		{
			if (autodel == "true")
				autoDelArgNames.insert(name);
			else if (autodel != "false")
				ThrowLeException(LeError::WRONG_DEFINED_TAG, tag.name, "Invalid value in attribute 'autodel'", autodel);
		}
//----- 21.05.19 Fukushiro M. 追加終 (For AdvanceSoft ＆標準化予定)-----

		// get summary.
		xmlCommentArgNameToSummary[name] = UtilString::GetLangPart(LANG, tag.GetText());
// 21.05.19 Fukushiro M. 1行削除 ()
//		xmlCommentArgNameToSummary.emplace_back(std::make_pair(name, UtilString::GetLangPart(LANG, tag.GetText())));
	}
} // HandleParamTag.

/// <summary>
/// Handles the returns tag.
/// e.g. <returns ctype="void*"> -> Add argNameToAliasCtype { "__lstickvar_ret" -> "void*" }
/// e.g. <returns ltype="lightuserdata"> -> Add argNameToAliasLtype { "__lstickvar_ret" -> "lightuserdata" }
/// </summary>
/// <param name="tag">The tag.</param>
/// <param name="argNameToAliasCtype">The variable name to ctype hash.</param>
/// <param name="argNameToAliasLtype">The variable name to ltype hash.</param>
/// <param name="autoDelArgNames">The variable name having autodel attribute.</param>
/// <param name="xmlCommentArgNameToSummary">The variable name to summary hash.</param>
static void HandleReturnsTag(
	const UtilXml::Tag & tag,
	std::unordered_map<std::string, std::string> & argNameToAliasCtype,
	std::unordered_map<std::string, LuaType> & argNameToAliasLtype,
// 21.05.19 Fukushiro M. 1行追加 ()
	std::unordered_set<std::string> & autoDelArgNames,
	std::unordered_map<std::string, std::string> & xmlCommentArgNameToSummary
// 21.05.19 Fukushiro M. 1行削除 ()
//	std::vector<std::pair<std::string, std::string>> & xmlCommentArgNameToSummary
)
{
	// get xxx from "ctype="xxx"
	auto ctype = UtilMisc::FindMapValue(tag.attributes, "ctype");
	if (!ctype.empty())
	{
		NormalizeVarType(ctype, Dummy<std::string>(), ctype, ClassRec());
		argNameToAliasCtype["__lstickvar_ret"] = ctype;
	}

	// get xxx from "ltype="xxx"
	auto ltype = UtilMisc::FindMapValue(tag.attributes, "ltype");
	if (!ltype.empty())
	{
		NormalizeVarType(ltype, Dummy<std::string>(), ltype, ClassRec());
		argNameToAliasLtype["__lstickvar_ret"] = LuaType(ltype);
	}

//----- 21.05.19 Fukushiro M. 追加始 (For AdvanceSoft ＆標準化予定)-----
	// get xxx from "autodel="xxx"
	auto autodel = UtilMisc::FindMapValue(tag.attributes, "autodel");
	if (!autodel.empty())
	{
		if (autodel == "true")
			autoDelArgNames.insert("__lstickvar_ret");
		else if (autodel != "false")
			ThrowLeException(LeError::WRONG_DEFINED_TAG, tag.name, "Invalid value in attribute 'autodel'", autodel);
	}
//----- 21.05.19 Fukushiro M. 追加終 (For AdvanceSoft ＆標準化予定)-----

	// get summary.
	xmlCommentArgNameToSummary["__lstickvar_ret"] = UtilString::GetLangPart(LANG, tag.GetText());
// 21.05.19 Fukushiro M. 1行削除 ()
//	xmlCommentArgNameToSummary.emplace_back(std::make_pair("__lstickvar_ret", UtilString::GetLangPart(LANG, tag.GetText())));

} // HandleReturnsTag.

/// <summary>
/// Handles the exception tag.
/// </summary>
/// <param name="tag">The tag.</param>
/// <param name="exceptions">The exception array.</param>
static void HandleExceptionTag(const UtilXml::Tag & tag, std::vector<std::string> & exceptions)
{
	// e.g. "<exception cref="MyException*"></exception>"
	// get xxx from "cref="xxx"
	auto cref = UtilMisc::FindMapValue(tag.attributes, "cref");
	NormalizeVarType(Dummy<std::string>(), cref, cref, ClassRec());
	if (cref.empty())
		ThrowLeException(LeError::WRONG_DEFINED_TAG, tag.name, "cref");
	if (cref != "std::exception&")
		exceptions.emplace_back(cref);
}

/// <summary>
/// Check 3 slash comment.
/// </summary>
/// <param name="text">The text.</param>
/// <returns>true:3 slash comment/false:not 3 slash comment</returns>
static bool Is3Slash(const char * text)
{
	if (strncmp(text, "///", 3) == 0)
		return (text[3] != '/');
	else
	return false;
}

/// <summary>
/// Skips the 3 slash comment.
/// </summary>
/// <param name="text">The text.</param>
/// <returns>true:3 slash comment/false:not 3 slash comment</returns>
static bool Skip3Slash(const char *& text)
{
	if (Is3Slash(text))
	{
		text += 3;
		return true;
	}
	return false;
}

/// <summary>
/// Skips the 2 slash comment.
/// </summary>
/// <param name="text">The text.</param>
/// <returns>true:2 slash comment/false:not 2 slash comment</returns>
static bool Skip2Slash(const char *& text)
{
	if (strncmp(text, "//", 2) == 0)
	{
		text += 2;
		return true;
	}
	return false;
}

static void OutputStructGetFuncContent(const ClassRec & classRec)
{
	if (classRec.superClassId != -1)
		OutputStructGetFuncContent(ClassRec::Get(classRec.superClassId));

	for (const auto & variableAndId : classRec.variableLuanameToVariableId)
	{
		const auto & variableRec = VariableRec::Get(variableAndId.second);

		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
	{
		// ${SRCMARKER}
		//
		// lua_getfield(L, arg, "var1");
		//
		//        stack
		//     +----------+
		//   -1|  value1  |
		//     |----------|
		//     :          :
		//     |----------|
		//     | table A  |--------------+
		//     |----------|              |
		//     :          :              V
		//                       +--------+--------+
		//                       |  Key   | Value  |
		//                       |--------|--------|
		//                       :        :        :
		//                       |--------|--------|
		//                       | "var1" | value1 | ----> Push this value on stack
		//                       |--------|--------|
		//                       :        :        :
		//
		lua_getfield(L, arg, "${variableRec.variableLuaname}");


)", variableRec.variableLuaname);

		int argMajorNumber = 1;
		auto varCtype = variableRec.luaToCConversionPath.front();
		auto getLuaArgFunc = LuaTypeToGetFuncName(CtypeToLuaType(varCtype));
		if (variableRec.luaToCConversionPath.size() == 1)
		{
			auto tmpArgName = variableRec.variableCname;

			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		//
		// ${getLuaArgFunc}(value.${tmpArgName}, L, -1);
		//
		//        stack
		//     +----------+
		//   -1|  value1  | ---> Set this value to 'value.${tmpArgName}'
		//     |----------|
		//     :          :
		//     |----------|
		//     | table A  |
		//     |----------|
		//     :          :
		//
		${getLuaArgFunc}(value.${tmpArgName}, L, -1);

)", tmpArgName, getLuaArgFunc);
		}
		else
		{
			auto tmpArgName = UtilString::Format("_argin_%d", argMajorNumber);

			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		//
		// ${getLuaArgFunc}(${tmpArgName}, L, -1);
		//
		//        stack
		//     +----------+
		//   -1|  value1  | ---> Set this value to '${tmpArgName}'
		//     |----------|
		//     :          :
		//     |----------|
		//     | table A  |
		//     |----------|
		//     :          :
		//
		${varCtype} ${tmpArgName};
		${getLuaArgFunc}(${tmpArgName}, L, -1);


)", varCtype, tmpArgName, getLuaArgFunc);
			for (size_t i = 1; i != variableRec.luaToCConversionPath.size(); i += 2)
			{
				argMajorNumber++;
				const auto & convFunc = variableRec.luaToCConversionPath.at(i);		// Converter.
				varCtype = variableRec.luaToCConversionPath.at(i + 1);
				if (i + 2 == variableRec.luaToCConversionPath.size())
				{
					if (convFunc == "*" || convFunc == "&")
					{
						OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		value.${variableRec.variableCname} = ${convFunc}${tmpArgName};

)", variableRec.variableCname, convFunc, tmpArgName);
					}
					else
					{
						OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${convFunc}(value.${variableRec.variableCname}, ${tmpArgName});

)", variableRec.variableCname, convFunc, tmpArgName);
					}
				}
				else
				{
					auto nextTmpArgName = UtilString::Format("_argin_%d", argMajorNumber);
					if (convFunc == "*" || convFunc == "&")
					{
						OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${nextTmpArgName} = ${convFunc}${tmpArgName};

)", varCtype, nextTmpArgName, convFunc, tmpArgName);
					}
					else
					{
						OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${nextTmpArgName};
		${convFunc}(${nextTmpArgName}, ${tmpArgName});

)", varCtype, nextTmpArgName, convFunc, tmpArgName);
					}
					tmpArgName = nextTmpArgName;
				}
			}
		}

		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(

		// ${SRCMARKER}
		//
		// lua_pop(L, 1);
		//
		//        stack
		//     :          :
		//     |----------|
		//  arg| table A  |
		//     |----------|
		//     :          :
		//
		lua_pop(L, 1);
	}

)");
	}
} // static void OutputStructGetFuncContent(const ClassRec & classRec).

static void OutputStructGetFunc(const ClassRec & classRec)
{
	// fullpath class name. ex. "::ClassA::ClassB::StructC".
	const std::string fullpathClassName = classRec.GetFullpathCname();

	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
/// <summary>
/// Gets ${fullpathClassName} value from Lua stack.
/// </summary>
template<>
void Sticklib::check_lvalue<${fullpathClassName}>(${fullpathClassName} & value, lua_State * L, int arg)
{
	//        stack
	//     :          :
	//     |----------|
	//  arg| table A  |--------------+
	//     |----------|              |
	//     :          :              V
	//                       +--------+--------+
	//                       |  Key   | Value  |
	//                       |--------|--------|
	//                       :        :        :
	//                       |--------|--------|
	//                       | "var1" | value1 |
	//                       |--------|--------|
	//                       :        :        :
	// Premise for following.
	//

)", fullpathClassName);

	OutputStructGetFuncContent(classRec);

	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
}


)");
} // static void OutputStructGetFunc(const ClassRec & classRec).

static void OutputStructGetFuncs(const ClassRec & classRec)
{
	if (classRec.classType == ClassRec::Type::STRUCT)
		OutputStructGetFunc(classRec);
	for (const auto & classId : classRec.memberClassIdArray)
		OutputStructGetFuncs(ClassRec::Get(classId));
} // static void OutputStructGetFuncs(const ClassRec & classRec).

static void OutputStructPushFuncContent(const ClassRec & classRec)
{
	if (classRec.superClassId != -1)
		OutputStructPushFuncContent(ClassRec::Get(classRec.superClassId));

	for (const auto & variableAndId : classRec.variableLuanameToVariableId)
	{
		const auto & variableRec = VariableRec::Get(variableAndId.second);
		int argMajorNumber = 1;
		auto varCtype = variableRec.cToLuaConversionPath.front();
		auto tmpVarName = UtilString::Format("_var_%d", argMajorNumber);
		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
	{
		// ${SRCMARKER}
		${varCtype} ${tmpVarName} = (${varCtype})value.${variableRec.variableCname};

)", varCtype, tmpVarName, variableRec.variableCname);

		for (size_t i = 1; i != variableRec.cToLuaConversionPath.size(); i += 2)
		{
			argMajorNumber++;
			const auto & convFunc = variableRec.cToLuaConversionPath.at(i);		// Converter.
			const auto & varCtype = variableRec.cToLuaConversionPath.at(i + 1);	// Type of variable.
			auto nextTmpVarName = UtilString::Format("_var_%d", argMajorNumber);
			if (convFunc == "*" || convFunc == "&")
			{
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${nextTmpVarName} = ${convFunc}${tmpVarName};

)", varCtype, nextTmpVarName, convFunc, tmpVarName);
			}
			else
			{
				OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${nextTmpVarName};
		${convFunc}(${nextTmpVarName}, ${tmpVarName});

)", varCtype, nextTmpVarName, convFunc, tmpVarName);
			}
			tmpVarName = nextTmpVarName;
		}

//----- 21.05.24 Fukushiro M. 追加始 ()-----
// テスト。
		// "::TestClass0*". Do not remove '*'. 
		auto fullpathCtypeAst = UtilString::Replace(variableRec.cToLuaConversionPath.back(), "&", "");
		auto varLuaType = CtypeToLuaType(fullpathCtypeAst);
		if (varLuaType != LuaType::NIL && varLuaType.CompareHead(LuaType("classobject(")) == 0)
		{	//----- if class object is defined as static variable -----
			auto fullpathCtype = UtilString::Replace(fullpathCtypeAst, "*", "");
			const std::string uniqueClassName = ClassRec::FullpathNameToUniqueName(fullpathCtype);
			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		//
		// Sticklib::set_classobject_to_table(L, "var1", false, obj, "${uniqueClassName}");
		// See the comment described at 'Sticklib::set_classobject_to_table'.
		//
		//        stack  
		//     +----------+      +---------+---------+
		//   -1|  TABLE   |----->| Key     | Value   |
		//     |----------|      +---------+---------+
		//     :          :      | "var1"  |  obj    |
		//                       +---------+---------+
		//
		Sticklib::set_classobject_to_table<${fullpathCtype}>(L, "${variableRec.variableLuaname}", false, ${tmpVarName}, "${uniqueClassName}");
	}

)", fullpathCtype, variableRec.variableLuaname, tmpVarName, uniqueClassName);
		}
		else if (varLuaType != LuaType::NIL && varLuaType.CompareHead(LuaType("array<classobject>(")) == 0)
		{	//----- if array of class object is defined as static variable -----
			auto fullpathCtype = UtilString::Replace(fullpathCtypeAst, "*", "");
			const std::string uniqueClassName = ClassRec::FullpathNameToUniqueName(fullpathCtype);

			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		//
		// Sticklib::set_classobjectarray_to_table(L, "var1", false, obj, "${uniqueClassName}");
		// See the comment described at 'Sticklib::set_classobjectarray_to_table'.
		//
		//        stack  
		//     +----------+      +---------+---------+
		//   -1|  TABLE   |----->| Key     | Value   |
		//     |----------|      +---------+---------+      +-----+--------+
		//     :          :      | "var1"  | TABLE   |----->| Key | Value  |
		//                       +---------+---------+      +-----+--------+
		//                                                  | 1   | value1 |
		//                                                  +-----+--------+
		//                                                  | 2   | value2 |
		//                                                  +-----+--------+
		//                                                  :              :
		//
		Sticklib::set_classobjectarray_to_table<${fullpathCtype}>(L, "${variableRec.variableLuaname}", false, ${tmpVarName}, "${uniqueClassName}");
	}

)", fullpathCtype, variableRec.variableLuaname, tmpVarName, uniqueClassName);
		}
		else

		// テスト。
		// if (variableRec.cToLuaConversionPath.back() == "Sticklib::classobject")ブロックは削除すべき。

//----- 21.05.24 Fukushiro M. 追加終 ()-----


		if (variableRec.cToLuaConversionPath.back() == "Sticklib::classobject")
		{	//----- if class object is defined as static variable -----
			// Get the class type of the class object.
			auto preClassType = variableRec.cToLuaConversionPath[variableRec.cToLuaConversionPath.size() - 3];
			preClassType = UtilString::Replace(preClassType, "*", "", "&", "");
			// Get the unique name (like "lm__TestClass0__") of the class.
			auto preClassId = ClassRec::Get(0).FindClass(preClassType);
			const std::string uniqueClassName = ClassRec::Get(preClassId).GetUniqueClassName();

			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		//
		// Sticklib::set_classobject_to_table(L, "var1", false, obj, "${uniqueClassName}");
		// See the comment described at 'Sticklib::set_classobject_to_table'.
		//
		//        stack  
		//     +----------+      +---------+---------+
		//   -1|  TABLE   |----->| Key     | Value   |
		//     |----------|      +---------+---------+
		//     :          :      | "var1"  |  obj    |
		//                       +---------+---------+
		//
		Sticklib::set_classobject_to_table(L, "${variableRec.variableLuaname}", false, ${tmpVarName}, "${uniqueClassName}");
	}

)", variableRec.variableLuaname, tmpVarName, uniqueClassName);
		}
		else
		{
			auto varCtype = variableRec.cToLuaConversionPath.back();
			varCtype = UtilString::Replace(varCtype, "&", "");
			auto pushLuaValueFunc = LuaTypeToSetFuncName(CtypeToLuaType(varCtype));

			OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(

		// ${SRCMARKER}
		//
		// ${pushLuaValueFunc}(L, value);
		//
		//        stack  
		//     +----------+
		//   -1|  value   |
		//     +----------+      +---------+---------+
		//   -2|  TABLE   |----->| Key     | Value   |
		//     |----------|      +---------+---------+
		//     :          :
		//
		${pushLuaValueFunc}(L, ${tmpVarName});

		// lua_setfield(L, -2, "var1");
		//
		//        stack  
		//     +----------+      +---------+---------+
		//   -1|  TABLE   |----->| Key     | Value   |
		//     |----------|      +---------+---------+
		//     :          :      | "var1"  |  value  |
		//                       +---------+---------+
		//
		lua_setfield(L, -2, "${variableRec.variableLuaname}");
	}

)", pushLuaValueFunc, variableRec.variableLuaname, tmpVarName);
		}
	}
} // static void OutputStructPushFuncContent(const ClassRec & classRec).

static void OutputStructPushFunc(const ClassRec & classRec)
{
	// fullpath class name. ex. "::ClassA::ClassB::StructC".
	const std::string fullpathClassName = classRec.GetFullpathCname();

	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
/// <summary>
/// Pushes ${fullpathClassName} value on Lua stack.
/// </summary>
template<>
void Sticklib::push_lvalue<${fullpathClassName}>(lua_State * L, ${fullpathClassName} const & value)
{
	//        stack
	//     :          :
	//     |          | 
	//     :          : 
	//                  
	// Premise for following.
	//

	// lua_newtable(L);
	//
	//        stack
	//     +----------+      +---------+---------+
	//   -1|  TABLE   |----->| Key     | Value   |
	//     |----------|      +---------+---------+
	//     :          : 
	//
	lua_newtable(L);

)", fullpathClassName);

	OutputStructPushFuncContent(classRec);

	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
}


)");
} // static void OutputStructPushFunc(const ClassRec & classRec).

static void OutputStructPushFuncs(const ClassRec & classRec)
{
	if (classRec.classType == ClassRec::Type::STRUCT)
		OutputStructPushFunc(classRec);
	for (const auto & classId : classRec.memberClassIdArray)
		OutputStructPushFuncs(ClassRec::Get(classId));
} // static void OutputStructPushFuncs(const ClassRec & classRec).

static void OutputDestructor(const ClassRec & classRec)
{
	// fullpath class name. ex. "::ClassA::ClassB::ClassC".
	const std::string fullpathClassName = classRec.GetFullpathCname();
	// fullpath function name. ex. "::ClassA::ClassB::ClassC::~ClassC".
	const std::string fullpathFunCname = fullpathClassName + "::~" + classRec.classCname;

	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
/// <summary>
/// ${fullpathFunCname}()
/// </summary>
static int ${classRec.GetWrapperDestructorName()}(lua_State* L)
{
	try
	{
		// Check the count of arguments.
		if (lua_gettop(L) != 1)
			throw std::invalid_argument("Count of arguments is not correct.");
		// Get the class object.
		auto __wrapper = (StickInstanceWrapper*)Sticklib::check_classobject(L, 1, "${classRec.GetUniqueClassName()}");
		if (__wrapper->own && __wrapper->ptr != nullptr)
		{
			delete (${fullpathClassName}*)__wrapper->ptr;
			__wrapper->ptr = nullptr;
		}
	}
	catch (std::exception & e)
	{
		luaL_error(L, (std::string("C function error:${fullpathFunCname}:") + e.what()).c_str());
	}
	catch (...)
	{
		luaL_error(L, "C function error:${fullpathFunCname}");
	}
	return 0;
}


)", fullpathFunCname, classRec.GetWrapperDestructorName(), fullpathClassName, classRec.GetUniqueClassName());
} // static void OutputDestructor(const ClassRec & classRec).

static void OutputDestructors(const ClassRec & classRec)
{
	if (classRec.classType == ClassRec::Type::CLASS)
		OutputDestructor(classRec);
	for (const auto & classId : classRec.memberClassIdArray)
		OutputDestructors(ClassRec::Get(classId));
} // static void OutputDestructors(const ClassRec & classRec).

/// <summary>
/// Registers the enum converter.
/// This is equivalent of reading "<stickdef type="c-lua" luatype="integer" ctype="::B::EnumC" luatoc="Sticklib::int64_to_enumT<::B::EnumC>" ctolua="Sticklib::enumT_to_int64<::B::EnumC>" />".
/// </summary>
/// <param name="classRec">The enum record.</param>
static void RegisterEnumConverter(const EnumRec & enumRec)
{
	// e.g.
	// enum A { ... } -> enumName="A"
	const std::string enumName = enumRec.GetFullpathCname();
	const std::string i64_to_enum = std::string("Sticklib::int64_to_enumT<") + enumName + ">";
	const std::string enum_to_i64 = std::string("Sticklib::enumT_to_int64<") + enumName + ">";
//----- 20.01.19 Fukushiro M. 変更前 ()-----
//	CTYPE_1TO2_CONVERTER[std::make_pair("__int64", enumName)] = i64_to_enum;
//	CTYPE_1TO2_CONVERTER[std::make_pair(enumName, "__int64")] = enum_to_i64;
//	CTYPE_2TO1_CONVERTER[std::make_pair("__int64", enumName)] = enum_to_i64;
//	CTYPE_2TO1_CONVERTER[std::make_pair(enumName, "__int64")] = i64_to_enum;
//----- 20.01.19 Fukushiro M. 変更後 ()-----
	UtilXml::Tag tag;
	tag.name = "stickconv";
	tag.attributes["type1"] = "__int64";
	tag.attributes["type2"] = enumName;
	tag.attributes["type1to2"] = i64_to_enum;
	tag.attributes["type2to1"] = enum_to_i64;
	// tag.type = UtilXml::Tag::ONESHOT;
	RegisterStickconvTag(tag, false);
//----- 20.01.19 Fukushiro M. 変更終 ()-----
}

//// <summary>
/// Registers the class converter.
/// This is equivalent of reading "<stickdef type="c-lua" luatype="userdata" ctype="::X::A*" luatoc="Sticklib::linteger_to_intT<__int64>" ctolua="Sticklib::intT_to_linteger<__int64>" />".
/// </summary>
/// <param name="classRec">The class record.</param>
static void RegisterClassConverter(const ClassRec & classRec)
{
// テスト。この関数は削除。

	// Think about the following case.
	// ----------------
	// class A {};
	// void Func(A& a);
	// ----------------
	// The above program should convert to the following.
	// ----------------
	// Manglib::classobject p;
	// p = lua_popobject(L);
	// A* a;
	// classobject_to_typeptr<A>(a, p);
	// Func(*a);
	// ----------------
	// So, register void* <--> A* converter in this function, and register A* --> A procedure in the function GetLuaToCppConverter.

	const std::string className = classRec.GetFullpathCname();
	const std::string object_to_classp = std::string("Sticklib::classobject_to_typeptr<") + className + ">";
	const std::string classp_to_object = std::string("Sticklib::typeptr_to_classobject<") + className + ">";
//----- 20.01.19 Fukushiro M. 変更前 ()-----
//	CTYPE_1TO2_CONVERTER[std::make_pair("Sticklib::classobject", className + "*")] = object_to_classp;
//	CTYPE_1TO2_CONVERTER[std::make_pair(className + "*", "Sticklib::classobject")] = classp_to_object;
//	CTYPE_2TO1_CONVERTER[std::make_pair("Sticklib::classobject", className + "*")] = classp_to_object;
//	CTYPE_2TO1_CONVERTER[std::make_pair(className + "*", "Sticklib::classobject")] = object_to_classp;
//----- 20.01.19 Fukushiro M. 変更後 ()-----
	UtilXml::Tag tag;
	tag.name = "stickconv";
	tag.attributes["type1"] = "Sticklib::classobject";
	tag.attributes["type2"] = className + "*";
	tag.attributes["type1to2"] = object_to_classp;
	tag.attributes["type2to1"] = classp_to_object;
//	tag.type = UtilXml::Tag::ONESHOT;
	RegisterStickconvTag(tag, false);
//----- 20.01.19 Fukushiro M. 変更終 ()-----
}

//// <summary>
/// Registers a lua-type of the struct.
/// This is equivalent of reading below.
/// <sticktype name="__xxxx___" ctype="::MyStruct" getfunc="Sticklib::check_lvalue" setfunc="Sticklib::push_lvalue" />
/// <sticktype name="array<__xxxx___>" ctype="std::vector<::MyStruct>" getfunc="Sticklib::check_array<::MyStruct>" setfunc="Sticklib::push_array<::MyStruct>" />
/// <sticktype name="hash<number,__xxxx___>" ctype="std::unordered_map<double,::MyStruct>" getfunc="Sticklib::check_hash<double,::MyStruct>" setfunc="Sticklib::push_hash<double,::MyStruct>" />
///   :
/// </summary>
/// <param name="classRec">The class record.</param>
static void RegisterStructType(const ClassRec & classRec)
{
	UtilXml::Tag tag;
	tag.name = "sticktype";

// 20.01.27 Fukushiro M. 1行変更 ()
//	const auto uniqClassName = classRec.GetUniqueClassName();
	const auto fullLuaName = classRec.GetFullpathLuaname();
	const auto fullCname = classRec.GetFullpathCname();
//	tag.type = UtilXml::Tag::ONESHOT;

	tag.attributes["name"] = fullLuaName;
	tag.attributes["ctype"] = fullCname;
	tag.attributes["getfunc"] = "Sticklib::check_lvalue";
	tag.attributes["setfunc"] = "Sticklib::push_lvalue";
	RegisterSticktypeTag(tag);

	tag.attributes["name"] = UtilString::Format("array<%s>", fullLuaName.c_str());
	tag.attributes["ctype"] = UtilString::Format("std::vector<%s>", fullCname.c_str());
	tag.attributes["getfunc"] = UtilString::Format("Sticklib::check_array<%s>", fullCname.c_str());
	tag.attributes["setfunc"] = UtilString::Format("Sticklib::push_array<%s>", fullCname.c_str());
	RegisterSticktypeTag(tag);

	tag.attributes["name"] = UtilString::Format("hash<number,%s>", fullLuaName.c_str());
	tag.attributes["ctype"] = UtilString::Format("std::unordered_map<double,%s>", fullCname.c_str());
	tag.attributes["getfunc"] = UtilString::Format("Sticklib::check_hash<double,%s>", fullCname.c_str());
	tag.attributes["setfunc"] = UtilString::Format("Sticklib::push_hash<double,%s>", fullCname.c_str());
	RegisterSticktypeTag(tag);

	tag.attributes["name"] = UtilString::Format("hash<integer,%s>", fullLuaName.c_str());
	tag.attributes["ctype"] = UtilString::Format("std::unordered_map<__int64,%s>", fullCname.c_str());
	tag.attributes["getfunc"] = UtilString::Format("Sticklib::check_hash<__int64,%s>", fullCname.c_str());
	tag.attributes["setfunc"] = UtilString::Format("Sticklib::push_hash<__int64,%s>", fullCname.c_str());
	RegisterSticktypeTag(tag);

	tag.attributes["name"] = UtilString::Format("hash<boolean,%s>", fullLuaName.c_str());
	tag.attributes["ctype"] = UtilString::Format("std::unordered_map<bool,%s>", fullCname.c_str());
	tag.attributes["getfunc"] = UtilString::Format("Sticklib::check_hash<bool,%s>", fullCname.c_str());
	tag.attributes["setfunc"] = UtilString::Format("Sticklib::push_hash<bool,%s>", fullCname.c_str());
	RegisterSticktypeTag(tag);

	tag.attributes["name"] = UtilString::Format("hash<string,%s>", fullLuaName.c_str());
	tag.attributes["ctype"] = UtilString::Format("std::unordered_map<std::string,%s>", fullCname.c_str());
	tag.attributes["getfunc"] = UtilString::Format("Sticklib::check_hash<std::string,%s>", fullCname.c_str());
	tag.attributes["setfunc"] = UtilString::Format("Sticklib::push_hash<std::string,%s>", fullCname.c_str());
	RegisterSticktypeTag(tag);
}


//----- 21.05.23 Fukushiro M. 追加始 ()-----
// テスト。
static void RegisterClassType(const ClassRec & classRec)
{
	UtilXml::Tag tag;
	tag.name = "sticktype";

	const auto fullLuaName = classRec.GetFullpathLuaname();
	const auto fullCname = classRec.GetFullpathCname();

	tag.attributes["name"] = UtilString::Format("classobject(%s)", fullCname.c_str());
	tag.attributes["ctype"] = fullCname + "*";
	tag.attributes["getfunc"] = LuaTypeToGetFuncName(LuaType("classobject")) + "<" + fullCname + ">";
	tag.attributes["setfunc"] = LuaTypeToSetFuncName(LuaType("classobject")) + "<" + fullCname + ">";
	RegisterSticktypeTag(tag);

	tag.attributes["name"] = UtilString::Format("array<classobject>(%s)", fullCname.c_str());
	tag.attributes["ctype"] = UtilString::Format("std::vector<%s*>", fullCname.c_str());
	tag.attributes["getfunc"] = LuaTypeToGetFuncName(LuaType("array<classobject>")) + "<" + fullCname + ">";
	tag.attributes["setfunc"] = LuaTypeToSetFuncName(LuaType("array<classobject>")) + "<" + fullCname + ">";
	RegisterSticktypeTag(tag);  
}
//----- 21.05.23 Fukushiro M. 追加終 ()-----






//----- 20.03.07  削除始 ()-----
///// <summary>
///// Decide the possibility of xml text.
///// text is 3 slash comment.
///// Purpose of this function is to remove like this line: '/// // <stickconv ...>'
///// </summary>
///// <param name="text">Comment text after 3 slash</param>
///// <returns>true:Might be xml/false:Is not xml</returns>
//static bool Is3SlashXml(const char * text)
//{
//	// case1:
//	// <stickconv type1="std::string" type2="char*" type1to2="Sticklib::astring_to_atext" type2to1="Sticklib::atext_to_astring" />
//	// case2:
//	// <stickconv
//	// type1="std::string" type2="char*" type1to2="Sticklib::astring_to_atext" type2to1="Sticklib::atext_to_astring"
//	// />
//	// case3:
//	// <stickconv type1=
//	// "std::string" type2="char*" type1to2="Sticklib::astring_to_atext" type2to1="Sticklib::atext_to_astring" />
//	// case4:
//	// <stickconv type1
//	// ="std::string" type2="char*" type1to2="Sticklib::astring_to_atext" type2to1="Sticklib::atext_to_astring" />
//
//	return (
//		('a' <= text[0] && text[0] <= 'z') ||
//		('A' <= text[0] && text[0] <= 'Z') ||
//		text[0] == '\"' ||
//		text[0] == '<' ||
//		text[0] == '=' ||
//		(text[0] == '/' && text[1] == '>')
//		);
//}
//----- 20.03.07  削除終 ()-----

/// <summary>
/// Join continuous "///" comment.
/// e.g.
/// /// comment1.
/// ///   comment2.
/// -> return "comment1.\r\n  comment2.\r\n"
/// </summary>
/// <param name="readBufferedFile">The read buffered file.</param>
/// <returns>text</returns>
static std::string Join3SlashComment(ReadBufferedFile & readBufferedFile)
{
	//  /// <hello>abc</hello>
	//  /// <world>abc</word>
	//            |
	//            V
	// return "<hello>abc</hello><world>abc</word>"

	std::string commentText;
	std::string strText;
	while (readBufferedFile.ReadOneSentence(strText))
	{	//---- Loop each line -----
		const char* text = strText.c_str();
		text = UtilString::SkipSpaceTab(text);
		if (Skip3Slash(text))
		{
//----- 20.03.15  変更前 ()-----
//			text = UtilString::SkipSpaceTab(text);
//			commentText += text;
//----- 20.03.15  変更後 ()-----
			// Skip first one space.
			if (*text == ' ') text++;
			commentText += text;
			commentText += "\r\n";
//----- 20.03.15  変更終 ()-----
		}
		else
		{
			readBufferedFile.PutBackOneSentence();
			break;
		}
	}
	return commentText;
}

/// <summary>
/// After the class or enum definision, check the block description is following or not.
/// </summary>
/// <param name="readBufferedFile">The read buffered file. Notes:'{' is skipped.</param>
/// <returns>true:block description is following/false:not</returns>
static bool IsContinuousBlock(ReadBufferedFile & readBufferedFile)
{
	bool isContinuousBlock = false;
	std::string strText;
	for (;;)
	{	//---- Loop each line -----
		if (!readBufferedFile.ReadOneSentence(strText))
			ThrowLeException(LeError::UNEXPECTED_EOF);

		const char* text = strText.c_str();
		text = UtilString::SkipSpaceTab(text);
		if (*text == '{')
		{
			isContinuousBlock = true;
			break;
		}
		else if (*text == '}')
		{
			ThrowLeException(LeError::UNEXPECTED_CHAR, *text);
		}
		else if (*text == ';')
		{
			isContinuousBlock = false;
			break;
		}
		else if (strncmp(text, "//", 2) == 0)
		{
			continue;
		}
		else if (*text == '\0')
		{	//----- Empty line -----
			continue;
		}
		else
		{	//----- 上記以外の場合 -----
			ThrowLeException(LeError::UNEXPECTED_CHAR, *text);
		}
	}
	return isContinuousBlock;
}


/// <summary>
/// <para> Parses enum block.                        </para>
/// <para> e.g.                                      </para>
/// <para> ----------------------------              </para>
/// <para> enum class X                              </para>
/// <para> {                                         </para>
/// <para>     A,   <-- readBufferedFile is here.    </para>
/// <para>     B,                                    </para>
/// <para>     C,                                    </para>
/// <para> };                                        </para>
/// <para> ----------------------------              </para>
/// </summary>
/// <param name="readBufferedFile">The read buffered file.</param>
/// <param name="luanameToCname">Returns hash value which records item's lua-name and c++-name pairs</param>
/// <param name="luanameToSummary">Returns hash value which records item's lua-name and summary pairs</param>
static void ParseEnumContent(
	ReadBufferedFile & readBufferedFile,
	std::unordered_map<std::string, std::string> & luanameToCname,
	std::unordered_map<std::string, std::string> & luanameToSummary
)
{
	// <stick export="true">
	bool xmlCommentIsExport = false;
	// <stick lname="????">
	std::string xmlCommentLuaname;
	// <summary>????</summary>
	std::string xmlCommentSummary;

	auto xmlCommentClear = [&]
	{
		xmlCommentIsExport = false;
		xmlCommentLuaname.clear();
		xmlCommentSummary.clear();
	}; 

	std::string strText;
	while (readBufferedFile.ReadOneSentence(strText))
	{	//---- Loop each line -----
		auto text = strText.c_str();
		if (*text == '{')
		{
			ThrowLeException(LeError::UNEXPECTED_CHAR, text);
		}
		else if (*text == '}')
		{
			return;
		}
		else if (*text == ';')
		{
			ThrowLeException(LeError::UNEXPECTED_CHAR, text);
		}
		else if (Is3Slash(text))
		{	//--- if begins by "///" ---
			readBufferedFile.PutBackOneSentence();
			// Join continuous "///" comment.
			std::string commentText = Join3SlashComment(readBufferedFile);
			text = commentText.c_str();
			while (*text != '\0')
			{
				UtilXml::Tag tag;
				if (UtilXml::Parse(tag, text))
				{
					if (xmlCommentIsExport)
					{	//----- if lua-export mode -----
						if (tag.name == "stick")
						{
							ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "<stick> tag occured repeatedly");
						}
						if (tag.name == "summary")
							xmlCommentSummary = UtilString::GetLangPart(LANG, tag.GetText());
					}
					else
					{
						if (tag.name == "stick")
						{
							HandleStickTag(
								tag,
								xmlCommentIsExport,
								Dummy<std::string>(),
								xmlCommentLuaname,
								Dummy<std::string>(),
								Dummy<LuaType>(),
								Dummy<std::string>()
							);
						}
//----- 20.06.15 Fukushiro M. 追加始 ()-----
						else if (tag.name == "summary")
						{
							HandleStickTag(
								tag,
								xmlCommentIsExport,
								Dummy<std::string>(),
								xmlCommentLuaname,
								Dummy<std::string>(),
								Dummy<LuaType>(),
								Dummy<std::string>()
							);
							if (xmlCommentIsExport)
								xmlCommentSummary = UtilString::GetLangPart(LANG, tag.GetText());
						}
//----- 20.06.15 Fukushiro M. 追加終 ()-----
					}
				}
			}
		}
		else if (Skip2Slash(text))
		{	//----- Comment starts with [//] -----
		}
		else if (strncmp(text, "/*", 2) == 0)
		{	//----- Comment starts with [/*] -----
		}
		else if (*text == '\0')
		{	//----- Empty line -----
		}
		else
		{	//----- Else -----
			if (xmlCommentIsExport)
			{	//----- Export mode -----
				// i.g.
				// enum { A = 10, B = 20, C };
				//         |
				//         V
				// strText = " A = 10, B = 20, C "
				std::vector<std::string> items;
				UtilString::Split(items, strText, ',');
				if (!items.empty())
				{
					// Register first item only. Second and later items are not the target of export.
					// item = " A = 10"
					auto item = items[0];
					UtilString::Trim(item);
					std::string cname;
					UtilString::Split(cname, Dummy<std::string>(), item, '=');
					UtilString::Trim(cname);
					if (xmlCommentLuaname.empty())
						xmlCommentLuaname = cname;
					luanameToCname[xmlCommentLuaname] = cname;
					luanameToSummary[xmlCommentLuaname] = xmlCommentSummary;
				}
			}
			xmlCommentClear();
		}
	}
	ThrowLeException(LeError::UNEXPECTED_EOF);
}

/// <summary>
/// Parses the source. First step.
/// 1. Registers Lua-type, Lua-C++ converter and Exceptions defined by <sticktype>, <stickconv> and <stickdef> tag.
/// 2. Registers class and struct records.
/// 
/// Think about parsing the following.
/// ----------------------------------
/// L01:  extern int func1(int a);
/// L02:  class A
/// L03:  {
/// L04:  public:
/// L05:      int func2(int a);
/// L06:  };
/// ----------------------------------
/// When L01: classRec={ClassRec::Type::GLOBAL, ""}
///           parentClassPrefix=""
/// When L03: Recursive call self.
/// When L04: classRec={ClassRec::Type::CLASS, "A"}
///           parentClassPrefix="A::"
/// </summary>
/// <param name="readBufferedFile">The read buffered file.</param>
/// <param name="classRec">The class and member.</param>
static void ParseSource1(ReadBufferedFile & readBufferedFile, ClassRec & classRec)
{
	// <stick export="true">
	bool xmlCommentIsExport = false;
	// <stick type="????">
	std::string xmlCommentClassType;
	// <stick lname="????">
	std::string xmlCommentLuaname;
	// <stick ctype="????">
	std::string xmlCommentCtype;
	// <stick ltype="????">
	LuaType xmlCommentLuatype;
	// <stick super="????">
	std::string xmlCommentSuper;

	auto xmlCommentClear = [&]
	{
		xmlCommentIsExport = false;
		xmlCommentClassType.clear();
		xmlCommentLuaname.clear();
		xmlCommentCtype.clear();
		xmlCommentLuatype = LuaType::NIL;
		xmlCommentSuper.clear();
	};

	std::string strText;
	while (readBufferedFile.ReadOneSentence(strText))
	{	//---- Loop each line -----
		const char* text = strText.c_str();
		text = UtilString::SkipSpaceTab(text);
		if (*text == '{')
		{
			xmlCommentClear();

			// Parses source until next encounter with '}'.
			// from this point, ineffective lua-tag scope.
			// so ClassRec().classType = ClassRec::Type::STOP.
			ParseSource1(readBufferedFile, Dummy<ClassRec>());
		}
		else if (*text == '}')
		{
			return;
		}
		else if (*text == ';')
		{
			xmlCommentClear();
		}
		else if (Is3Slash(text))
		{	//--- if begins by "///" ---
			readBufferedFile.PutBackOneSentence();
			// Join continuous "///" comment.
			std::string commentText = Join3SlashComment(readBufferedFile);
			text = commentText.c_str();
			while (*text != '\0')
			{
				UtilXml::Tag tag;
				if (UtilXml::Parse(tag, text))
				{
					if (classRec.classType == ClassRec::Type::NONE)
					{	//----- if inside of the ineffective lua-tag scope -----
						// you cannot describe LuaStick original tags inside of the unexported scope.
						if (tag.name == "stick" || tag.name == "stickdef" || tag.name == "sticktype" || tag.name == "stickconv")
							ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "LuaStick xml-tag occured inside of the unexported scope.");
					}
					else
					{
						if (xmlCommentIsExport)
						{	//----- if lua-export mode -----
							if (tag.name == "stick")
								ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "<stick> tag occured repeatedly");
						}
						else
						{
// 20.06.15 Fukushiro M. 1行変更 ()
//							if (tag.name == "stick")
							if (tag.name == "stick" || tag.name == "summary")
								HandleStickTag(
									tag,
									xmlCommentIsExport,
									xmlCommentClassType,
									xmlCommentLuaname,
									xmlCommentCtype,
									xmlCommentLuatype,
									xmlCommentSuper
								);
						}
						if (tag.name == "sticktype")
							RegisterSticktypeTag(tag);
						else if (tag.name == "stickconv")
							RegisterStickconvTag(tag, true);
						else if (tag.name == "stickdef")
							RegisterStickdefTag(tag);
					}
				}
			}
		}
		else if (Skip2Slash(text))
		{	//----- Comment starts with [//] -----
		}
		else if (strncmp(text, "/*", 2) == 0)
		{	//----- Comment starts with [/*] -----
		}
		else if (*text == '\0')
		{	//----- Empty line -----
		}
		else
		{	//----- Else -----
			for (;;)
			{
				if (xmlCommentIsExport)
				{	//----- Export mode -----
					// [extern TTT FFF (PPP)]のTTT,FFF,PPPを抽出
					if (ParseFuncDef(
						Dummy< std::string>(),	// funcStrType,
						Dummy< std::string>(),	// returnCtype,
						Dummy< std::string>(),	// funcCname,
						Dummy< std::string>(),	// arguments,
						text,
						classRec.classCname
					))
					{
						break;
					}

					ClassRec::Type nextClassType;
					std::string nextClassName;
					std::string nextSuperClassName;
					if (CheckClass(nextClassType, nextClassName, nextSuperClassName, text, xmlCommentClassType))
					{
//----- 20.05.18  変更前 ()-----
//						if (
//							(
//								classRec.classType == ClassRec::Type::CLASS ||
//								classRec.classType == ClassRec::Type::INCONSTRUCTIBLE ||
//								classRec.classType == ClassRec::Type::STRUCT ||
//								classRec.classType == ClassRec::Type::STATICCLASS
//							)
//							&&
//							nextClassType == ClassRec::Type::NAMESPACE
//							)
//							ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "Cannot use namespace inside of the class.");
//						if (classRec.memberClassLuanameSet.find(xmlCommentLuaname.empty() ? nextClassName : xmlCommentLuaname) != classRec.memberClassLuanameSet.end())
//							ThrowLeException(LeError::REGISTER_SAME_NAME, "Cannot register same class name twice.", xmlCommentLuaname.empty() ? nextClassName : xmlCommentLuaname);
//----- 20.05.18  変更後 ()-----
						if (
							classRec.classType == ClassRec::Type::CLASS ||
							classRec.classType == ClassRec::Type::INCONSTRUCTIBLE ||
							classRec.classType == ClassRec::Type::STRUCT ||
							classRec.classType == ClassRec::Type::STATICCLASS
							)
						{
							if (nextClassType == ClassRec::Type::NAMESPACE)
								ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "Cannot use namespace inside of the class.");

							// class, and struct cannot register twice, except namespace. You can describe same namespace into two or more source files.
							if (classRec.memberClassLuanameSet.find(xmlCommentLuaname.empty() ? nextClassName : xmlCommentLuaname) != classRec.memberClassLuanameSet.end())
								ThrowLeException(LeError::REGISTER_SAME_NAME, "Cannot register same class name twice.", xmlCommentLuaname.empty() ? nextClassName : xmlCommentLuaname);
						}
//----- 20.05.18  変更終 ()-----
						if (!xmlCommentSuper.empty())
						{	//----- If super-class was specified with xml-comment -----
							if (xmlCommentSuper == "-")
								nextSuperClassName.clear();
							else
								nextSuperClassName = xmlCommentSuper;
						}

						if (IsContinuousBlock(readBufferedFile))
						{	//----- if block description is following -----
//----- 21.05.22 Fukushiro M. 追加始 ()-----
							auto memberClassId = classRec.FindClass(nextClassName);
							if (memberClassId != -1)
							{	//----- nextClassName was already registered in another header file. Namespace can appear more than two times -----
								auto & memberClassRec = ClassRec::Get(memberClassId);
								// Parses source until next encounter with '}'.
								ParseSource1(readBufferedFile, memberClassRec);
							}
							else
							{	//----- It is first time nextClassName appears -----
//----- 21.05.22 Fukushiro M. 追加終 ()-----
								auto & memberClassRec = ClassRec::New(classRec.id);
								memberClassRec.classType = nextClassType;
								memberClassRec.classCname = nextClassName;
								memberClassRec.classLuaname = xmlCommentLuaname.empty() ? nextClassName : xmlCommentLuaname;
								memberClassRec.superClassId = nextSuperClassName.empty() ? -1 : classRec.FindClass(nextSuperClassName);
								classRec.memberClassIdArray.emplace_back(memberClassRec.id);
								// already used class lname.
								classRec.memberClassLuanameSet.insert(memberClassRec.classLuaname);

								// Register class converter.
								if (memberClassRec.classType == ClassRec::Type::CLASS || memberClassRec.classType == ClassRec::Type::INCONSTRUCTIBLE)
								{
// テスト。
//									RegisterClassConverter(memberClassRec);
									RegisterClassType(memberClassRec);
								}
								else if (memberClassRec.classType == ClassRec::Type::STRUCT)
								{
									RegisterStructType(memberClassRec);
								}

								// Parses source until next encounter with '}'.
								ParseSource1(readBufferedFile, memberClassRec);
// 21.05.22 Fukushiro M. 1行追加 ()
							}
						}

						break;
					}

					EnumRec::Type nextEnumType;
					if (CheckEnum(nextEnumType, Dummy<std::string>(), text, xmlCommentClassType))
					{
						if (IsContinuousBlock(readBufferedFile))
						{	//----- if block description is following -----
							ParseEnumContent(readBufferedFile, Dummy<std::unordered_map<std::string, std::string>>(), Dummy<std::unordered_map<std::string, std::string>>());
						}
						break;
					}

					if (
						ParseConstDef(Dummy<std::string>(), Dummy<std::string>(), text) ||
						ParseConstexprDef(Dummy<std::string>(), Dummy<std::string>(), text) ||
						ParseDefineDef(Dummy<std::string>(), text)
					)
					{
						break;
					}
					if (ParseVariableDef(Dummy<std::string>(), Dummy<std::string>(), text))
					{
						break;
					}
					ThrowLeException(LeError::NOT_SUPPORTED, text);
				}
				break;
			}
			xmlCommentClear();
		}
	}
}

/// <summary>
/// Parses the source. First step.
/// 1. Register enum
/// 2. Register function
/// 3. Register constant value
/// 4. Register member variable
/// </summary>
/// <param name="readBufferedFile">The read buffered file.</param>
/// <param name="classRec">The class and member.</param>
static void ParseSource2(ReadBufferedFile & readBufferedFile, ClassRec & classRec)
{
	//
	// void FuncA();   <--- parentClassPrefix="", classRec.nextClassName=""
	// class A {
	//   void FuncB();   <--- parentClassPrefix="::", classRec.nextClassName="A"
	//   class B {
	//     void FuncC();   <--- parentClassPrefix="::A::", classRec.nextClassName="B"
	//

	// variable name -> io { "in" / "out" / "inout" }
	// e.g. <param name="abc" io="inout" > -> { "abc" -> "inout" }
	std::unordered_map<std::string, std::string> xmlCommentArgNameToIO;
	// variable name -> ctype alias.
	// e.g. <param name="abc" ctype="void*" > -> { "abc" -> "void*" }
	std::unordered_map<std::string, std::string> xmlCommentArgNameToAliasCtype;
	// variable name -> ltype alias.
	// e.g. <param name="abc" ltype="lightuserdata" > -> { "abc" -> "lightuserdata" }
	std::unordered_map<std::string, LuaType> xmlCommentArgNameToAliasLtype;
//----- 21.05.19 Fukushiro M. 追加始 ()-----
	// Argument name set which has autodel option.
	// e.g. <param name="hello" autodel="true"...>  <param name="world" autodel="true"...>  <returns autodel="true">
	// autoDelargNames={ "hello", "world", "__lstickvar_ret" }
	std::unordered_set<std::string> xmlCommentAutoDelArgNames;
//----- 21.05.19 Fukushiro M. 追加終 ()-----
	// variable name -> summary
	// e.g. <param name="abc">hello</param> -> { "abc" -> "hello" }
	std::unordered_map<std::string, std::string> xmlCommentArgNameToSummary;
// 21.05.19 Fukushiro M. 1行削除 ()
//	std::vector<std::pair<std::string, std::string>> xmlCommentArgNameToSummary;

	// exception name array. <exception cref="?????" ...>
	std::vector<std::string> xmlCommentExceptions;
	// <stick export="true">
	bool xmlCommentIsExport = false;
	// <stick type="????">
	std::string xmlCommentClassType;
	// <stick lname="????">
	std::string xmlCommentLuaname;
	// <stick ctype="????">
	std::string xmlCommentCtype;
	// <stick ltype="????">
	LuaType xmlCommentLuatype;
	// <summary>????</summary>
	std::string xmlCommentSummary;

	auto xmlCommentClear = [&]
	{
		xmlCommentArgNameToIO.clear();
		xmlCommentArgNameToAliasCtype.clear();
		xmlCommentArgNameToAliasLtype.clear();
// 21.05.19 Fukushiro M. 1行追加 ()
		xmlCommentAutoDelArgNames.clear();
		xmlCommentArgNameToSummary.clear();
		xmlCommentExceptions.clear();
		xmlCommentIsExport = false;
		xmlCommentClassType.clear();
		xmlCommentLuaname.clear();
		xmlCommentCtype.clear();
		xmlCommentLuatype = LuaType::NIL;
		xmlCommentSummary.clear();
	};

	std::string strText;
	while (readBufferedFile.ReadOneSentence(strText))
	{	//---- Loop each line -----
		const char* text = strText.c_str();
		text = UtilString::SkipSpaceTab(text);
		if (*text == '{')
		{
			xmlCommentClear();

			// Parses source until next encounter with '}'.
			// from this point, ineffective lua-tag scope.
			// so ClassRec().classType = ClassRec::Type::STOP.
			ParseSource2(readBufferedFile, Dummy<ClassRec>());
		}
		else if (*text == '}')
		{
			return;
		}
		else if (*text == ';')
		{
			xmlCommentClear();
		}
		else if (Is3Slash(text))
		{	//--- if begins by "///" ---
			readBufferedFile.PutBackOneSentence();
			// Join continuous "///" comment.
			std::string commentText = Join3SlashComment(readBufferedFile);
			text = commentText.c_str();
			while (*text != '\0')
			{
				UtilXml::Tag tag;
				if (UtilXml::Parse(tag, text))
				{
					// Tag error was already checked in 'ParseSource1'. So you do not have to check it here.
					if (classRec.classType != ClassRec::Type::NONE)
					{	//----- if inside of lua-tag scope -----
						if (xmlCommentIsExport)
						{	//----- if lua-export mode -----
							if (tag.name == "param")
								HandleParamTag(
									tag,
									xmlCommentArgNameToIO,
									xmlCommentArgNameToAliasCtype,
									xmlCommentArgNameToAliasLtype,
// 21.05.19 Fukushiro M. 1行追加 ()
									xmlCommentAutoDelArgNames,
									xmlCommentArgNameToSummary
								);
							else if (tag.name == "returns")
								HandleReturnsTag(
									tag,
									xmlCommentArgNameToAliasCtype,
									xmlCommentArgNameToAliasLtype,
// 21.05.19 Fukushiro M. 1行追加 ()
									xmlCommentAutoDelArgNames,
									xmlCommentArgNameToSummary
								);
							else if (tag.name == "exception")
								HandleExceptionTag(tag, xmlCommentExceptions);
							else if (tag.name == "summary")
								xmlCommentSummary = UtilString::GetLangPart(LANG, tag.GetText());
						}
						else
						{
							if (tag.name == "stick")
							{
								HandleStickTag(
									tag,
									xmlCommentIsExport,
									xmlCommentClassType,
									xmlCommentLuaname,
									xmlCommentCtype,
									xmlCommentLuatype,
									Dummy<std::string>()
								);
							}
//----- 20.06.15 Fukushiro M. 追加始 ()-----
							else if (tag.name == "summary")
							{
								HandleStickTag(
									tag,
									xmlCommentIsExport,
									xmlCommentClassType,
									xmlCommentLuaname,
									xmlCommentCtype,
									xmlCommentLuatype,
									Dummy<std::string>()
								);
								if (xmlCommentIsExport)
									xmlCommentSummary = UtilString::GetLangPart(LANG, tag.GetText());
							}
//----- 20.06.15 Fukushiro M. 追加終 ()-----
						}
					}
				}
			}
		}
		else if (Skip2Slash(text))
		{	//----- Comment starts with [//] -----
		}
		else if (strncmp(text, "/*", 2) == 0)
		{	//----- Comment starts with [/*] -----
		}
		else if (*text == '\0')
		{	//----- Empty line -----
		}
		else
		{	//----- Else -----
			if (xmlCommentIsExport)
			{	//----- Export mode -----
				for (;;)
				{
					{
						// [extern TTT FFF (PPP)]のTTT,FFF,PPPを抽出
						FuncRec::Type funcType;
						std::string funcCname;
						std::vector<std::string> argNames;
						std::unordered_map<std::string, std::string> argNameToCtype;
						std::unordered_map<std::string, std::string> argNameToRawCtype;
						std::unordered_set<std::string> inArgNames;
						std::unordered_set<std::string> outArgNames;
						std::unordered_map<std::string, std::vector<std::string>> inArgNameToLuaToCppConversionPath;
						std::unordered_map<std::string, std::vector<std::string>> outArgNameToCppToLuaConversionPath;
						// std::string returnRawCtype;
						if (
							CheckFunction(
								funcType,
								funcCname,
								argNames,
								argNameToCtype,
								argNameToRawCtype,
								inArgNames,
								outArgNames,
								inArgNameToLuaToCppConversionPath,
								outArgNameToCppToLuaConversionPath,
								classRec,
								text,
								xmlCommentClassType,
								xmlCommentArgNameToIO,
								xmlCommentArgNameToAliasCtype,
								xmlCommentArgNameToAliasLtype
							)
							)
						{
							const std::string funcLuaname = xmlCommentLuaname.empty() ? funcCname : xmlCommentLuaname;
// 21.05.16 Fukushiro M. 1行変更 ()
//							const int luaArgCount = (funcType == FuncRec::Type::METHOD) ? (int)inArgNames.size() + 1 : (int)argNames.size();
							const int luaArgCount = (funcType == FuncRec::Type::METHOD) ? (int)inArgNames.size() + 1 : (int)inArgNames.size();
							auto & funcLuanameToFuncGroupId =
								(funcType == FuncRec::Type::METHOD) ?
								classRec.methodFuncLuanameToFuncGroupId :
								classRec.staticFuncLuanameToFuncGroupId
								;
							if (funcLuanameToFuncGroupId.find(funcLuaname) == funcLuanameToFuncGroupId.end())
							{
								auto & funcGroupRec = FuncGroupRec::New(classRec.id);
								funcGroupRec.luaname = funcLuaname;
								funcLuanameToFuncGroupId[funcLuaname] = funcGroupRec.id;
							}
							auto & funcGroupRec = FuncGroupRec::Get(funcLuanameToFuncGroupId.at(funcLuaname));
							if (funcGroupRec.argCountToFuncId.find(luaArgCount) != funcGroupRec.argCountToFuncId.end())
								ThrowLeException(LeError::REGISTER_SAME_NAME, "Cannot register the same pair of function name and argument count twice.", funcLuaname);

							// Creates a new function record.
							auto & funcRec = FuncRec::New(funcGroupRec.id);
							funcRec.type = funcType;
							funcRec.funcCname = funcCname;
							funcRec.funcLuaname = funcLuaname;
							funcRec.argNames = argNames;
							funcRec.argNameToCtype = argNameToCtype;
							funcRec.argNameToRawCtype = argNameToRawCtype;
// 21.05.19 Fukushiro M. 1行追加 ()
							funcRec.autoDelArgNames = xmlCommentAutoDelArgNames;
							funcRec.argNameToSummary = xmlCommentArgNameToSummary;
							funcRec.inArgNames = inArgNames;
							funcRec.outArgNames = outArgNames;
							funcRec.inArgNameToLuaToCppConversionPath = inArgNameToLuaToCppConversionPath;
							funcRec.outArgNameToCppToLuaConversionPath = outArgNameToCppToLuaConversionPath;
							funcRec.exceptions = xmlCommentExceptions;
							funcGroupRec.argCountToFuncId[luaArgCount] = funcRec.id;
							funcRec.summary = xmlCommentSummary;
							break;
						}
					}
					{
						std::string nextClassName;
						if (CheckClass(Dummy<ClassRec::Type>(), nextClassName, Dummy<std::string>(), text, xmlCommentClassType))
						{
							if (IsContinuousBlock(readBufferedFile))
							{	//----- if block description is following -----

								auto memberClassId = classRec.FindClass(nextClassName);
								auto & memberClassRec = ClassRec::Get(memberClassId);
								memberClassRec.summary = xmlCommentSummary;

								// Parses source until next encounter with '}'.
								ParseSource2(readBufferedFile, memberClassRec);
							}
							break;
						}
					}
					{
						auto nextEnumType = EnumRec::Type::NONE;
						std::string nextEnumName;
						if (CheckEnum(nextEnumType, nextEnumName, text, xmlCommentClassType))
						{
							if (IsContinuousBlock(readBufferedFile))
							{	//----- if block description is following -----
								std::unordered_map<std::string, std::string> luanameToCname;
								std::unordered_map<std::string, std::string> luanameToSummary;
								ParseEnumContent(readBufferedFile, luanameToCname, luanameToSummary);
								auto & enumRec = EnumRec::New(classRec.id);
								enumRec.enumType = nextEnumType;
								enumRec.enumCname = nextEnumName;
								enumRec.enumLuaname = xmlCommentLuaname.empty() ? nextEnumName : xmlCommentLuaname;
								enumRec.summary = xmlCommentSummary;
								enumRec.luanameToCname = luanameToCname;
								enumRec.luanameToSummary = luanameToSummary;
								if (nextEnumType == EnumRec::Type::REGULAR)
									classRec.luanameToRegularEnumId[enumRec.enumLuaname] = enumRec.id;
								else
									classRec.luanameToClassEnumId[enumRec.enumLuaname] = enumRec.id;
								RegisterEnumConverter(enumRec);
							}
							break;
						}
					}
					{
						ConstantRec::Type constantRecType;
						std::string constantRawCtype;
						std::string constantName;
						std::vector<std::string> cToLuaConversionPath;
						if (CheckConstant(
							constantRecType,
							constantRawCtype,
							constantName,
							cToLuaConversionPath,
							classRec,
							text,
							xmlCommentClassType,
							xmlCommentCtype,
							xmlCommentLuatype
						))
						{
							auto & constantRec = ConstantRec::New(classRec.id);
							constantRec.recType = constantRecType;
							constantRec.constantRawCtype = constantRawCtype;
							constantRec.constantCname = constantName;
							constantRec.constantLuaname = xmlCommentLuaname.empty() ? constantName : xmlCommentLuaname;
							constantRec.summary = xmlCommentSummary;
							constantRec.cToLuaConversionPath = cToLuaConversionPath;
							classRec.constantLuanameToConstantId[constantRec.constantLuaname] = constantRec.id;
							break;
						}
					}

					{
						std::string variableRawCtype;
						std::string variableName;
						std::vector<std::string> cToLuaConversionPath;
						std::vector<std::string> luaToCConversionPath;
						if (CheckVariable(
							variableRawCtype,
							variableName,
							cToLuaConversionPath,
							luaToCConversionPath,
							classRec,
							text,
							xmlCommentClassType,
							xmlCommentCtype,
							xmlCommentLuatype
						))
						{
							if (classRec.classType != ClassRec::Type::STRUCT)
								ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "Variables only structure's member can be exported.");
							auto & variableRec = VariableRec::New(classRec.id);
							variableRec.variableRawCtype = variableRawCtype;
							variableRec.variableCname = variableName;
							variableRec.variableLuaname = xmlCommentLuaname.empty() ? variableName : xmlCommentLuaname;
							variableRec.summary = xmlCommentSummary;
							variableRec.cToLuaConversionPath = cToLuaConversionPath;
							variableRec.luaToCConversionPath = luaToCConversionPath;
							classRec.variableLuanameToVariableId[variableRec.variableLuaname] = variableRec.id;
							break;
						}
					}
					ThrowLeException(LeError::NOT_SUPPORTED, text);
					break;
				}
			}
			xmlCommentClear();
		}
	}
}

/// <summary>
/// Creates a FuncRec for the default constructor and register it to the ClassRec.
/// If any constructor is specified by "<stick export="true">, this function does not create the default constructor.
/// </summary>
/// <param name="classRec">The class record.</param>
static void CreateDefaultConstructors(ClassRec & classRec)
{
	if (classRec.classType == ClassRec::Type::CLASS)
	{
		bool isConstructorExist = false;
		for (const auto & nameFuncGroupId : classRec.staticFuncLuanameToFuncGroupId)
		{
			const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
			for (const auto & argCountFuncId : funcGroupRec.argCountToFuncId)
			{
				const auto & funcRec = FuncRec::Get(argCountFuncId.second);
				if (funcRec.type == FuncRec::Type::CONSTRUCTOR)
				{
					isConstructorExist = true;
					break;
				}
				if (isConstructorExist) break;
			}
			if (isConstructorExist) break;
		}

		// check any constructor exist or not. if exists, do not output default constructor.
		if (!isConstructorExist)
		{
			// add default constructor.
			if (classRec.staticFuncLuanameToFuncGroupId.find("New") == classRec.staticFuncLuanameToFuncGroupId.end())
			{
				auto & funcGroupRec = FuncGroupRec::New(classRec.id);
				funcGroupRec.luaname = "New";
				classRec.staticFuncLuanameToFuncGroupId["New"] = funcGroupRec.id;
			}
			auto & funcGroupRec = FuncGroupRec::Get(classRec.staticFuncLuanameToFuncGroupId.at("New"));
			if (funcGroupRec.argCountToFuncId.find(0) != funcGroupRec.argCountToFuncId.end())
				ThrowLeException(LeError::REGISTER_SAME_NAME, "Cannot register the same pair of function name and argument count twice.", "New");
			auto & funcRec = FuncRec::New(funcGroupRec.id);
			funcRec.type = FuncRec::Type::CONSTRUCTOR;
			funcRec.funcCname = classRec.classCname;
			funcRec.funcLuaname = "New";
			funcRec.summary = "Default constructor.";
			std::string funcCtype;
			NormalizeVarType(funcCtype, Dummy<std::string>(), classRec.classCname, classRec);
			if (funcRec.argNameToCtype.find("__lstickvar_ret") == funcRec.argNameToCtype.end())
				funcRec.argNameToCtype["__lstickvar_ret"] = funcCtype + "*";
			if (funcRec.argNameToRawCtype.find("__lstickvar_ret") == funcRec.argNameToRawCtype.end())
				funcRec.argNameToRawCtype["__lstickvar_ret"] = funcCtype + "*";
			if (funcRec.outArgNameToCppToLuaConversionPath.find("__lstickvar_ret") == funcRec.outArgNameToCppToLuaConversionPath.end())
			{
				std::vector<std::string> successPath;
				GetCppToLuaConverter(
					CppToLuaConversionType::FUNC_RETURN,
					funcCtype + "*",
// テスト。
//					LuaType("classobject"),
					LuaType::NIL,
					successPath
				);
				funcRec.outArgNameToCppToLuaConversionPath["__lstickvar_ret"] = successPath;
			}
//----- 19.12.11 Fukushiro M. 削除始 ()-----
//			if (funcRec.argNameToLuatype.find("__lstickvar_ret") == funcRec.argNameToLuatype.end())
//				funcRec.argNameToLuatype["__lstickvar_ret"] = LuaType("classobject");
//----- 19.12.11 Fukushiro M. 削除終 ()-----
			funcGroupRec.argCountToFuncId[0] = funcRec.id;
		}
	}
	for (const auto & classId : classRec.memberClassIdArray)
		CreateDefaultConstructors(ClassRec::Get(classId));
} // CreateDefaultConstructors.

/// <summary>
/// When polymorphic function exists, this function outputs the distributing wrapper function.
/// </summary>
/// <param name="classRec">The class record.</param>
/// <param name="funcRec">The function record.</param>
/// <param name="funcStrType">Type of the function.</param>
static void OutputPolymorphicFunction(const FuncGroupRec & funcGroupRec)
{
	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
/// <summary>
/// ${funcGroupRec.GetFullpathLuaname()}(...)
/// Distribute the process to the polymorphic functions. ${funcGroupRec.GetFullpathLuaname()}(...)
/// </summary>
static int ${funcGroupRec.GetWrapperFunctionName()}(lua_State* L)
{
	switch (lua_gettop(L))
	{

)", funcGroupRec.GetFullpathLuaname(), funcGroupRec.GetWrapperFunctionName());

	for (const auto & argCountFuncId : funcGroupRec.argCountToFuncId)
	{
		const auto argCount = argCountFuncId.first;
		const auto & funcRec = FuncRec::Get(argCountFuncId.second);
// 21.05.23 Fukushiro M. 1行削除 ()
//		const auto argCount = funcRec.inArgNames.size();
		OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
	case ${argCount}:
		// ${SRCMARKER}
		return ${funcRec.GetWrapperFunctionName()}(L);

)", argCount, funcRec.GetWrapperFunctionName());
	}
	OUTPUT_EXPORTFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	default:
		luaL_error(L, "incorrect argument");
		return 0;
	}
}

)");
}

/// <summary>
/// When polymorphic function exists, this function outputs the distributing wrapper function.
/// </summary>
/// <param name="classRec">The class record.</param>
static void OutputPolymorphicFunctions(const ClassRec & classRec)
{
	for (const auto & funcLuanameToFuncGroupId : { classRec.methodFuncLuanameToFuncGroupId, classRec.staticFuncLuanameToFuncGroupId })
	{
		for (const auto & nameFuncGroupId : funcLuanameToFuncGroupId)
		{
			const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
			if (funcGroupRec.argCountToFuncId.size() != 1)
				OutputPolymorphicFunction(funcGroupRec);
		}
	}
	for (const auto & classId : classRec.memberClassIdArray)
		OutputPolymorphicFunctions(ClassRec::Get(classId));
}

static void OutputClassPushArgFunc(const ClassRec & classRec)
{
	const auto pushFunc = LuaTypeToSetFuncName(LuaType("classobject"));
	OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(
/// <summary>
/// Push the class object into Lua stack.
/// ${SRCMARKER}
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ${classRec.GetFullpathCname()}&& head, Args&& ... args)
{
	${pushFunc}(L, false, &head, "${classRec.GetUniqueClassName()}");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the class object into Lua stack.
/// ${SRCMARKER}
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ${classRec.GetFullpathCname()}*&& head, Args&& ... args)
{
	${pushFunc}(L, false, head, "${classRec.GetUniqueClassName()}");
	return stick_pusharg(L, data, std::move(args)...) + 1;
}


)", pushFunc, classRec.GetFullpathCname(), classRec.GetUniqueClassName());
} // static void OutputClassPushArgFunc(const ClassRec & classRec).


static void OutputStructPushArgFunc(const ClassRec & classRec)
{
	const auto fullLuaName = classRec.GetFullpathLuaname();

	const auto pushFunc = LuaTypeToSetFuncName(LuaType(fullLuaName));
	OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(
template<>
void ${pushFunc}<${classRec.GetFullpathCname()}>(lua_State * L, ${classRec.GetFullpathCname()} const & value);

/// <summary>
/// Push the struct data into Lua stack.
/// ${SRCMARKER}
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ${classRec.GetFullpathCname()}&& head, Args&& ... args)
{
	${pushFunc}(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the struct data into Lua stack.
/// ${SRCMARKER}
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ${classRec.GetFullpathCname()}&& head, Args&& ... args)
{
	${pushFunc}(L, head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}


)", pushFunc, classRec.GetFullpathCname());
} // static void OutputStructPushArgFunc(const ClassRec & classRec).

static void OutputEnumPushArgFunc(const EnumRec & enumRec)
{
	if (!enumRec.enumCname.empty())
	{
		const auto pushFunc = LuaTypeToSetFuncName(LuaType("integer"));
		OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(
/// <summary>
/// Push the enum data into Lua stack.
/// ${SRCMARKER}
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, ${enumRec.GetFullpathCname()}&& head, Args&& ... args)
{
	${pushFunc}(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}

/// <summary>
/// Push the enum data into Lua stack.
/// ${SRCMARKER}
/// </summary>
/// <param name="L">The l.</param>
/// <param name="data">The data.</param>
/// <returns>Number of arguments</returns>
template<typename ... Args>
int stick_pusharg(lua_State* L, void* data, const ${enumRec.GetFullpathCname()}&& head, Args&& ... args)
{
	${pushFunc}(L, (__int64)head);
	return stick_pusharg(L, data, std::move(args)...) + 1;
}


)", pushFunc, enumRec.GetFullpathCname());
	}
} // OutputEnumPushArgFunc.

static void OutputClassPushArgFuncs(const ClassRec & classRec)
{
	if (classRec.classType == ClassRec::Type::CLASS || classRec.classType == ClassRec::Type::INCONSTRUCTIBLE)
		OutputClassPushArgFunc(classRec);
	else if (classRec.classType == ClassRec::Type::STRUCT)
		OutputStructPushArgFunc(classRec);
	for (const auto & luanameToEnumId : { classRec.luanameToRegularEnumId, classRec.luanameToClassEnumId })
	{
		for (const auto & luanameEnumId : luanameToEnumId)
		{
			const auto & enumRec = EnumRec::Get(luanameEnumId.second);
			OutputEnumPushArgFunc(enumRec);
		}
	}

	for (const auto & classId : classRec.memberClassIdArray)
		OutputClassPushArgFuncs(ClassRec::Get(classId));
} // static void OutputClassPushArgFuncs(const ClassRec & classRec).

/// <summary>
/// Outputs the class registration source code for luastick_init.
/// e.g. If classRec=class B, this function outputs *1 ~ *2.
/// class A {
///   class B {     <-- *1
///     class C {
///       :
///     }
///   }             <-- *2
/// }
/// </summary>
/// <param name="classRec">The class record.</param>
static void OutputStickInitCpp(const ClassRec & classRec)
{
	if (
		classRec.classType == ClassRec::Type::STRUCT &&
		classRec.staticFuncLuanameToFuncGroupId.empty() &&
		classRec.luanameToRegularEnumId.empty() &&
		classRec.luanameToClassEnumId.empty() &&
		classRec.constantLuanameToConstantId.empty() &&
		classRec.methodFuncLuanameToFuncGroupId.empty() &&
		classRec.memberClassIdArray.empty()
		)
		return;

	const std::string uniqueClassName = classRec.GetUniqueClassName();
	const std::string topComment = classRec.classLuaname.empty() ?
		"Register the global functions." :
		UtilString::Format("Register the static class '%s' and its static member functions.", classRec.GetFullpathCname().c_str());

	// Outputs like the following source code. It describes to register the static class 'B'.
	//-------------------------------------
	// Sticklib::push_table(L, "B");
	// static struct luaL_Reg lm__B__Static[] =
	// {
	//	{ "MyFunc1", lm__MyFunc1__2 },
	//	{ "MyFunc2", lm__MyFunc2__2 },
	//	{ nullptr, nullptr },
	// };
	// Sticklib::set_functions(L, lm__B__Static);
	//-------------------------------------
	OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	// ${topComment}
	Sticklib::push_table(L, "${classRec.classLuaname}");
	{
		static struct luaL_Reg ${uniqueClassName}Static[] =
		{

)", topComment, classRec.classLuaname, uniqueClassName);

	for (const auto & nameFuncGroupId : classRec.staticFuncLuanameToFuncGroupId)
	{
		const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
		std::string wrapperFunCname;
		if (funcGroupRec.argCountToFuncId.size() == 1)
		{
			const auto & funcRec = FuncRec::Get(funcGroupRec.argCountToFuncId.begin()->second);
			wrapperFunCname = funcRec.GetWrapperFunctionName();
		}
		else
		{
			wrapperFunCname = funcGroupRec.GetWrapperFunctionName();
		}
		OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
			// ${SRCMARKER}
			{ "${funcGroupRec.luaname}", ${wrapperFunCname} },

)", funcGroupRec.luaname, wrapperFunCname);
	}

	OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
			// ${SRCMARKER}
			{ nullptr, nullptr },
		};
		Sticklib::set_functions(L, ${uniqueClassName}Static);
	}


)", uniqueClassName);

	for (const auto & luanameToEnumId : { classRec.luanameToRegularEnumId, classRec.luanameToClassEnumId })
	{
		if (!luanameToEnumId.empty())
		{
			// Outputs like the following source code.
			//-------------------------------------
			// // Register the enumeration in the static class '::X'
			// Sticklib::push_table(L, "Enum1");
			// Sticklib::set_lvalue_to_table(L, "A", (lua_Integer)::X::Enum1::A);
			// Sticklib::set_lvalue_to_table(L, "B", (lua_Integer)::X::Enum1::B);
			// Sticklib::pop(L);

			OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// Register the enumeration in the static class '${classRec.GetFullpathCname()}'

)", classRec.GetFullpathCname());

			for (const auto & luanameEnumId : luanameToEnumId)
			{
				const auto & enumRec = EnumRec::Get(luanameEnumId.second);
				if (enumRec.enumType == EnumRec::Type::ENUM_CLASS)
				{
					OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	Sticklib::push_table(L, "${enumRec.enumLuaname}");

)", enumRec.enumLuaname);
				}
				for (const auto & lname_cname : enumRec.luanameToCname)
				{
					OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	Sticklib::set_lvalue_to_table(L, "${lname_cname.first}", (lua_Integer)${enumRec.GetFullpathElementCprefix()}${lname_cname.second});

)", lname_cname.first, lname_cname.second, enumRec.GetFullpathElementCprefix());
				}
				if (enumRec.enumType == EnumRec::Type::ENUM_CLASS)
				{
					OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	Sticklib::pop(L);


)");
				}
				else
				{
					OUTPUT_INITFUNC_STREAM << u8"\n";
				}
			}
		}
	}

	if (!classRec.constantLuanameToConstantId.empty())
	{
		// Outputs like the following source code.
		//-------------------------------------
		// // Register the constants in the static class '::X::A'
		// {
		// 	Sticklib::l_string __lstickvar_r1;
		// 	Sticklib::atext_to_lstring(__lstickvar_r1, ::X::A::ABC);
		// 	Sticklib::setstring(L, "ABC", __lstickvar_r1);
		// 	Sticklib::l_integer __lstickvar_r2;
		// 	Sticklib::intT_to_linteger<int>(__lstickvar_r2, ::X::A::EFG);
		// 	Sticklib::set_lvalue_to_table(L, "EFG", __lstickvar_r2);
		// }

		int luaArgNumber = 1;
		OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	// Register the constants in the static class '${classRec.GetFullpathCname()}'
	{

)", classRec.GetFullpathCname());

		int argMajorNumber = 0;
		for (const auto & constantAndId : classRec.constantLuanameToConstantId)
		{
			argMajorNumber++;
			int argMinorNumber = 1;
			const auto & constantRec = ConstantRec::Get(constantAndId.second);
			const auto & conversionPath = constantRec.cToLuaConversionPath;

			// conversionPath={ A*& -> * -> A& -> A_to_str -> std::string }
			// ------------------------
			// A*& a = (A*&)Const;
			// A& b = *a;
			// std::string c;
			// A_to_str(c, b);
			// ------------------------

			auto varCtype = conversionPath.front();
			auto tmpVarName = UtilString::Format("_var_%d_%d", argMajorNumber, argMinorNumber);
			OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${tmpVarName} = (${varCtype})${constantRec.GetFullpathCname()};

)", varCtype, tmpVarName, constantRec.GetFullpathCname());
			argMinorNumber++;

			for (size_t i = 1; i != conversionPath.size(); i += 2)
			{
				const auto & convFunc = conversionPath.at(i);		// Converter.
				const auto & varCtype = conversionPath.at(i + 1);	// Type of variable.
				auto nextTmpVarName = UtilString::Format("_var_%d_%d", argMajorNumber, argMinorNumber);
				if (convFunc == "*" || convFunc == "&")
				{
					OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${nextTmpVarName} = ${convFunc}${tmpVarName};

)", varCtype, nextTmpVarName, convFunc, tmpVarName);
				}
				else
				{
					OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		${varCtype} ${nextTmpVarName};
		${convFunc}(${nextTmpVarName}, ${tmpVarName});

)", varCtype, nextTmpVarName, convFunc, tmpVarName);
				}
				tmpVarName = nextTmpVarName;
				argMinorNumber++;
			}
//----- 21.05.24 Fukushiro M. 追加始 ()-----
// テスト。
			// "::TestClass0*". Do not remove '*'. 
			auto fullpathCtypeAst = UtilString::Replace(conversionPath.back(), "&", "");
			auto varLuaType = CtypeToLuaType(fullpathCtypeAst);
			if (varLuaType != LuaType::NIL && varLuaType.CompareHead(LuaType("classobject(")) == 0)
			{	//----- if class object is defined as static constant -----
				auto fullpathCtype = UtilString::Replace(fullpathCtypeAst, "*", "");
				const std::string uniqueClassName = ClassRec::FullpathNameToUniqueName(fullpathCtype);
				OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		Sticklib::set_classobject_to_table<${fullpathCtype}>(L, "${constantRec.constantLuaname}", false, ${tmpVarName}, "${uniqueClassName}");

)", fullpathCtype, constantRec.constantLuaname, tmpVarName, uniqueClassName);
			}
			else if (varLuaType != LuaType::NIL && varLuaType.CompareHead(LuaType("array<classobject>(")) == 0)
			{	//----- if class object is defined as static constant -----
				auto fullpathCtype = UtilString::Replace(fullpathCtypeAst, "*", "");
				const std::string uniqueClassName = ClassRec::FullpathNameToUniqueName(fullpathCtype);
				OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		Sticklib::set_classobjectarray_to_table<${fullpathCtype}>(L, "${constantRec.constantLuaname}", false, ${tmpVarName}, "${uniqueClassName}");

)", fullpathCtype, constantRec.constantLuaname, tmpVarName, uniqueClassName);
			}
			else

// テスト。以下のif (conversionPath.back() == "Sticklib::classobject")ブロックは削除すべき。
//----- 21.05.24 Fukushiro M. 追加終 ()-----


			if (conversionPath.back() == "Sticklib::classobject")
			{	//----- if class object is defined as static constant -----
				// Get the class type of the class object.
				auto preClassType = conversionPath[conversionPath.size() - 3];
				preClassType = UtilString::Replace(preClassType, "*", "", "&", "");
				// Get the unique name (like "lm__TestClass0__") of the class.
				auto preClassId = ClassRec::Get(0).FindClass(preClassType);
				const std::string uniqueClassName = ClassRec::Get(preClassId).GetUniqueClassName();

				OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		Sticklib::set_classobject_to_table(L, "${constantRec.constantLuaname}", false, ${tmpVarName}, "${uniqueClassName}");

)", constantRec.constantLuaname, tmpVarName, uniqueClassName);
			}
			else
			{
				OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		Sticklib::set_lvalue_to_table(L, "${constantRec.constantLuaname}", ${tmpVarName});

)", constantRec.constantLuaname, tmpVarName);
			}
		}
		OUTPUT_INITFUNC_STREAM << u8"	}\n";
	}

	// Outputs like the following, except constructors.
	// --- source ---------------------------------------
	// class A : public B {
	//     A();
	//     void Func1(int a, int b);
	//     void Func2(int a);
	// };
	// --- generated ------------------------------------
	// static struct luaL_Reg __A__[] =
	// {
	// 	{ "__gc", __A__Destructor },    <---- ClassRec::Type::CLASS only.
	// 	{ "Func1", __A__Func1__2 },
	// 	{ "Func2", __A__Func2__1 },
	// 	{ nullptr, nullptr },
	// };

	if (classRec.classType == ClassRec::Type::CLASS || classRec.classType == ClassRec::Type::INCONSTRUCTIBLE)
	{	//----- if regular class -----
		OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	// Register the regular class '${classRec.GetFullpathCname()}' and its regular member functions.
	static struct luaL_Reg ${uniqueClassName}Method[] =
	{

)", classRec.GetFullpathCname(), uniqueClassName);

		if (classRec.classType == ClassRec::Type::CLASS)
		{	//----- if regular class -----

			OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		{ "__gc", ${classRec.GetWrapperDestructorName()} },

)", classRec.GetWrapperDestructorName());
		}

		for (const auto & nameFuncGroupId : classRec.methodFuncLuanameToFuncGroupId)
		{
			const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
			std::string wrapperFunCname;
			if (funcGroupRec.argCountToFuncId.size() == 1)
			{
				const auto & funcRec = FuncRec::Get(funcGroupRec.argCountToFuncId.begin()->second);
				wrapperFunCname = funcRec.GetWrapperFunctionName();
			}
			else
			{
				wrapperFunCname = funcGroupRec.GetWrapperFunctionName();
			}
			OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		// ${SRCMARKER}
		{ "${funcGroupRec.luaname}", ${wrapperFunCname} },

)", funcGroupRec.luaname, wrapperFunCname);
		}
		OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
		{ nullptr, nullptr },
	};

)");

		// Outputs like the following.
		// --- generated ------------------------------------------
		// Sticklib::register_class(L, "A", "__A__", __A__, "__B__");
		if (classRec.superClassId == -1)
		{
			OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	Sticklib::register_class(L, "${uniqueClassName}", ${uniqueClassName}Method, nullptr);


)", uniqueClassName);
		}
		else
		{
			OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	Sticklib::register_class(L, "${uniqueClassName}", ${uniqueClassName}Method, "${ClassRec::Get(classRec.superClassId).GetUniqueClassName()}");


)", uniqueClassName, ClassRec::Get(classRec.superClassId).GetUniqueClassName());
		}
	}

	for (const auto & classId : classRec.memberClassIdArray)
	{
		const auto & subcm = ClassRec::Get(classId);
		OutputStickInitCpp(subcm);
	}
	OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
	// ${SRCMARKER}
	Sticklib::pop(L);


)");
} // static void OutputStickInitCpp(const ClassRec & classRec).

static void OutputModuleFuncHtml(StringBuffer & buffer, const FuncRec & funcRec)
{
	// Lua output argument list. e.g. "integer a, string b = ";
	std::string outLuaArgList;
	// Lua input argument list. e.g. "integer c, string d";
	std::string inLuaArgList;

	// Argument name list. Copy to insert the return value '__lstickvar_ret'.
	auto argNames = funcRec.argNames;
	if (funcRec.outArgNames.find("__lstickvar_ret") != funcRec.outArgNames.end())
		argNames.insert(argNames.begin(), "__lstickvar_ret");

	for (const auto & argName : argNames)
	{
		auto luaTypeName = funcRec.ArgNameToLuaType(argName).ToString();
		auto idPos = luaTypeName.find('(');
		if (idPos != std::string::npos)
			luaTypeName = luaTypeName.substr(idPos);
		if (funcRec.outArgNames.find(argName) != funcRec.outArgNames.end())
		{
			if (!outLuaArgList.empty())
				outLuaArgList += ", ";
			outLuaArgList += luaTypeName + " " + argName;
		}
		if (funcRec.inArgNameToLuaToCppConversionPath.find(argName) != funcRec.inArgNameToLuaToCppConversionPath.end())
		{
			if (!inLuaArgList.empty())
				inLuaArgList += ", ";
			inLuaArgList += luaTypeName + " " + argName;
		}
	}
	if (outLuaArgList.empty())
	{
		if (funcRec.type == FuncRec::Type::CONSTRUCTOR)
			outLuaArgList = "classobject return_value = ";
	}
	else
	{
		outLuaArgList = UtilString::Replace(outLuaArgList, "__lstickvar_ret", "return_value");
		outLuaArgList += " = ";
	}

	// C++ output argument. e.g. "int a = ", "void "
	std::string returnRawCtype;
	// C++ input argument list. e.g. "int c, const char* d";
	std::string inRawArgList;
	for (const auto & argName : funcRec.argNames)
	{
		const auto rawCtype = funcRec.argNameToRawCtype.at(argName);
		if (!inRawArgList.empty())
			inRawArgList += ", ";
		inRawArgList += rawCtype + " " + argName;
	}
	if (funcRec.outArgNames.find("__lstickvar_ret") != funcRec.outArgNames.end())
		returnRawCtype = funcRec.argNameToRawCtype.at("__lstickvar_ret") + " ";
	else
		returnRawCtype = "void ";

	const auto divClass = (funcRec.type == FuncRec::Type::METHOD) ? "class-method" : "module-function";
	const auto funcType = (funcRec.type == FuncRec::Type::METHOD) ? "method" : "function";
	buffer << FORMHTML(u8R"(
	<!-- ${SRCMARKER} -->
	<div class="${divClass}">
		<h3 class="element-name">${funcRec.GetFullpathLuaname()}<span class="element-name-additional"> ${funcType}</span></h3>
		<p class="summary">${funcRec.summary}</p>

)", divClass, funcRec.GetFullpathLuaname(), funcType, funcRec.summary);

	buffer << FORMHTML(u8R"(
	<!-- ${SRCMARKER} -->
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>${outLuaArgList}${funcRec.GetFullpathLuanameForCall()}(${inLuaArgList})</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>${returnRawCtype}${funcRec.GetFullpathCname()}(${inRawArgList})</code></pre>
	</div>

)", outLuaArgList, funcRec.GetFullpathLuanameForCall(), inLuaArgList, returnRawCtype, funcRec.GetFullpathCname(), inRawArgList);

	if (!funcRec.argNameToSummary.empty())
	{
		buffer << FORMHTML(u8R"(
	<!-- ${SRCMARKER} -->
	<p class="element-member-title">Parameters</p>
	<dl class="element-member">

)");

//----- 21.05.19 Fukushiro M. 変更前 ()-----
//		for (const auto variableName_Id : funcRec.argNameToSummary)
//		{
//			auto argName = (variableName_Id.first == "__lstickvar_ret") ? std::string("return_value") : variableName_Id.first;
//			auto summary = variableName_Id.second;
//			auto luaTypeName = funcRec.ArgNameToLuaType(variableName_Id.first).ToString();
//			buffer << FORMHTML(u8R"(
//		<dt>${argName}</dt>
//		<dd>${luaTypeName} type: ${summary}</dd>
//
//)", argName, luaTypeName, summary);
//		}
//----- 21.05.19 Fukushiro M. 変更後 ()-----
		std::vector<std::string> orderedArgName;
		for (const auto & argName : argNames)
		{
			if (funcRec.outArgNames.find(argName) != funcRec.outArgNames.end())
				orderedArgName.emplace_back(argName);
		}
		for (const auto & argName : argNames)
		{
			if (funcRec.inArgNames.find(argName) != funcRec.inArgNames.end())
				orderedArgName.emplace_back(argName);
		}
		for (const auto & argName  : orderedArgName)
		{
			auto variableName_Id = funcRec.argNameToSummary.find(argName);
			if (variableName_Id != funcRec.argNameToSummary.end())
			{
				auto variableName = (variableName_Id->first == "__lstickvar_ret") ? std::string("return_value") : variableName_Id->first;
				auto summary = variableName_Id->second;
				auto luaTypeName = funcRec.ArgNameToLuaType(variableName_Id->first).ToString();
				buffer << FORMHTML(u8R"(
		<dt>${variableName}</dt>
		<dd>${luaTypeName} type: ${summary}</dd>

)", variableName, luaTypeName, summary);
			}
		}
//----- 21.05.19 Fukushiro M. 変更終 ()-----


		buffer << FORMHTML(u8R"(
	</dl>

)");
	}
} // OutputModuleFuncHtml

static void OutputModuleVariableHtml(StringBuffer & buffer, const ConstantRec & constRec)
{
	std::string returnLuatype = constRec.GetLuaType().ToString() + " ";
	std::string returnCtype;
	switch (constRec.recType)
	{
	case ConstantRec::Type::CONSTANT:
		returnCtype = std::string("const ") + constRec.constantRawCtype + " ";
		break;
	case ConstantRec::Type::CONSTEXPR:
		returnCtype = std::string("constexpr ") + constRec.constantRawCtype + " ";
		break;
	case ConstantRec::Type::DEFINE:
		returnCtype = "#define ";
		break;
	default:
		ThrowLeSystemError();
	}
	buffer << FORMHTML(u8R"(
	<!-- ${SRCMARKER} -->
	<div class="module-variable">
		<h3 class="element-name">${constRec.GetFullpathLuaname()}<span class="element-name-additional"> constant</span></h3>
		<p class="summary">${constRec.summary}</p>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>${returnLuatype}${constRec.GetFullpathLuaname()}</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>${returnCtype}${constRec.GetFullpathCname()}</code></pre>
	</div>

)", constRec.GetFullpathLuaname(), constRec.summary, returnLuatype, returnCtype, constRec.GetFullpathCname());
}

static void OutputModuleEnumHtml(StringBuffer & buffer, const EnumRec & enumRec)
{
	std::string returnCtype;
	switch (enumRec.enumType)
	{
	case EnumRec::Type::ENUM_CLASS:
		returnCtype = std::string("enum class ") + enumRec.GetFullpathCname();
		break;
	case EnumRec::Type::REGULAR:
		returnCtype = std::string("enum ") + enumRec.GetFullpathCname();
		break;
	default:
		ThrowLeSystemError();
	}
	for (const auto & lname_cname : enumRec.luanameToCname)
	{
		const auto luaname = lname_cname.first;
		const auto cname = lname_cname.second;
		const auto summary = (enumRec.luanameToSummary.find(luaname) != enumRec.luanameToSummary.end()) ?
			enumRec.luanameToSummary.at(luaname) : std::string();
		buffer << FORMHTML(u8R"(
	<!-- ${SRCMARKER} -->
	<div class="module-variable">
		<h3 class="element-name">${enumRec.GetFullpathElementLuaprefix()}${luaname}<span class="element-name-additional"> enumerated constant</span></h3>
		<p class="summary">${summary}</p>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>integer ${enumRec.GetFullpathElementLuaprefix()}${luaname}</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>${returnCtype} { ${cname} }</code></pre>
	</div>

)", enumRec.GetFullpathElementLuaprefix(), luaname, cname, summary, returnCtype);
	}
} // OutputModuleEnumHtml.

static void OutputStructLuaElementHtml(StringBuffer & buffer, const ClassRec & classRec, std::string spaceCode)
{
	if (classRec.superClassId != -1)
		OutputStructLuaElementHtml(buffer, ClassRec::Get(classRec.superClassId), spaceCode);

	for (const auto & variableName_Id : classRec.variableLuanameToVariableId)
	{
		const auto & variableRec = VariableRec::Get(variableName_Id.second);
		const auto luaTypeName = variableRec.GetLuaType().ToString();

		std::string luaValueText;
		static const std::regex ARRAY_DEF(R"(^array<(\w+)>$)");
		static const std::regex HASH_DEF(R"(^hash<(\w+),(\w+)>$)");
		std::smatch results;
		if (
			luaTypeName == "boolean" ||
			luaTypeName == "integer" ||
			luaTypeName == "number" ||
			luaTypeName == "string" ||
			luaTypeName == "lightuserdata"
		)
		{
			// e.g. [boolean value]
			luaValueText = std::string("[") + luaTypeName + " value]";
		}
		else if (luaTypeName == "classobject")
		{
			// e.g. [ClassA.ClassB value]
			// Get the class type of the class object.
			auto preClassType = variableRec.cToLuaConversionPath[variableRec.cToLuaConversionPath.size() - 3];
			preClassType = UtilString::Replace(preClassType, "*", "", "&", "");
			auto preClassId = ClassRec::Get(0).FindClass(preClassType);
			luaValueText = std::string("[") + ClassRec::Get(preClassId).GetFullpathLuaname() + " classobject]";
		}
		else if (std::regex_search(luaTypeName, results, ARRAY_DEF))
		{
			// e.g. { [integer value], [integer value], ... }
			luaValueText = std::string("{ [") + results[1].str() + " value], [" + results[1].str() + " value], ... }";
		}
		else if (std::regex_search(luaTypeName, results, HASH_DEF))
		{
			// e.g. { [key] = [integer value], [key] = [integer value], ... }
			luaValueText = std::string("{ [key] = [") + results[2].str() + " value], [key] = [" + results[2].str() + " value], ... }";
		}

		if (!luaValueText.empty())
		{
			// Output like following.
			//     type = [integer value],

			buffer << FORMHTML(u8R"(
${spaceCode}${variableRec.variableLuaname} = ${luaValueText},

)", spaceCode, variableRec.variableLuaname, luaValueText);
		}
		else
		{
			// Output like following.
			//    center = {
			//        x = [number value],       <--- Written by OutputStructLuaElementHtml calling.
			//        y = [number value],       <--- Written by OutputStructLuaElementHtml calling.
			//    }

			// Assume it is struct.
// テスト。
//			auto subClassId = ClassRec::Get(0).FindClass(UtilString::Replace(variableRec.cToLuaConversionPath.back(), "&", ""));
			auto subClassId = ClassRec::Get(0).FindClass(UtilString::Replace(UtilString::Replace(variableRec.cToLuaConversionPath.back(), "*", "", "&", "")));
			const auto & subClassRec = ClassRec::Get(subClassId);

			buffer << FORMHTML(u8R"(
${spaceCode}${variableRec.variableLuaname} = {

)", spaceCode, variableRec.variableLuaname);

			OutputStructLuaElementHtml(buffer, subClassRec, spaceCode + "    ");

			buffer << FORMHTML(u8R"(
${spaceCode}}

)", spaceCode);

		}
	}
} // OutputStructLuaElementHtml.

static void OutputStructHtml(StringBuffer & buffer, const ClassRec & classRec)
{
	const auto fullpathLuaname = classRec.GetFullpathLuaname();
	const auto fullpathCname = classRec.GetFullpathCname();

	// Outputs like the following.
	//----------------------------------------------------------------
	// <div class="structure1">
	//     <h2 class="elements-title">PolyJoint structure</h2>
	//     <div class="structure2">
	//         <h3 class="element-name">PolyJoint<span class="element-name-additional"> table</span></h3>
	//         <dl class="element-member">
	//             <dt>type</dt>
	//             <dd>タイプ</dd>
	//             <dt>x</dt>
	//             <dd>x座標</dd>
	//         </dl>
	//         <p class="element-lua-title">Lua</p>
	//         <pre class="element-lua-form"><code>table value = {
	//----------------------------------------------------------------
	buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
<div class="structure1">
	<h2 class="elements-title">${fullpathLuaname} structure</h2>
	<p class="summary">${classRec.summary}</p>
	<div class="structure2">
		<h3 class="element-name">${fullpathLuaname}<span class="element-name-additional"> table</span></h3>
		<p class="element-member-title">Member variables</p>
		<dl class="element-member">

)", fullpathLuaname, classRec.summary);

	for (const auto & variableName_Id : classRec.variableLuanameToVariableId)
	{
		const auto & variableRec = VariableRec::Get(variableName_Id.second);
		buffer << FORMHTML(u8R"(
			<dt>${variableRec.variableLuaname}</dt>
			<dd>${variableRec.GetLuaType().ToString()} type: ${variableRec.summary}</dd>

)", variableRec.variableLuaname, variableRec.GetLuaType().ToString(), variableRec.summary);
	}

	buffer << FORMHTML(u8R"(
		</dl>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>table value = {

)");

	OutputStructLuaElementHtml(buffer, classRec, "    ");

	buffer << FORMHTML(u8R"(
}</code></pre>

)");


	if (classRec.superClassId != -1)
	{
		const auto & superClassRec = ClassRec::Get(classRec.superClassId);

		buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>${fullpathCname} : public ${superClassRec.GetFullpathCname()} {

)", fullpathCname, superClassRec.GetFullpathCname());
	}
	else
	{
		buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>${fullpathCname} {

)", fullpathCname);
	}

	for (const auto & variableName_Id : classRec.variableLuanameToVariableId)
	{
		const auto & variableRec = VariableRec::Get(variableName_Id.second);
		buffer << FORMHTML(u8R"(
    ${variableRec.variableRawCtype} ${variableRec.variableCname};

)", variableRec.variableRawCtype, variableRec.variableCname);
	}

	buffer << FORMHTML(u8R"(
}</code></pre>
	</div>

)");

	buffer << FORMHTML(u8R"(
	<div class="structure2">
		<h3 class="element-name">array&lt;${fullpathLuaname}&gt;<span class="element-name-additional"> table</span></h3>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>table value = {
    { [${fullpathLuaname} table} },
    { [${fullpathLuaname} table} },
        :
}</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>std::vector&lt;${fullpathCname}&gt;</code></pre>
	</div>
	<div class="structure2">
		<h3 class="element-name">hash&lt;number,${fullpathLuaname}&gt;<span class="element-name-additional"> table</span></h3>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>table value = {
    { [number] = [${fullpathLuaname} table} },
    { [number] = [${fullpathLuaname} table} },
        :
}</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>std::unordered_map&lt;double,${fullpathCname}&gt;</code></pre>
	</div>
	<div class="structure2">
		<h3 class="element-name">hash&lt;string,${fullpathLuaname}&gt;<span class="element-name-additional"> table</span></h3>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>table value = {
    { [string] = [${fullpathLuaname} table} },
    { [string] = [${fullpathLuaname} table} },
        :
}</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>std::unordered_map&lt;std::string,${fullpathCname}&gt;</code></pre>
	</div>
	<div class="structure2">
		<h3 class="element-name">hash&lt;boolean,${fullpathLuaname}&gt;<span class="element-name-additional"> table</span></h3>
		<p class="element-lua-title">Lua</p>
		<pre class="element-lua-form"><code>table value = {
    { [boolean] = [${fullpathLuaname} table} },
        :
}</code></pre>
		<p class="element-cpp-title">C++</p>
		<pre class="element-cpp-form"><code>std::unordered_map&lt;bool,${fullpathCname}&gt;</code></pre>
	</div>

)", fullpathLuaname, fullpathCname);

	buffer << FORMHTML(u8R"(
</div>

)");
} // OutputStructHtml.

static void OutputModuleHtml(StringBuffer & buffer, const ClassRec & classRec)
{
	// e.g. "ABC module"
	const auto moduleTitle = (classRec.classType == ClassRec::Type::GLOBAL) ? "Global scope" : classRec.GetFullpathLuaname() + " module";
	// e.g. "ABC class"
	const auto classTitle = (classRec.classType == ClassRec::Type::GLOBAL) ? "Global scope" : classRec.GetFullpathLuaname() + " class";

//----- 20.01.22 Fukushiro M. 追加始 ()-----
	//----- Outputs module structures -----
	if (classRec.classType == ClassRec::Type::STRUCT)
	{
		OutputStructHtml(buffer, classRec);
	}
//----- 20.01.22 Fukushiro M. 追加終 ()-----

	//----- Outputs module functions -----
	if (!classRec.staticFuncLuanameToFuncGroupId.empty())
	{
		buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
<div class="module-functions">
	<h2 class="elements-title">${moduleTitle}</h2>
	<p class="summary">${classRec.summary}</p>

)", moduleTitle, classRec.summary);

		for (const auto & nameFuncGroupId : classRec.staticFuncLuanameToFuncGroupId)
		{
			const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
			for (const auto & argCountFuncId : funcGroupRec.argCountToFuncId)
			{
				const auto & funcRec = FuncRec::Get(argCountFuncId.second);
				OutputModuleFuncHtml(buffer, funcRec);
			}
		}
		buffer << u8"</div>\n";
	}

	//----- Outputs module constant values -----
	if (!classRec.constantLuanameToConstantId.empty() || !classRec.luanameToRegularEnumId.empty())
	{
		buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
<div class="module-variables">
	<h2 class="elements-title">${moduleTitle}</h2>

)", moduleTitle);

		//----- Outputs C++ constant as module variables -----
		if (!classRec.constantLuanameToConstantId.empty())
		{
			for (const auto & nameConstId : classRec.constantLuanameToConstantId)
			{
				const auto & constRec = ConstantRec::Get(nameConstId.second);
				OutputModuleVariableHtml(buffer, constRec);
			}
		}
		//----- Outputs C++ enumerations as module variables -----
		if (!classRec.luanameToRegularEnumId.empty())
		{
			for (const auto & nameEnumId : classRec.luanameToRegularEnumId)
			{
				const auto & enumRec = EnumRec::Get(nameEnumId.second);
				OutputModuleEnumHtml(buffer, enumRec);
			}
		}
		buffer << u8"</div>\n";
	}

	//----- Outputs class functions -----
	if (!classRec.methodFuncLuanameToFuncGroupId.empty())
	{
		buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
<div class="class-methods">
	<h2 class="elements-title">${classTitle}</h2>
	<p class="summary">${classRec.summary}</p>

)", classTitle, classRec.summary);

		for (const auto & nameFuncGroupId : classRec.methodFuncLuanameToFuncGroupId)
		{
			const auto & funcGroupRec = FuncGroupRec::Get(nameFuncGroupId.second);
			for (const auto & argCountFuncId : funcGroupRec.argCountToFuncId)
			{
				const auto & funcRec = FuncRec::Get(argCountFuncId.second);
				OutputModuleFuncHtml(buffer, funcRec);
			}
		}
		buffer << u8"</div>\n";
	}

	//----- Outputs C++ enumerations as module variables -----
	for (const auto & nameEnumId : classRec.luanameToClassEnumId)
	{
		const auto & enumRec = EnumRec::Get(nameEnumId.second);
		buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
<div class="module-variables">
	<h2 class="elements-title">${enumRec.GetFullpathLuaname()} module</h2>
	<p class="summary">${enumRec.summary}</p>

)", enumRec.GetFullpathLuaname(), enumRec.summary);

		OutputModuleEnumHtml(buffer, enumRec);
		buffer << u8"</div>\n";
	}

	//------------------------------------


	for (const auto & classId : classRec.memberClassIdArray)
	{
		const auto & subcr = ClassRec::Get(classId);
		OutputModuleHtml(buffer, subcr);
	}
} // OutputModuleHtml

static void OutputHtml(StringBuffer & buffer, const ClassRec & classRec, const std::string & filename)
{
	buffer << FORMHTML(u8R"(
<!-- ${SRCMARKER} -->
<!doctype html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>LuaStick Output API Manual</title>
<link rel="stylesheet" href="${filename}.css" type="text/css">
</head>
<body>
<h1 class="manual-title">LuaStick Output API Manual</h1>

)", filename);

	OutputModuleHtml(buffer, classRec);

	buffer << FORMHTML(u8R"(
</body>
</html>

)");
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	constexpr wchar_t STICKLIB[]{ L"Sticklib.h" };
	constexpr size_t STICKLIB_LEN = _countof(STICKLIB) - 1;
	constexpr wchar_t STICKRUN[]{ L"Stickrun.h" };

	std::string dummy;
	try
	{
		StringBuffer OUTPUT_CPP_FILE_STREAM;

		// Get option params and source files.
		std::unordered_map<std::wstring, std::wstring> optionToValue;
		std::vector<std::wstring> sourceFiles;
		UtilMisc::ParseOptionArgs(optionToValue, sourceFiles, argc, argv);
		if (optionToValue.find(OPTION_OUT) == optionToValue.end())
			ThrowLeException(LeError::OPTION_NOT_SPECIFIED, OPTION_OUT);	// コマンドラインオプションが指定されていません。
		if (sourceFiles.empty())
			ThrowLeException(LeError::FILE_NOT_SPECIFIED);	// ファイルが指定されていません。

		std::wstring stickrunPath;
		for (const auto & headFile : sourceFiles)
		{
			std::wstring r(headFile);
			if (::_wcsicmp(UtilString::Right(r, STICKLIB_LEN).c_str(), STICKLIB) == 0)
			{
				stickrunPath = headFile;
				stickrunPath.erase(stickrunPath.length() - STICKLIB_LEN);
				stickrunPath += STICKRUN;
				break;
			}
		}
		if (stickrunPath.empty())
			ThrowLeException(LeError::FILE_NOT_SPECIFIED, STICKRUN);	// ファイルが指定されていません。

		// Output file path. It does not include extension. e.g. "C:\folder1\fileA"
		const auto outFilePath = optionToValue.find(OPTION_OUT)->second;
		wchar_t fname[_MAX_FNAME];
		_wsplitpath_s(
			outFilePath.c_str(),	// path,
			nullptr, 0,				// drive,
			nullptr, 0,				// dir,
			fname, _countof(fname),
			nullptr, 0				// ext,
		);
		// Output file name. It does not include extension. e.g. "fileA"
		const std::wstring outFileName = fname;

		// Language. e.g. "ja","en","zh"...
		const auto iOptionLang = optionToValue.find(OPTION_LANG);
		if (iOptionLang != optionToValue.end())
			Astrwstr::wstr_to_astr(LANG, iOptionLang->second);

		// class and it's member function array.
		// std::vector<ClassRec> CLASS_AND_CFUNC_SET;
		auto & classRec = ClassRec::New(-1);
		classRec.classType = ClassRec::Type::GLOBAL;

		// externで宣言されたC++の関数名セット。グローバル関数名。
		std::vector<std::string> globalFunction;

		// Current date. __DATE__ is macro, so it is impossible to replace __DATE__ inside of FORMTEXT.
		const char * _DATE_ = __DATE__;

		// Add header include sentences. ex. "#include "hello.h""
		OUTPUT_CPP_FILE_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
// Generated by LuaStick, ${_DATE_}.


)", _DATE_);

		for (const auto & headFile : sourceFiles)
		{
			OUTPUT_CPP_FILE_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
#include "${Astrwstr::wstr_to_astr(dummy, headFile)}"

)", Astrwstr::wstr_to_astr(dummy, headFile));
		}

		OUTPUT_CPP_FILE_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
#include "${Astrwstr::wstr_to_astr(dummy, outFileName)}.h"


)", Astrwstr::wstr_to_astr(dummy, outFileName));

		OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
// Generated by LuaStick, ${_DATE_}.

#pragma once


)", _DATE_);

		for (const auto & headFile : sourceFiles)
		{
			OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
#include "${Astrwstr::wstr_to_astr(dummy, headFile)}"

)", Astrwstr::wstr_to_astr(dummy, headFile));
		}

		OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(

struct lua_State;
extern void luastick_init(lua_State* L);


)");

		for (const auto & sourceFile : sourceFiles)
		{
			ReadBufferedFile readBufferedFile(sourceFile.c_str());
			try
			{
				ParseSource1(readBufferedFile, classRec);
			}
			catch (LeException e)
			{
				LeError::ErrorCode errorCode;
				__int32 lineNumber;
				std::wstring params;
				ErrorManager.GetError(
					errorCode,
					lineNumber,
					Dummy<std::wstring>(),
					params,
					e.GetErrorId()
				);
				throw PsException(readBufferedFile.m_fileName, readBufferedFile.m_lineNumber, errorCode, params);
			}
			catch (PsException e)
			{
				throw e;
			}
			catch (...)
			{
				throw PsException(readBufferedFile.m_fileName, readBufferedFile.m_lineNumber, LeError::SYSTEM, LeError::SystemErrorToString(::GetLastError()));
			}
		}

		for (const auto & sourceFile : sourceFiles)
		{
			ReadBufferedFile readBufferedFile(sourceFile.c_str());
			try
			{
				ParseSource2(readBufferedFile, classRec);
			}
			catch (LeException e)
			{
				LeError::ErrorCode errorCode;
				__int32 lineNumber;
				std::wstring params;
				ErrorManager.GetError(
					errorCode,
					lineNumber,
					Dummy<std::wstring>(),
					params,
					e.GetErrorId()
				);
				throw PsException(readBufferedFile.m_fileName, readBufferedFile.m_lineNumber, errorCode, params);
			}
			catch (PsException e)
			{
				throw e;
			}
			catch (...)
			{
				throw PsException(readBufferedFile.m_fileName, readBufferedFile.m_lineNumber, LeError::SYSTEM, LeError::SystemErrorToString(::GetLastError()));
			}
		}

		CreateDefaultConstructors(classRec);

		OutputStructGetFuncs(classRec);

		OutputStructPushFuncs(classRec);

		OutputDestructors(classRec);

		OutputFuncWrappers(classRec);

		OutputPolymorphicFunctions(classRec);

		OutputClassPushArgFuncs(classRec);


		OUTPUT_H_FILE_STREAM << FORMTEXT(u8R"(

#include "${stickrunPath}"

)", stickrunPath);

		OUTPUT_INITFUNC_STREAM << FORMTEXT(u8R"(
// ${SRCMARKER}
/// <summary>
/// LuaStick initializing function.
/// luastick_init must be called to register the classes and its member functions.
/// </summary>
void luastick_init(lua_State* L)
{

)");

		OutputStickInitCpp(classRec);

		OUTPUT_INITFUNC_STREAM << u8"}\n\n";

		// std::tr2::sys::path path(outFilePath);
		std::experimental::filesystem::path path(outFilePath);

		UtilFile::SaveFile(OUTPUT_H_FILE_STREAM.get(dummy), (outFilePath + L".h").c_str());

		OUTPUT_CPP_FILE_STREAM.insert(OUTPUT_CPP_FILE_STREAM.end(), OUTPUT_EXPORTFUNC_STREAM.begin(), OUTPUT_EXPORTFUNC_STREAM.end());
		OUTPUT_CPP_FILE_STREAM.insert(OUTPUT_CPP_FILE_STREAM.end(), OUTPUT_INITFUNC_STREAM.begin(), OUTPUT_INITFUNC_STREAM.end());
		UtilFile::SaveFile(OUTPUT_CPP_FILE_STREAM.get(dummy), (outFilePath + L".cpp").c_str());

		StringBuffer htmlManualBuffer;
		OutputHtml(htmlManualBuffer, classRec, path.filename().string());
		UtilFile::SaveFile(htmlManualBuffer.get(dummy), (outFilePath + L".html").c_str());
	}
	catch (LeException e)
	{
		e.OutErrorMessage(std::wcerr);
	}
	catch (PsException e)
	{
		e.OutErrorMessage(std::wcerr);
	}
	catch (...)
	{
		CerrLeException(LeError::SYSTEM);
	}
	return 0;
}
