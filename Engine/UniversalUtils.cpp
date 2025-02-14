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

void UniversalUtils::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((TIME->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText = GRAPHIC->GetAppDesc().mainWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(GRAPHIC->GetMainWnd(), windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
