#pragma once

namespace Bulb {
	// Wrapping class of tinyxml2::XMLElenent
	class BULB_API XMLElement {
	public:
		XMLElement() {}
		XMLElement(tinyxml2::XMLElement* origin) : _origin(origin) {}
		~XMLElement() {}

		bool IsNullPtr() { return _origin == nullptr; }

	public:
		const char* Attribute(const char* name, const char* value = 0) const { return _origin->Attribute(name, value); }

		int IntAttribute(const char* name, int defaultValue = 0) const { return _origin->IntAttribute(name, defaultValue); }

		bool BoolAttribute(const char* name, bool defaultValue = false) const { return _origin->BoolAttribute(name, defaultValue); }

		double DoubleAttribute(const char* name, double defaultValue = 0) const { return _origin->DoubleAttribute(name, defaultValue); }

		float FloatAttribute(const char* name, float defaultValue = 0) const { return _origin->FloatAttribute(name, defaultValue); }


		void SetAttribute(const char* name, const char* value) { _origin->SetAttribute(name, value); }

		void SetAttribute(const char* name, int value) { _origin->SetAttribute(name, value); }

		void SetAttribute(const char* name, bool value) { _origin->SetAttribute(name, value); }

		void SetAttribute(const char* name, double value) { _origin->SetAttribute(name, value); }

		void SetAttribute(const char* name, float value) { _origin->SetAttribute(name, value); }


		Bulb::XMLElement InsertNewChildElement(const char* name) { return XMLElement(_origin->InsertNewChildElement(name)); }

		Bulb::XMLElement FirstChildElement(const char* name = 0) { return XMLElement(_origin->FirstChildElement(name)); }

		Bulb::XMLElement NextSiblingElement() { return XMLElement(_origin->NextSiblingElement()); }

	private:
		tinyxml2::XMLElement* _origin;
	};
}

