#pragma once

#define REGISTER_COMPONENT(className)				\
	static ComponentRegisterHelper<className> _reg_##className(#className);

template<typename T>
struct ComponentRegisterHelper {
	ComponentRegisterHelper(string name) {
		ComponentFactory::Register<T>(name);
	}
};

class ComponentFactory
{
	using Creator = function<shared_ptr<Component>()>;

public:
	template<typename T>
	static void Register(string name) {
		GetMap()[name] = []() { return make_shared<T>(); };
	}

	static shared_ptr<Component> Create(string name) {
		if (GetMap().contains(name)) return GetMap()[name]();
		return nullptr;
	}

private:
	static unordered_map<string, Creator>& GetMap() {
		static unordered_map<string, Creator> _registry;
		return _registry;
	}
};

