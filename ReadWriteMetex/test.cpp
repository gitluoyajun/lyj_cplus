#include "test.h"
#include <cstdio>
#include "../Common/MyTime.h"
#include <mutex>
#include <shared_mutex>
using namespace std;
int& GetRunTime()
{
	static int RunTimes = 100000;
	return RunTimes;
}

void readProc(ReadWriteMutex& mutex, std::vector<int>& num_list)
{
	auto run_time = GetRunTime();
	thread_local int pre_number = 0;
	while (run_time > 0)
	{
		UseReadWriteMutex lck(ReadWriteType::Read, mutex);
		auto last = num_list.rbegin();
		pre_number = *last;
		if (pre_number != *last)
		{
			printf("read_111 thread unsafe : pre :%d current:%d\n", pre_number, *last);
		}
		run_time--;
	}
}
void writeProc(ReadWriteMutex& mutex, std::vector<int>& num_list)
{
	auto run_time = GetRunTime();
	struct timeval t_start;
	struct timezone t_zone;
	gettimeofday(&t_start, &t_zone);
	int cin_num = 0;
	//主线程写入,随意0~100,大于五十就删除最后一个,小于50插入,频繁改写vector对象
	while (run_time > 0)
	{
		cin_num = rand() % 100;
		UseReadWriteMutex lck(ReadWriteType::Write, mutex);
		if (num_list.size() > 5)
		{
			num_list.pop_back();
		}
		if (cin_num > 50 && num_list.size() > 1)
		{
			num_list.pop_back();
		}
		else
		{
			num_list.push_back(cin_num);
		}
		run_time--;
	}
	PrintfTime(string("运行次数:") + to_string(GetRunTime()) + ",用时:", &t_start);
}
void ReadWriteMutexTest()
{
	printf("hello from ReadWriteMetex!\n");
	std::vector<int> num_list{ 1 };
	ReadWriteMutex mutex;
	thread read_th1([&mutex, &num_list]() {readProc(mutex, num_list); });
	thread read_th2([&mutex, &num_list]() {readProc(mutex, num_list); });
	thread write_th1([&mutex, &num_list]() {writeProc(mutex, num_list); });
	thread write_th2([&mutex, &num_list]() {writeProc(mutex, num_list); });
	
	if (read_th1.joinable())
	{
		read_th1.join();
	}
	if (read_th2.joinable())
	{
		read_th2.join();
	}
	if (write_th1.joinable())
	{
		write_th1.join();
	}
	if (write_th2.joinable())
	{
		write_th2.join();
	}
}

void StdShareMutexTest()
{
	printf("hello from sharemutex!\n");
	std::vector<int> num_list{ 1 };
	std::shared_mutex mutex;
	thread read_th1([&mutex, &num_list]()
		{
			auto run_time = GetRunTime();
			thread_local int pre_number = 0;
			while (run_time > 0)
			{
				std::shared_lock<std::shared_mutex> lc(mutex);
				auto last = num_list.rbegin();
				pre_number = *last;
				if (pre_number != *last)
				{
					printf("read_111 thread unsafe : pre :%d current:%d\n", pre_number, *last);
				}
				run_time--;
			}
		});
	thread read_th2([&mutex, &num_list]()
		{
			auto run_time = GetRunTime();
			thread_local int pre_number = 0;
			while (run_time > 0)
			{
				std::shared_lock<std::shared_mutex> lc(mutex);
				auto last = num_list.rbegin();
				pre_number = *last;
				if (pre_number != *last)
				{
					printf("read_222 thread unsafe : pre :%d current:%d\n", pre_number, *last);
				}
				run_time--;
			}
		});
	{
		auto run_time = GetRunTime();
		struct timeval t_start;
		struct timezone t_zone;
		gettimeofday(&t_start, &t_zone);
		int cin_num = 0;
		//主线程写入,随意0~100,大于五十就删除最后一个,小于50插入,频繁改写vector对象
		while (run_time > 0)
		{
			cin_num = rand() % 100;
			std::lock_guard<std::shared_mutex> lc(mutex);
			if (cin_num > 50 && num_list.size() > 0)
			{
				num_list.pop_back();
			}
			else
			{
				num_list.push_back(cin_num);
				//printf("insert: %d\n", cin_num);
			}
			run_time--;
		}
		PrintfTime(string("运行次数:") + to_string(GetRunTime()) + ",用时:", &t_start);
		if (read_th1.joinable())
		{
			read_th1.join();
		}
		if (read_th2.joinable())
		{
			read_th2.join();
		}
	}
}
