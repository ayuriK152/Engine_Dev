#pragma once

class UniversalUtils
{
public:
	static void CalculateFrameStats();
	static string ToString(wstring s);
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)												\
{																		\
	HRESULT hr__ = (x);													\
	wstring wfn = AnsiToWString(__FILE__);								\
	if (FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }	\
}
#endif

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const wstring& functionName, const wstring& filename, int lineNumber);

	wstring ToString()const;

	HRESULT errorCode = S_OK;
	wstring functionName;
	wstring filename;
	int lineNumber = -1;
};

inline wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return wstring(buffer);
}

