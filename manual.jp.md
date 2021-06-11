# LuaStickの使い方  

************************************************************  

## フォルダー構成  

**LuaStick**
- LuaStick.exeのソースプログラム。可能な限り標準的なC\+\+コードで書かれている。  

**lua-5.3.5**
- lua-5.3.5のソースプログラム及びVisual Studioプロジェクト。

**lua-5.4.3**
- lua-5.4.3のVisual Studioプロジェクト。

**Examples_5_3**
- lua-5.3のサンプルプログラム。

**Examples_5_4**
- lua-5.4のサンプルプログラム。

**StickSimpleExample**
- C\+\+の関数をLuaにエクスポートするサンプルプログラム。  

**StickClassExample**
- C\+\+のクラス、ネームスペースをエクスポートするサンプルプログラム。  

**Sticktrace**
- デバッグツールのライブラリ。デバッグトレース、変数のウォッチ等の機能を追加する。  
- アルファ版。  
- MFCを使用。  

<img src="https://github.com/hukushirom/luastick/blob/master/Examples_5_3/Sticktrace/image01.png">

**StickTest**
- Sticktraceを使用したサンプルプログラム。  
- このドキュメントの最後にある付録 'デバッグ方法'を参照。

************************************************************  

## ビルド環境  

**LuaStick**
- Visual Studio 2017 C\+\+  
- ビルドにLuaは使わない。  

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

## ビルド方法  

Lua 5.3.5 用

1. Visual Studio 2017を起動。  
2. 'BuildAll.sln'を開く。  
3. ビルド。  

Lua 5.4.3 用

1. 'Build_5_4.sln'を開く。  
2. ビルド。  

************************************************************  

## LuaStickの実行方法  

以下のようなコマンドラインで実行する。  
```  
>LuaStick.exe -out Stick -lang ja extinsert.html Sticklib.h main.h srcx.h  
```  
**-out**&emsp;出力するファイルの名前を指定する。  
**-lang**&emsp;言語を指定する。詳しくは後述の「言語の指定」を参照。  

以下は、LuaStick.exeによって作成されるファイルとアプリケーションの構成例。  
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
- HTMLヘルプに挿入するHTML。  
- ユーザーが作成する。ほとんどのケースで指定する必要がない。  

**Sticklib.h**
- Stick.cppの中で使われる関数が記述されたライブラリー。パラメーターに必ず指定しなければならない。  
- LuaStick/filesフォルダーに存在する。  
- ユーザーが変更する必要はない。  

**main.h, srcx.h**
- Luaにエクスポートする関数が記述されたヘッダーファイル。  
- ユーザーが作成する。  
- 以下で説明するタグの記載が必要。  

**Stickrun.h**
- Luaを実行するための関数が記述されたライブラリー。  
- LuaStick/filesフォルダーに存在する。  
- ユーザーが変更する必要はない。  

**Stick.h, Stick.cpp**
- C\+\+の機能をLuaにエクスポートするためのバインド関数が記述されたソースコード。  
- LuaStick.exeによって生成される。  
- ユーザーが変更する必要はない。  

**Stick.html**
- LuaにエクスポートされたC\+\+の機能について解説したマニュアルのHTMLファイル。  
- LuaStick.exeによって生成される。  
- 必要であれば、ユーザーが変更する。  

**Stick.css**
- Stick.htmlで使用するスタイルシート。  
- LuaStick/filesフォルダーに存在する。  
- 必要であれば、ユーザーが変更する。  

************************************************************  

## LuaStickで使われるタグ  

LuaStickで使用するタグはすべて３つのスラッシュ"///"から始まる必要がある。
LuaStickでは、以下のタグを使用する。
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

エクスポートしたい機能に対しては<stick\>、\<param\>、\<exception\>を記述する。
パラメーターにstd::string, int 等、一般的な型を使う場合は\<sticktype\>、\<stickconv\>、\<stickdef\>を記述する必要はない。
マニュアルに出力したい機能に対しては、\<summary\>、\<para\>、\<code\>を記述する。

<br/><br/>

### \<stick\>  

Luaにエクスポートする機能の前に記述する。以下で使用可能。  
- class  
- struct  
- namespace  
- enum  
- static const  
- constexpr  
- \#define  
- 関数  
- 変数[^1]

