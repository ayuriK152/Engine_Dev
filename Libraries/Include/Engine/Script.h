#pragma once
#include "Component.h"
class Script : public Component
{
	using Super = Component;
public:
	Script();
	virtual ~Script();

	virtual void OnDestroy() override = 0;

	virtual void LoadXML(XMLElement* compElem) override = 0;
	virtual void SaveXML(XMLElement* compElem) override = 0;

	virtual ComponentSnapshot CaptureSnapshot() override { 
		ComponentSnapshot snapshot;
		snapshot.id = _id;
		return snapshot;
	};
	virtual void RestoreSnapshot(ComponentSnapshot snapshot) override { };
};

