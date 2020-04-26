#pragma once

#include "LeException.h"
#include "UtilFile.h"
#include "UtilString.h"

class ReadBufferedFile
{
public:
	ReadBufferedFile (const wchar_t* fileName)
	{
		UtilFile::LoadFile(m_buffer, fileName);
		m_bufferHead = m_buffer.data();
		m_fileName = fileName;
		m_lineNumber = 1;
		// Backup previous information.
		m_lineNumberBack = m_lineNumber;
		m_bufferHeadBack = m_bufferHead;
	}
	~ReadBufferedFile ()
	{
		m_bufferHead = nullptr;
	}
	
	/// <summary>
	/// <para> Reads the one sentence.                          </para>
	/// <para> 1.Cut at next char of ';'.                       </para>
	/// <para> 2.Cut in front of '{' and at next of '{'.        </para>
	/// <para> 3.Cut in front of '}' and at next of '}'.        </para>
	/// <para> 4.Comment is one sentence.                       </para>
	/// <para> 5.Comment is erased when inside of parenthesis.  </para>
	/// <para> 6.Do not cut at '\r' and '\n' characters.        </para>
	/// </summary>
	/// <param name="buffer">The buffer.</param>
	/// <returns>true:Data was read/false:End of text</returns>
	bool ReadOneSentence(std::string & buffer)
	{
		int lineNumber = m_lineNumber;
		try
		{
			auto nextPtr = UtilString::ReadOneSentence(buffer, m_bufferHead, lineNumber);
			if (m_bufferHead == nextPtr) return false;

			// Backup previous information.
			m_lineNumberBack = m_lineNumber;
			m_bufferHeadBack = m_bufferHead;

			m_bufferHead = nextPtr;
			m_lineNumber = lineNumber;
		}
		catch (LeException e)
		{
			LeError::ErrorCode errorCode;
			__int32 lineNumber;
			std::wstring params;
			std::wstring dummy;
			ErrorManager.GetError(
				errorCode,
				lineNumber,
				dummy,
				params,
				e.GetErrorId()
			);
			throw PsException(m_fileName, lineNumber, errorCode, params);
		}
		return true;
	}

	/// <summary>
	/// Put back one sentence which are read with ReadOneSentence.
	/// </summary>
	void PutBackOneSentence()
	{
		m_lineNumber = m_lineNumberBack;
		m_bufferHead = m_bufferHeadBack;
	}

public:
	std::wstring m_fileName;
	int m_lineNumber;
private:
	std::vector<char> m_buffer;
	const char* m_bufferHead;

	int m_lineNumberBack;
	const char * m_bufferHeadBack;
}; // class ReadBufferedFile.