[^1]: 変数はstructのメンバー変数のみエクスポート可能。classのメンバー変数、グローバル変数はエクスポートできない。

###### 例  

```  
/// <stick export="true" />  
class A  
{  
  :  
```  

#### 属性  

##### export  

必須。"true"/"false"のいずれかを指定する。  
"true"が指定された機能がエクスポートされる。  

###### 例  

```  
/// <stick export="true" />  
namespace sample  
{  
  :  
```  

##### type  

任意。クラス等の種別を変更する場合に指定する。以下で使用可能。  
class, struct, namespace で指定可能。  

- **class** に指定  

    - type=**"inconstructible"** : コンストラクターが無いクラスとしてエクスポートする。Factory methodで構築されるクラス用。  
    - type=**"struct"**：structとしてエクスポートする。  
    - type=**"namespace"**：namespaceとしてエクスポートする。  

- **struct** に指定  

    - type=**"class"**：classとしてエクスポートする。  
    - type=**"inconstructible"** : コンストラクターが無いクラスとしてエクスポートする。Factory methodで構築されるクラス用。  
    - type=**"namespace"**：namespaceとしてエクスポートする。  

- **namespace** に指定  

    - type=**"class"**：classとしてエクスポートする。 

###### 例  

```  
/// <stick export="true" type="namespace" />  
class ABC  
{  
  :  
// namespaceとしてエクスポートする。メンバー関数はstaticでなければならない。  
```  

##### ctype  

任意。定数の型を指定する場合に記述する。\#defineの場合は必須。  

###### 例  

```  
/// <stick export="true" ctype="void*" />  
static const int* ARRAY;  
// int* は Luaの型に変換できない。void* と定義することにより lightuserdata として変換可能になる。  

/// <stick export="true" ctype="void*" />  
constexpr int* ARRAY;  
// int* は Luaの型に変換できない。void* と定義することにより lightuserdata として変換可能になる。  

/// <stick export="true" ctype="char*" />  
#define ABC "hello"  
// #defineの型はLuaStickでは決定できない。明示する必要がある。  
```  

##### ltype  

任意。定数の型をLua型に変換する際、変換先のLua型を指定する。  

###### 例  

```  
/// <stick export="true" ltype="lightuserdata" />  
static const __int64 ABC=....;  
// __int64の定数ABCをlightuserdata型に変換してLuaに登録する。  
```  

##### lname  

任意。Luaにエクスポートする際に名称を変更する。以下で使用可能。  
- class
- struct
- namespace
- enum
- static const
- constexpr
- \#define
- 関数

###### 例  

```  
/// <stick export="true" lname="HELLO" />  
static const char* ARRAY;  
// Luaでは "HELLO" という変数名で参照する。  

/// <stick export="true" lname="ABC1" />  
/// <param name="a" io="in"></param>  
void ABC(int a);  

/// <stick export="true" lname="ABC2" />  
/// <param name="a" io="in"></param>  
void ABC(const char* a);  
// 多重定義の関数を Lua では別々の名称で定義する。  
```  

##### super  

任意。クラスのスーパークラスを変更する。スーパークラスを持たせない場合は"-"を指定。  

###### 例  

```  
class C2 : public C3 { ... }
/// <stick export="true" super="C3" />  
class C1 : public C2 { ... }
// クラスC1のスーパークラスをC3に変更してLuaにエクスポートする。  

/// <stick export="true" super="-" />  
class A1 : public A2 { ...  
// クラスA1を、スーパークラスを持たないクラスとしてLuaにエクスポートする。  
```  

<br/><br/>

### \<param\>  

Luaにエクスポートされる関数のパラメーターを定義する。  
HTMLヘルプに出力される、パラメーターの説明を記述する。  

###### 例  

```  
/// <stick export="true" />  
/// <param name="v1" io="inout">The value 1</param>  
/// <param name="v2" io="in">The value 2</param>  
/// <param name="obj" io="out" autodel="true">Create and return an object instance.</param>  
static void MyFunc(double & v1, int v2, MyObj *& obj);  
```  
#### 属性  

##### name  

必須。パラメーターの名称を指定する。  

##### io  

必須。入力のみのパラメーターは"in"、出力のみのパラメーターは"out"、入力と出力の両方のパラメーターは"inout"を指定する。  

