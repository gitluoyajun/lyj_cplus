#include "TcpClientConnectBase.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>

using namespace std;
	
TcpClientConnectBase::TcpClientConnectBase(TcpConnectPara& para) :connect_para(para)
{
	string error_info = "";
	id = para.id;
	name = para.name;
	server_addr.sin_family = AF_INET;   //IPV4协议
	server_addr.sin_port = htons(atoi(connect_para.port.c_str()));     //服务器端口
	server_addr.sin_addr.s_addr = inet_addr(connect_para.ip.c_str());
}

void TcpClientConnectBase::ConnectService()
{
	s_server = socket(2, 1, 0);
	if (s_server == 0)
	{
		string error_info = "socket创建失败";
	}
		
	//if (ConnectTcpWithTimeOut() != 0)
	if (connect(s_server, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
	{
		string error_info = "server connect fail!!";
	}
	else {
		SetConnect(true);
	}
}

int TcpClientConnectBase::ConnectTcpWithTimeOut()
{
	fd_set set;
	struct timeval timeo {5,0};
	int flag = fcntl(s_server, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(s_server, F_SETFL, flag); //将fd设置为非阻塞
	if (connect(s_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
		flag &= ~O_NONBLOCK;
		fcntl(s_server, F_SETFL, flag); //将fd设置为非阻塞
		return 0;
	}
	if (errno != EINPROGRESS) {
		perror("connect");
		return-1;
	}
	FD_ZERO(&set);
	FD_SET(s_server, &set);
	int retval = select(s_server + 1, NULL, &set, NULL, &timeo);
	if (retval == -1) {
		return-1;
	}
	else if(retval == 0) {
		return -2;
	}
	else if (retval == 1)
	{
		flag &= ~O_NONBLOCK;
		fcntl(s_server, F_SETFL, flag); //将fd设置为非阻塞
		return 0;
	}
	else
	{
		return -1;
	}
	flag &= ~O_NONBLOCK;
	fcntl(s_server, F_SETFL, flag); //将fd设置为非阻塞
	return 0;
}

ssize_t TcpClientConnectBase::Send(std::string& msg)
{
	return send(s_server, msg.c_str(), msg.size(), 0);
}

void TcpClientConnectBase::Start()
{
	is_stop = false;
	recv_thread = std::unique_ptr<std::thread>(new thread([this]() {this->RecvProc(); }));
	heart_thread = make_unique<std::thread>([this]() {this->HeartProc(); });
}

void TcpClientConnectBase::Stop()
{
	is_stop = true;
	if (is_connect)
	{
		SetConnect(false);
	}

	cv_stop.notify_all();
	if (heart_thread && heart_thread->joinable())
	{
		heart_thread->join();
	}
	//别close 0 socket ,等下申请socket的时候就给你0了
	if (s_server > 0)
	{
		shutdown(s_server, SHUT_RDWR);
		close(s_server);
	}

	if (recv_thread && recv_thread->joinable())
	{
		recv_thread->join();
	}
	//wait all thread break;
		
	std::vector<ITcpClientSpi*> emptyspis = std::vector<ITcpClientSpi *>();
	spis.swap(emptyspis);
	//sleep_ms(500);
}

void TcpClientConnectBase::RecvProc()
{
	char accept_buff[RecvMsgLen];
	ssize_t iResult = 0;
	std::string str = "";
	do
	{
		if (is_connect)
		{
			str = "";
			memset(accept_buff, 0, RecvMsgLen);
			try
			{
				iResult = recv(s_server, accept_buff, RecvMsgLen - 1, 0);
			}
			catch (const std::exception& e)
			{
				cout << "recv exception: " << e.what() << endl;
			}
				
			if (iResult > 0)
			{
				str.append(accept_buff);
				ErasePingAndUpdatePingTime(str, connect_para.send_ping_para.ping_recv_str);
				InvokeRecvMsgCallBack(str);
			}
			else if (iResult == 0)
			{

				SetConnect(false);
			}
			else
			{
				std::cout << "recv failed:" << iResult;
			}
					
		}
		else {
			//对外部管理连接来说断开连接后线程没有存在的必要了,等重新连接创建吧
			//对内部重连来说,线程也可以重建
			break;
		}
	} while (!is_stop);
	//没停就一直等重连取数据
#ifdef _TIMELOG
	if (outfile)
	{
		time_log_str += "\n";
		outfile.write(time_log_str.c_str(), time_log_str.size());
		outfile.close();
	}
#endif // _TIMELOG

}

void TcpClientConnectBase::HeartProc()
{
	ssize_t len = 0;
	const size_t heart_grap = connect_para.send_ping_para.send_ping_grap;
	const string heart_command = connect_para.send_ping_para.ping_common;
	len = send(s_server, heart_command.c_str(), heart_command.size(), 0);
	time(&last_ping_time);
	do
	{
		if (is_connect)
		{
			try
			{
				len = send(s_server, heart_command.c_str(), heart_command.size(), 0);
			}
			catch (const std::exception&)
			{
				std::cout << "send heart command fail : " << len;
			}
				
			if (len < 1)
			{
				std::cout << "send heart command fail : " << len;
				SetConnect(false);
			}
			auto now_time = time(nullptr);
			if (time(nullptr) - last_ping_time > heart_grap * 3)
			{
				std::cout << "3 * heart_grep seconds no ping msg : last_ping_time:" << last_ping_time << " now:" << now_time;
				SetConnect(false);
			}
				
		}
		{
			std::unique_lock <std::mutex> lc(stop_mutex);
			cv_stop.wait_for(lc, std::chrono::seconds(heart_grap), [this]() {return is_stop; });
			lc.unlock();
		}
	} while (!is_stop);
}

void TcpClientConnectBase::SetConnect(bool connect)
{
	std::cout << "tcp connect: "<< name << "status change:" << connect;
	if (is_connect != connect)
	{
		is_connect = connect;
		InvokeStatusCallBack(is_connect);
	}	
}

	
void TcpClientConnectBase::ErasePingAndUpdatePingTime(std::string& messages, const std::string& ping_sign)
{
	if (ping_sign.empty())
		return;
	bool has_ping = false;
	do 
	{
		auto pos = messages.find(ping_sign);
		if (std::string::npos != pos)
		{
			messages.erase(pos, ping_sign.size());
			time(&last_ping_time);
			has_ping = true;
		}
		else
			has_ping = false;
	} while (has_ping);
	//有心跳就继续删除,直到没有心跳
		
}
