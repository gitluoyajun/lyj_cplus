#pragma once
#include <string>
#include <iostream>

class IService
{
public:
	template <class T>
	void SendMessage(const T& t) {};
};

class IServiceSpi
{
public:
	template <class T>
	void RecvMsg(const T& t) {};
};

class Client;

class Service : public IService
{
public:
	Service(IServiceSpi* client_) :client(client_) {};
	void SendMessage(const std::string& str)
	{
		std::cout << "service send: " << str << std::endl;
		client->RecvMsg<std::string>(str);
	}
private:
	IServiceSpi* client;
};

class Client :public IServiceSpi
{
public:
	void RecvMsg(const std::string& str) {
		std::cout << "clinet recv: " << str << std::endl;
	}
};