##### ctype  

任意。パラメーターの型を指定する場合に記述する。  

###### 例  

```  
/// <param name="array" io="in" ctype="void*"></param>  
static void MyFunc(const int* array);  
// int* は Luaの型に変換できない。void* と定義することにより lightuserdata として変換可能になる。  
```  

##### ltype  

任意。パラメーターの型をLua型に変換する際、変換先のLua型を指定する。  

###### 例  

```  
/// <param name="array" io="out" ltype="lightuserdata"></param>  
static void MyFunc(int*& array);  
// int* は Luaの型に変換できない。lightuserdata と定義することにより lightuserdata として変換可能になる。  
```  

##### autodel  

任意。io="out" 且つ クラスインスタンスを出力する場合のみ有効。この属性を指定したインスタンスはLuaの中で自動的に削除される。  

##### text content  

任意。HTMLヘルプに出力される、パラメーターの説明を記述する。  

###### 例  

```  
/// <param name="array" io="out">Array of X</param>  
static void MyFunc(int*& array);  
// HTMLヘルプのパラメーター"array"の説明に"Array of X"が記載される。  
```  

<br/><br/>

### \<returns\>  

C\+\+関数が返す値を定義する。  
HTMLヘルプに出力される、返値の説明を記述する。  

###### 例  

```  
/// <stick export="true" />  
/// <returns ctype="void*" ltype="lightuserdata">My value</returns>  
static int* MyFunc();  
```  

#### 属性  

##### ctype  

任意。返値の型を指定する場合に記述する。  

##### ltype  

任意。返値の型をLua型に変換する際、変換先のLua型を指定する。  

##### autodel  

任意。クラスインスタンスを返す場合のみ有効。この属性を指定したインスタンスはLuaの中で自動的に削除される。  

##### text content  

任意。HTMLヘルプに出力される、返値の説明を記述する。  

###### 例  

```  
/// <returns>My value</returns>  
static int* MyFunc();  
// HTMLヘルプの返値"return_value"の説明に"My value"が記載される。  
```  

<br/><br/>

### \<exception\>  

エクスポートされるC\+\+関数が発生させる例外を定義する。例外は\<stickdef\>タグで予め定義しておく必要がある。  

###### 例  

```  
/// <stickdef type="exception" cref="MyException*" message="e->GetMessage()" delete="e->Delete()" />  

/// <stick export="true" />  
/// <summary>  
/// </summary>  
/// <exception cref="MyException*"" />  
static void MyFunc();  
```  

#### 属性  

##### cref  

必須。例外の型を指定する。  

<br/><br/>

### \<sticktype\>  

Luaのスタックから値を出力し、値を入力できる型を定義する。  

###### 例  

```  
<sticktype name="boolean" ctype="bool" getfunc="Sticklib::check_lvalue" setfunc="Sticklib::push_lvalue" />  
```  

#### 属性  

##### name  

必須。型の名称。HTMLのヘルプに出力される。ユニークでなければならない。  

##### ctype  

必須。C\+\+の型。  

##### getfunc  

必須。Luaのスタックからctypeで指定された型の値を取り出すための関数。以下の形式。  
```
void 関数名([ctype] & value, lua_State * L, int arg)  
```
- **value** : ctypeで指定されたC\+\+の型の変数。取得した値が設定される。  
- **L** : Luaのオブジェクト。  
- **arg** : パラメーターの番号。1,2,...。  

##### setfunc  

必須。Luaのスタックにctypeで指定された型の値を設定するための関数。以下の形式。  
```
void 関数名(lua_State * L, const [ctype] & value)  
```
- **L** : Luaのオブジェクト。  
- **value** : ctypeで指定されたC\+\+の型の変数。Luaに設定する値が指定される。  

<br/><br/>

### \<stickconv\>  

Luaから取り出した値をC\+\+の関数のパラメーターの型に変換するコンバーター、C\+\+の関数から取得した値をLuaに設定する値の型に変換するコンバーターを指定する。  
型の変換は２つ以上のコンバーターを中継して行うことができる。  

###### 例  

