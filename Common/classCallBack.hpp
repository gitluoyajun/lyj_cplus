#pragma once
#include <string>
#include <functional>
using namespace std;
typedef std::function<string(const string&, string&)> apendCallBack;
class FuncClass
{

public :
	FuncClass()
	{
		callBack = bind(&FuncClass::apend, this, std::placeholders::_1, std::placeholders::_2);
	}
	std::string apend(const std::string& a, std::string& b)
	{
		return a + b;
	}

private:
	apendCallBack callBack;
};

