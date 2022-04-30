#pragma once

#include "singleton.hpp"
#include <vector>
#include <string>
#include <Windows.h>
#include "helpers/Utilities.hpp"

#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif

template <typename T>
class ConfigValue
{
public:
	ConfigValue(std::string category_, std::string name_, T* value_)
	{
		category = category_;
		name = name_;
		value = value_;
	}

	std::string category, name;
	T* value;
};

class CConfig : public Singleton<CConfig>
{
protected:
	std::vector<ConfigValue<int>*> ints;
	std::vector<ConfigValue<bool>*> bools;
	std::vector<ConfigValue<float>*> floats;
private:
	void SetupValue(int&, int, std::string, std::string);
	void SetupValue(bool&, bool, std::string, std::string);
	void SetupValue(float&, float, std::string, std::string);
	void SetupColor(float value[4], std::string name);

	void SetupRage();
	void SetupLegit();
	void SetupVisuals();
	void SetupMisc();
	void SetupColors();
public:
	void Initialize();
	void Load(std::string szIniFile);
	void Save(std::string szIniFile);
};