```  
// wchar_t*をstd::stringに変換する  
// [wchar_t*]---(wcharp_to_wstring)-->[std::wstring]---(wstring_to_string)-->[std::string]  

/// <stickconv type1="wchar_t*" type2="std::wstring" type1to2="wcharp_to_wstring" />  
/// <stickconv type1="std::wstring" type2="std::string" type1to2="wstring_to_string" />  

extern void wcharp_to_wstring(std::wstring& w, const wchar_t* p);
extern void wstring_to_string(std::string& w, const std::wstring & w);
```  

###### 例  

```  
// [__int32]---(T_to_U)-->[__int64]
// [__int32]<--(T_to_U)---[__int64]

/// <stickconv type1="__int64" type2="__int32" type1to2="T_to_U" type2to1="T_to_U" />  

template<typename T, typename U> static void T_to_U(U & u, T t)  
{ u = (U)t; }  
```  

#### 属性  

##### type1  

必須。変換する型の一つ。  

##### type2  

必須。変換する型の一つ。  

##### type1to2  

任意。但し type2to1 が指定されていない場合は必ずこれを指定しなければならない。  
type1 -\> type2 のコンバーターを指定。以下の形式。
```  
void 関数名([type2] & v2, const [type1] & v1)  
```  
または  
```  
void 関数名([type2] & v2, [type1] v1)  
```  
- **v2** : 変換先の変数。  
- **v1** : 変換元の変数。  

##### type2to1  

任意。但し type1to2 が指定されていない場合は必ずこれを指定しなければならない。  
type2 -\> type1 のコンバーターを指定。以下の形式。  
```
void 関数名([type1] & v1, const [type2] & v2)  
```
または  
```
void 関数名([type1] & v1, [type2] v2)  
```
- **v1** : 変換先の変数。  
- **v2** : 変換元の変数。  

<br/><br/>

### \<stickdef\>  

exceptionタグで指定されたユーザー定義の例外の、メッセージ取得方法と削除方法を記述する。  
但し"std::exception"はデフォルトでサポートされているため、記述する必要はない。  

###### 例  

```  
/// <stickdef type="exception" cref="MyException*" message="e->GetMessage()" delete="e->Delete()" />  

/// <exception cref="MyException*"></exception>  
static void MyFunc()
{
    throw new MyException();
}  
```  

#### 属性  

##### type  

必須。"exception"を指定。  

##### cref  

必須。例外の型を指定。  

##### message  

任意。例外オブジェクトからエラーメッセージのテキスト（std::string または const char*）を取得する方法を記述する。  
例外オブジェクトの変数名は[e]に固定されている。  

##### delete  

任意。例外オブジェクトを削除する方法を記述する。  
例外オブジェクトの変数名は[e]に固定されている。  

<br/><br/>

### \<summary\>  

HTMLヘルプに出力される、関数・定数等の説明を記述する。  
\<stick\>タグで使われる属性を\<summary\>に記述し、\<stick\>タグの代わりに使うことができる。

###### 例  

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

#### 属性  

##### text content  

任意。HTMLヘルプに出力される、関数・定数等の説明を記述する。  

###### 例  

```  
/// <summary>  
/// This is a my constant.  
/// </summary>  
constexpr int ABC = 10;  
// HTMLヘルプの定数"ABC"の説明に"This is a my constant."が記載される。  
```  

##### export  

任意。\<stick\>タグの解説を参照。  

##### type  

任意。\<stick\>タグの解説を参照。  

##### ctype  

任意。\<stick\>タグの解説を参照。  

##### ltype  

任意。\<stick\>タグの解説を参照。  

##### lname  

任意。\<stick\>タグの解説を参照。  

##### super  

任意。\<stick\>タグの解説を参照。  

<br/><br/>

### \<para\>  

\<summary\> ~ \</summary\>の中に記述することができる。  
改行を含むテキストをHTMLヘルプに出力させる。  

#### 属性  

##### text content  

 任意。HTMLヘルプに出力される、１行の文を記述する。  

###### 例  

```  
/// <summary>  
/// <para>This function has following functions  </para>  
/// <para>  1. Print xxx.   </para>  
/// <para>  2. Save xxx.    </para>  
/// </summary>  
static void MyFunc();  

// HTMLヘルプの関数"MyFunc"の説明に以下が記載される。  
This function has following functions  
1. Print xxx.  
2. Save xxx.  
```  

