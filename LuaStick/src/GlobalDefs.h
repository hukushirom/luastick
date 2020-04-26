#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <memory>
#include <regex>
#include <tuple>
#include <iostream>

#ifndef __WFILE__
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#endif//__WFILE__

using uint8 = unsigned __int8;

class GlobalDefs
{
public:
	static constexpr uint8 BOM8[] = { 0xef, 0xbb, 0xbf };
};

