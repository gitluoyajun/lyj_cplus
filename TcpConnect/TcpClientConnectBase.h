#pragma once
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "boost/property_tree/ptree.hpp"
#include "boost/thread/thread.hpp"
//

const int RecvMsgLen = 128;
struct TcpConnectSendPingPara
{
	bool is_send_ping = false;
	std::string ping_common{ "" };//心跳请求命令
	std::string ping_recv_str{ "" };	///心跳回报
	unsigned int send_ping_grap = 5;
};

struct TcpConnectPara
{
	int id;
	std::string name;
	std::string ip;
	std::string port;
	std::string username;
	std::string password;
	std::string start_para;
	TcpConnectSendPingPara send_ping_para;
};

template<typename T, typename ... Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class ITcpClientSpi
{
public:
	virtual void OnRecvMsg(int id, std::string& msg) = 0;
	virtual void OnConnectStatus(int id, bool is_connect) = 0;
};

//starting sequence: Construct() => ConnectAndInit() =>Start();
class TcpClientConnectBase
{
public:
	TcpClientConnectBase(TcpConnectPara& para);
	virtual ~TcpClientConnectBase() { Stop(); };

	ssize_t Send(std::string& msg);
	//这里可能不同服务有不同的需求,如登录,从服务读取开始信息,开始服务的参数等
	//需要的参数请填写在TcpConnectPara::start_para 里,多个参数","隔开
	virtual int ConnectAndInit() = 0;
	void Start();
	//clear thread, spis
	void Stop();
	int inline GetSocket() { return s_server; };
	void ErasePingAndUpdatePingTime(std::string& messages, const std::string& ping_sign);
	int GetID() {
		return id;
	};
	virtual inline std::string GetName() { return name; }
	virtual inline void RegisterSpi(ITcpClientSpi* spi)
	{
		std::lock_guard<boost::shared_mutex> lc(spi_mutex);
		auto iter = std::find(spis.begin(), spis.end(), spi);
		if (iter == spis.end())
		{
			spis.push_back(spi);
		}
	}
	virtual inline void UnRegisterSpi(ITcpClientSpi* spi)
	{
		std::lock_guard<boost::shared_mutex> lc(spi_mutex);
		auto iter = std::find(spis.begin(), spis.end(), spi);
		if (iter != spis.end())
		{
			spis.erase(iter);
		}
	}

protected:
	void ConnectService();
	inline void InvokeStatusCallBack(const int cnnstatus)
	{
		boost::shared_lock<boost::shared_mutex> lc(spi_mutex);
		for (auto spi : spis)
		{
			spi->OnConnectStatus(id, cnnstatus);
		}
	}

	inline void InvokeRecvMsgCallBack(std::string& msg)
	{
		boost::shared_lock<boost::shared_mutex> lc(spi_mutex);
		for (auto spi : spis)
		{
			spi->OnRecvMsg(id, msg);
		}
	}
protected:
	void RecvProc();
	void HeartProc();
	void SetConnect(bool connect);
	int ConnectTcpWithTimeOut();
protected:
	int id;
	std::string name{ "" };
	TcpConnectPara connect_para;
	int s_server;
	sockaddr_in server_addr;

	boost::shared_mutex spi_mutex;
	std::vector<ITcpClientSpi*> spis;
protected:
	time_t last_ping_time;

private:
	std::unique_ptr<std::thread> heart_thread;
	std::unique_ptr<std::thread> recv_thread;


private:
	std::mutex stop_mutex;
	std::condition_variable cv_stop;	//停止通知,让心跳线程在等待的时候能挂起,停止时立即返回,比sleep超时好
	bool is_stop{ false };
	bool is_connect{ false };

};