<br/><br/>

### \<code\>  

\<summary\> ~ \</summary\>の中に記述することにより、HTMLヘルプに以下の形式で出力させる。  
1. 等幅フォントのテキストで出力する  
2. スペースと改行をそのまま出力する  

#### 属性  

##### text content  

任意。HTMLヘルプに出力される、等幅フォントテキストを出力する。  

###### 例  

```  
/// <summary>  
/// <para>Program example for this function</para>  
/// <code>  
/// int x = 10;      // x is 10.
/// MyFunc(x);       // Call MyFunc.
/// </code>  
/// </summary>  
static void MyFunc(int x);  

HTMLヘルプの関数"MyFunc"の説明に以下が記載される。  

Program example for this function  
+-------------------------------------------------+  
| int x = 10;      // x is 10.                    |  
| MyFunc(x);       // Call MyFunc.                |  
+-------------------------------------------------+  
```  

************************************************************  

## 生関数  

パラメーターに lua_State\* を指定した関数を定義することができる。
生関数では、返値はvoid、パラメーターはlua_State\* のみでなければならない。

###### 例  

```  
/// <summary export="true">  
/// Raw function example  
/// </summary>  
/// <param io="in" name="L">lua_State object</<param>  
static void MyFunc(lua_State * L);  
```  

************************************************************  

## 言語の指定  

\<summary\>タグ等のtext contentは、複数の言語で記述することができる。  
コマンドラインの-langオプションで指定した言語の記述のみがマニュアルに出力される。  

#### 記述方法  

*言語コード1;* コンテンツ-1 *言語コード2;* コンテンツ-2...  

- 言語コードの前は行頭、空白、または改行でなければならない。  
- 言語コードは、HTMLのlang属性のいずれかのコードでなければならない。

#### 例  

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

## Luaの実行方法  

LuaStickでは、Stickrunクラスを使ってLuaを実行する。  

<br/><br/>

### Stickrun  

コンストラクター。Stickrunを初期化する。  

#### 文法  

Stickrun(std::function\<void(lua_State\*)\> luastick_init_func)  

#### パラメーター  

##### luastick_init_func  

Stick.cppに生成されたluastick_init関数。  

#### 例  

```  
Stickrun stickrun(luastick_init);  
```  

<br/><br/>

### DoString  

Luaスクリプトをロードし、実行する。  

#### 文法  

bool DoString(std::string* error_message, const std::string& source, const char* name)  

#### パラメーター  

##### error_message  

エラーメッセージを返す。nullptrが指定可能。  

##### source  

スクリプトコード。  

##### name

スクリプトコードが収納されるチャンクの名前。  

#### 返値  

- true : 正常  
- false : エラー  

#### 例  

```  
const std::string SCRIPT = "Test = function()\nPrint(msg)\nend";  
stickrun.DoString(nullptr, SCRIPT, "myscript");  
```  

<br/><br/>

### CallFunction  

Luaスクリプトの関数を実行する。  

#### 文法  

bool CallFunction(std::string* error_message, const char* funcname, Args&& ... args)  

#### パラメーター  

##### error_message  

エラーメッセージを返す。nullptrが指定可能。  

##### funcname  

関数名。  

##### args  

Lua関数の返値を受け取る変数、またはLua関数の引数。  
返値を受け取る場合は以下の形式にする。  
- Stickrun::Out([変数名])

#### 返値  

- true : 正常
- false : エラー  

#### 例  

```  
## Luaスクリプト  
Test = function(a, b)  
    return a + 1, b .. ",world"  
end  

## C\+\+プログラム  
int r1;  
std::string r2;  
stickrun.CallFunction(nullptr, "Test", Stickrun::Out(r1), Stickrun::Out(r2), 10, "hello");  
```  

<br/><br/>

### MakeSandboxEnv  

サンドボックスの環境を作成する。  

#### 文法  

void MakeSandboxEnv(const char* envname, bool is_copy_global)  

#### パラメーター  

##### envname  

サンドボックス名。  

##### is_copy_global  

- true : グローバル環境をサンドボックス環境へコピーする
- false : グローバル環境をサンドボックス環境へコピーしない  

trueを指定すると、グローバル環境の関数等がサンドボックスから使えるようになる。  

#### 例  

