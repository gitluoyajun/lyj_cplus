#pragma once
#include "error.h"
#include <iostream>

void ThrowError()
{
	 throw myerror_t(1,"test",2,21);
}

void TestCatchError()
{
	try
	{
		ThrowError();
	}
	catch (const myerror_t& e)
	{
		std::cout << e.what() <<std::endl;
	}
}