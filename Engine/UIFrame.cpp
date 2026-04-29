#include "pch.h"
#include "UIFrame.h"

UIFrame::UIFrame() : UIElement(UIType::Frame)
{

}

UIFrame::~UIFrame()
{
	cout << "Released - UIFrame\n";
}

void UIFrame::LoadXML(XMLElement* uiElem)
{
	
}
