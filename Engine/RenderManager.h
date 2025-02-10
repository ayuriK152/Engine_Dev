#pragma once

// Device, Command ������ �޼ҵ�� Graphic.h���� �޾ƿ���
// ������ ������ �� �ִ� ��� ���� �̽��ϱ�

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

