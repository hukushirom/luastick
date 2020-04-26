
// TestClass.h : ヘッダー ファイル
//

#pragma once

/// <stick export="true" />
/// <summary>
/// Test Space.
/// </summary>
namespace TestSpace
{
	/// <stick export="true" />
	/// <summary>
	/// Get ABC
	/// </summary>
	/// <returns>ABC</returns>
	std::string GetA();
}


/// <stick export="true" />
/// <summary>
/// <para>LuaStick test struct.  </para>
/// <code>
///      +--------------------------------+           Help                         
///      |          Application           |      +------------+  Generate          
///      |                                |      | Stick.html |«---------+         
///      |   +----------+  +----------+   |      +------------+          |         
///      |   |Stickrun.h|  |Sticklib.h|---|---+                          |         
///      |   +----------+  +----------+   |   |       +--------------+   |         
///      |   +----------+  +----------+   |   +------»|              |---+         
///      |   | src1.cpp |  | src1.h   |---|----------»| LuaStick.exe |------+      
///      |   +----------+  +----------+   |   +------»|              |---+  |      
///      |   +----------+  +----------+   |   |       +--------------+   |  |      
///      |   | src2.cpp |  | src2.h   |---|---+                          |  |      
///      |   +----------+  +----------+   |                              |  |      
///      |   +----------+                 |                              |  |      
///      |   | src3.cpp |  +----------+   |      Generate                |  |      
///      |   +----------+  | Stick.h  |«--|------------------------------+  |      
///      |   +----------+  +----------+   |                                 |      
///      |   | src3.h   |  +----------+   |      Generate                   |      
///      |   +----------+  |Stick.cpp |«--|---------------------------------+      
///      |   +----------+  +----------+   |                                        
///      |   |liblua.lib|                 |                                        
///      |   +----------+                 |                                        
///      |        :                       |                                        
///      +--------------------------------+                                        
/// </code>
/// </summary>
struct TestStruct0
{
	/// <stick export="true" />
	/// <summary>
	/// Member x.
	/// </summary>
	double	x;

	/// <stick export="true" />
	/// <summary>
	/// Member a.
	/// </summary>
	int m_a;

	/// <stick export="true" />
	/// <summary>
	/// Member b.
	/// </summary>
	std::wstring m_b;

	/// <stick export="true" />
	/// <summary>
	/// Member x.
	/// </summary>
	std::vector<int> m_x;
};

/// <stick export="true" type="namespace" />
/// <summary>
/// LuaStick test class.
/// </summary>
class TestClassA
{
public:
	/// <stick export="true" />
	/// <summary>
	/// Get number 10.
	/// </summary>
	/// <returns>Return 10</returns>
	static int Get()
	{
		return 10;
	}

};

/// <stick export="true" />
/// <summary>
/// LuaStick test class.
/// </summary>
class TestClass0
{
public:
	/// <stick export="true" />
	/// <summary>
	/// Enum A.
	/// </summary>
	enum class EnumA
	{
		/// <stick export="true" lname="AAA" />
		/// <summary>
		/// A in Enum A.
		/// </summary>
		A = 10,	/* hello. */

		/// <stick export="true" />
		/// <summary>
		/// B in Enum A.
		/// </summary>
		B,

		/// <stick export="true" />
		/// <summary>
		/// C in Enum A.
		/// </summary>
		C,
	};

	/// <stick export="true" />
	enum EnumB
	{
		/// <stick export="true" />
		E,
		/// <stick export="true" />
		F,
		/// <stick export="true" />
		G,
	};

	/// <stick export="true" />
	enum
	{
		H,
		I,
		/// <stick export="true" />
		J,
	};
public:
	TestClass0()
	{
	}

	~TestClass0()
	{
	}

	/// <stick export="true" />
	/// <summary>
	/// Create object.
	/// </summary>
	/// <returns>TestClass0 object</returns>
	static TestClass0 * Create0()
	{
		return new TestClass0();
	}

	/// <stick export="true" />
	/// <summary>
	/// 
	/// </summary>
	/// <param io="inout" name="test">Argument test.</param>
	void GetArray(std::vector<TestStruct0> & test) const
	{
		TestStruct0 t;
		t.m_a = 10;
		t.m_b = L"hello";
		test.emplace_back(t);
	}

	/// <stick export="true" />
	/// <summary>
	/// Get test.
	/// </summary>
	/// <param io="inout" name="test">TestStruct0 object</param>
	void Get(TestStruct0 & test) const
	{
		test.m_a = 10;
		test.m_b = L"hello";
	}

	/// <stick export="true" />
	/// <summary>
	/// GetX function.
	/// </summary>
	/// <returns>return 0</returns>
	virtual int GetX()
	{
		return 0;
	}

	/// <stick export="true" />
	/// <summary>
	/// Get Enum A.
	/// </summary>
	/// <param name="ea" io="inout">Return enum A</param>
	void GetEnumA(EnumA & ea)
	{
		if (ea == EnumA::A)
			ea = EnumA::B;
	}
};

/// <stick export="true" />
/// <summary>
/// LuaStick test struct.
/// </summary>
struct TestStruct1
{
	/// <stick export="true" />
	/// <summary>
	/// Member c in TestStruct1.
	/// </summary>
	TestStruct0 m_c;

	/// <stick export="true" />
	/// <summary>
	/// Member d in TestStruct1.
	/// </summary>
	TestClass0 m_d;
};


/// <stick export="true" />
/// <summary>
/// LuaStick test class.
/// </summary>
class TestClass1 : public TestClass0
{
public:
	TestClass1()
	{
	}

	~TestClass1()
	{
	}

	/// <stick export="true" />
	/// <summary>
	/// Return TestClass1 object.
	/// </summary>
	/// <returns>TestClass1 object</returns>
	static TestClass1 * Create1()
	{
		return new TestClass1();
	}

	/// <stick export="true" />
	/// <summary>
	/// Get function.
	/// </summary>
	/// <returns>Return 10</returns>
	int Get() const
	{
		return 10;
	}

	/// <stick export="true" />
	/// <summary>
	/// GetX function.
	/// </summary>
	/// <returns>Return 15</returns>
	virtual int GetX()
	{
		return 15;
	}
};

/// <stick export="true" />
/// <summary>
/// LuaStick test class.
/// </summary>
class TestClass2
{
public:
	/// <stick export="true" />
	/// <summary>
	/// LuaStick test constant.
	/// </summary>
	static const TestClass0 & TESTCLASS0;

public:
	TestClass2()
	{
	}

	/// <stick export="true" />
	/// <summary>
	/// Constructor. Its has an argument.
	/// </summary>
	/// <param io="in" name="a">A</param>
	/// <returns>RestClass2 object.</returns>
	TestClass2(int a)
	{
	}

	~TestClass2()
	{
	}

	/// <stick export="true" />
	/// <summary>
	/// Create TestClass2 object.
	/// </summary>
	/// <param io="out" name="testClass2">TestClass2 object</param>
	static void Create2(TestClass2 * & testClass2)
	{
		testClass2 = new TestClass2();
	}

	/// <stick export="true" />
	/// <summary>
	/// Call Get() function in testClass1.
	/// </summary>
	/// <param io="inout" name="testClass1">TestClass1 object</param>
	/// <returns>testClass1->Get()</returns>
	int Get2(TestClass1 * & testClass1) const
	{
		return testClass1->Get();
	}
};

