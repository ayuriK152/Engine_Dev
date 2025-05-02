#include "pch.h"
#include "UniversalUtils.h"

DxException::DxException(HRESULT hr, const wstring& functionName, const wstring& filename, int lineNumber) :
	errorCode(hr), functionName(functionName), filename(filename), lineNumber(lineNumber)
{

}

wstring DxException::ToString() const
{
	_com_error err(errorCode);
	wstring msg = err.ErrorMessage();

	return functionName + L" failed in " + filename + L"; line " + std::to_wstring(lineNumber) + L"; error: " + msg;
}

string UniversalUtils::ToString(wstring s)
{
	string result;
	result.assign(s.begin(), s.end());
	return result;
}

wstring UniversalUtils::ToWString(string s)
{
	wstring result;
	result.assign(s.begin(), s.end());
	return result;
}

char* UniversalUtils::ToChar(wstring s)
{
	const wchar_t* input = s.c_str();
	size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
	char* buffer = new char[size];
	size_t convertedSize;
	wcstombs_s(&convertedSize, buffer, size, input, size);

	return buffer;
}
