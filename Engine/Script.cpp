#include "pch.h"
#include "Script.h"

Script::Script() : Super(ComponentType::Script)
{

}

Script::~Script()
{
	cout << "Released - Script:" << _id << "\n";
}
