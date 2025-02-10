#pragma once

// Device, Command 관련은 메소드로 Graphic.h에서 받아오고
// 나눠서 관리할 수 있는 모든 것을 이식하기

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
public:
	~RenderManager();

public:
	void Init();
	void Update();

private:
	void UpdateMainCB();

};

