#pragma once
#include "Script.h"

class StartMenuSceneScript : public Script
{
public:
	~StartMenuSceneScript();

	void Init() override;
	void Update() override;


	void OnDestroy() override;


	void LoadXML(Bulb::XMLElement compElem) override;


	void SaveXML(Bulb::XMLElement compElem) override;

public:
	void OnClickedStartButton();

private:
	shared_ptr<UIButton> _startButton;
};

