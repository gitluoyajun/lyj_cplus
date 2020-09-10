#pragma once
#include <exception>
#include <string.h>
#include <iostream>
#include <sstream>
class myerror_t 
{
public:
	template<class T>
	void Add(std::string& str, T t)
	{
		std::cout << t << std::endl;;
		std::ostringstream oss;
		std::string strValue = "";
		oss << t;
		strValue = oss.str();
		str += strValue;
	}

	template <typename T, typename... Args>
	void Add(std::string& str, const T& t, const Args&... args)
	{
		std::cout << t << ",";
		std::ostringstream oss;
		oss << t;
		str += oss.str();
		Add(str,args...);
	}

	

	template<class ...Args>
	myerror_t(int err_no = -1,Args ...args) {
		_err_no = err_no;
		Add(_What, args...);	
	}

	myerror_t(const char* what, int err_no = -1):  _err_no(err_no) {
		_What = std::string(what);
	}

	myerror_t(const std::string what, int err_no = -1) : _What(what), _err_no(err_no){}

	int err_no() const
	{
		return _err_no;
	}
	const char* what() const throw ()
	{
		return _What.c_str();
	}
private:
	std::string _What{"error"};
	int _err_no{100};
};