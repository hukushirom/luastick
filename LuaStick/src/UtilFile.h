#pragma once

#include <iostream>
#include <fstream>
#include "LeException.h"

class UtilFile
{
public:
	static void LoadFile(std::vector<char> & buffer, const wchar_t* fileName)
	{
		std::ifstream is(fileName, std::ios::binary);
		//----- ファイルが開けなかった場合 -----
		if (!is)
			ThrowLeException(LeError::CANNOT_OPEN_FILE, fileName);
		is.seekg(0, is.end);
		auto fileSize = (size_t)is.tellg();
		is.seekg(0, is.beg);
		buffer.resize(fileSize + 1);
		is.read(buffer.data(), buffer.size());
		is.close();

		if (sizeof(GlobalDefs::BOM8) <= fileSize &&
			::memcmp(buffer.data(), GlobalDefs::BOM8, sizeof(GlobalDefs::BOM8)) == 0)
		{
			fileSize -= sizeof(GlobalDefs::BOM8);
			buffer.erase(buffer.begin(), buffer.begin() + sizeof(GlobalDefs::BOM8));
		}
		buffer[fileSize] = '\0';
	}

	static void SaveFile(const std::string & text, const wchar_t* fileName)
	{
		std::ofstream os(fileName, std::ios::out | std::ios::binary);
		if (!os)
			ThrowLeException(LeError::CANNOT_OPEN_FILE, fileName);
		os.write(text.c_str(), text.size());
		os.close();
	}
};

