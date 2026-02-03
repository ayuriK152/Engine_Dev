#include "pch.h"
#include "Utils.h"

mt19937 Utils::_gen;

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

void Utils::Init()
{
	random_device rd;
	_gen.seed(rd());
}

string Utils::ToString(wstring s)
{
	string result;
	result.assign(s.begin(), s.end());
	return result;
}

wstring Utils::ToWString(string s)
{
	wstring result;
	result.assign(s.begin(), s.end());
	return result;
}

char* Utils::ToChar(wstring s)
{
	const wchar_t* input = s.c_str();
	size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
	char* buffer = new char[size];
	size_t convertedSize;
	wcstombs_s(&convertedSize, buffer, size, input, size);

	return buffer;
}

int Utils::Random(int from, int to)
{
	uniform_int_distribution<int> dis(from, to);
	return dis(_gen);
}
