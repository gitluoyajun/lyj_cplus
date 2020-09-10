#pragma once
#include <atomic>
#include <sys/select.h>

void inline sleep_ms(unsigned const int millisecond) {
	timeval timeout;
	timeout.tv_sec = millisecond / 1000;
	timeout.tv_usec = millisecond % 1000 * 1000;
	::select(0, nullptr, nullptr, nullptr, &timeout);
}

enum ReadWriteType
{
	Read = 0,
	Write
};
class UseReadWriteMutex;



class ReadWriteMutex
{
	friend UseReadWriteMutex;
public:
	ReadWriteMutex() :read_mutex(0), write_mutex(0) {};
	
private:					//只能通过UseReadWriteMutex来玩了,这样避免忘记unlock;
	void inline WriteLock()
	{
		//直接抢走读锁,要不读远大于写的时候永远都轮不到写
		read_mutex += 1;
		int expected = 0;
		//有写锁了,说明读都完事了,而且我们抢走了读锁,他们都不再读了
		while (!(write_mutex.compare_exchange_strong(expected, write_mutex+1)))
		{
			expected = 0;
		}	
	}
	void inline WriteUnLock()
	{
		read_mutex -= 1;
		write_mutex -= 1;
	}
	void inline ReadLock()
	{
		while (! (read_mutex == 0))
		{
		}
		write_mutex += 1;
	}
	void inline ReadUnLock()
	{	
		write_mutex -= 1;
	}

private:
	std::atomic<int> read_mutex;	//写只有一个线程,用bool够了,如果写多个线程就不好用了,要用计数器
	std::atomic<int> write_mutex;	//读有多个线程,他们可能A线程锁上,还没释放前,B线程释放了,导致A不安全.所以
									//必须计数器,当所有的读线程释放完后才能写
};

class UseReadWriteMutex
{
public:
	explicit UseReadWriteMutex(ReadWriteType type, ReadWriteMutex& mutex) : type(type), mutex(mutex)
	{
		if (type == ReadWriteType::Read)
		{
			mutex.ReadLock();
		}
		else if (type == ReadWriteType::Write)
		{
			mutex.WriteLock();
		}
	};
	~UseReadWriteMutex() {
		if (type == ReadWriteType::Read)
		{
			mutex.ReadUnLock();
		}
		else if (type == ReadWriteType::Write)
		{
			mutex.WriteUnLock();
		}
	};
private:
	ReadWriteType type;
	ReadWriteMutex& mutex;
};