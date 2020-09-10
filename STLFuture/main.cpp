#include <cstdio>
#include <future>

#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <future>
using namespace std;

class A {
public:
	A() { flag = false; }
	A(const A&) {}
	~A() {}
	bool run() {
		std::unique_lock<std::mutex> _1241_lock(my_lock);
		std::cerr << "waiting...\n";
		cv.wait(_1241_lock, [&] {return flag == true; });
		std::cerr << "...finished waiting, i == 1\n";
		return true;
	}
	bool signal() {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		{
			std::unique_lock <std::mutex> _5214_lock(my_lock);
			std::cerr << "Notifying again...\n";
		}
		cv.notify_all();
		{
			std::lock_guard <std::mutex> _5242_lock();
			flag = true;
			std::cerr << "Notifying again...\n";
		}
		cv.notify_all();
		return true;
	}
private:
	std::mutex my_lock;
	std::condition_variable cv;
	bool flag;
};

int main() {
	A a;
	//std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	std::future <bool> result1 = std::async(std::launch::async, &A::run, &a);
	std::future <bool> result2 = std::async(std::launch::async, &A::run, &a);
	std::future <bool> result3 = std::async(std::launch::async, &A::run, &a);
	std::future <bool> reuslt4 = std::async(std::launch::async, &A::signal, &a);
	result1.wait();
	result2.wait();
	result3.wait();
	reuslt4.wait();
	system("pause");
	return 0;
}