#pragma once
#include <map>
#include <string>
 
/// 对象创建模版函数， 用到了 c++11 的可变参数
template<class YourClass, typename ...ArgType>
void* __createObjFunc(ArgType... arg)
{
	return new YourClass(arg...);
}
 
/// 需要反射的类使用该宏注册
#ifndef CLASS_REGISTER
#define CLASS_REGISTER(YourClass, ...) \
    static int __type##YourClass = Factory::regCreateObjFunc(#YourClass, (void*)&__createObjFunc<YourClass, ##__VA_ARGS__>);
#endif

#ifndef CLASS_STRING_REGISTER
#define CLASS_STRING_REGISTER(YourClass, BindString,...) \
    static int __type##YourClass = Factory::regCreateObjFunc(BindString, (void*)&__createObjFunc<YourClass, ##__VA_ARGS__>);
#endif
 
/// 对象工厂，根据类名创建
class Factory
{
public:
	/// 根据类名创建 BaseClass 子类的对象
	template<class BaseClass, typename ...ArgType>
	static BaseClass* CreateObj(std::string className, ArgType... arg)
	{
		typedef BaseClass*(*_CreateFactory)(ArgType...);
 
		auto& _funcMap = _GetStaticFuncMap();
		auto iFind = _funcMap.find(className);
		if (iFind == _funcMap.end())
			return nullptr;
		else
			return reinterpret_cast<_CreateFactory>(_funcMap[className])(arg...);
	}
 
	/// 注册“创建类的对象的工厂函数”
	static int regCreateObjFunc(std::string className, void* func)
	{
		_GetStaticFuncMap()[className] = func;
		return 0;
	}
 
private:
	/// 获取 函数名==>函数指针 的映射。
	static std::map<std::string, void*>& _GetStaticFuncMap()
	{
		static std::map<std::string, void*> _classMap;
		return _classMap;
	}
};
 