```  
stickrun.MakeSandboxEnv("SNDBX", true);  
```  

<br/><br/>

### RemoveSandboxEnv  

サンドボックスの環境を削除する。  

#### 文法  

void RemoveSandboxEnv(const char* envname)  

#### パラメーター  

##### envname  

サンドボックス名。  

#### 例  

```  
stickrun.RemoveSandboxEnv("SNDBX");  
```  

<br/><br/>

### DoSandboxString  

サンドボックス環境にLuaスクリプトをロードし、実行する。  

#### 文法  

bool DoSandboxString(std::string* error_message, const std::string& sandboxenv, std::string source, const char* name)  

#### パラメーター  

##### error_message  

エラーメッセージを返す。nullptrが指定可能。  

##### sandboxenv  

サンドボックス名。  

##### source  

スクリプトコード。  

##### name  

スクリプトコードが収納されるチャンクの名前。  

#### 返値  

- true : 正常
- false : エラー  

#### 例  

```  
const std::string SCRIPT = "Test = function()\nPrint(msg)\nend";  
stickrun.DoSandboxString(nullptr, "SANDBOX", SCRIPT, "myscript");  
stickrun.CallFunction(nullptr, "SANDBOX.Test");  
```  

<br/><br/>

### ErrorMessage  

各メンバー関数でエラーが発生した場合、この関数でエラーメッセージを取得する。  

#### 文法  

const std::string & ErrorMessage() const  

#### 返値  

エラーメッセージ  

<br/><br/>

### ClearError  

クラスインスタンスに保存されたエラーメッセージをクリアする。  

#### 文法  

void ClearError()  

<br/><br/>

### SetHook  

コールバック関数を設定する。詳しくはStickrun.hのHookFunc定義のコメントを参照。  

#### 文法  

void SetHook(Stickrun::HookFunc hookFunc, void* userData)  

#### パラメーター  

##### hookFunc  

コールバック関数。nullptrを指定することで解除。  

##### userData  

コールバック関数に渡すユーザーデータ。  

<br/><br/>

### GetHookUserData  

SetHook関数で指定したユーザーデータを返す。  

#### 文法  

void* GetHookUserData() const  

#### 返値  

ユーザーデータ。  

<br/><br/>

### GetLuaState  

Luaオブジェクトを返す。  

#### 文法  

lua_State* GetLuaState() const  

#### 返値  

Luaオブジェクト。  

************************************************************  

## Luaにエクスポート可能なC\+\+の型  

### 直接エクスポート可能な型  

以下の型は、Sticklib.h中の\<sticktype ...\>で定義されている。  
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

### コンバーターで間接エクスポート可能な型  

以下の型は、Sticklib.h中の<stickconv ...>で定義されたコンバーターを経由してエクスポートすることができる。  
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

## LuaStick のエクスポート方法  

#### グローバル関数  

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

#### 静的メンバー関数  

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

#### 定数  

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

#### 列挙  

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

#### クラス  

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

#### クラス継承  

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

#### クラスインスタンス  

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

#### 構造体インスタンス  

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

## 付録  

### デバッグ方法  

<img src="https://github.com/hukushirom/luastick/blob/master/Examples_5_3/Sticktrace/image01.png">

1. StickTest.exe を起動。  

2. (1)をクリック。'Script Editor' が起動する。  

3. (2)をクリックし、ブレークポイントを設定。  

4. (3)をクリックし、デバッグモードに変更。  

5. (4)をクリックし、Lua関数 'Test1' を実行。  

6. Luaスクリプトはデバッグポイントで停止する。  

7. 変数名 'c1' を(5)に入力。  

7. (6)をクリック。'c1' がウォッチウィンドウに追加される。

8. (7)をクリックし、配列'c1'を展開する。

### LuaStickの組み込みコマンド  

**STICK.ObjectToUserdata(object)**
- objectをlightuserdata型(C++ではvoid *)に変換する。  

**STICK.IsNullObject(object)**
- objectがnullptrかどうかを調べる。  

### Sticktraceの組み込みコマンド  

**STICKTRACE.DebugBreak()**
- Luaプログラムの実行を次の行で一時停止させる  

**STICKTRACE.OutputDebugMessage(x)**
- xをデバッガー中のメッセージボックスに表示させる  


