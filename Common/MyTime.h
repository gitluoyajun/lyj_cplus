#pragma once
#include <string>
#include <sys/time.h>
#include <cstdio>
void inline PrintfTime(std::string notify, timeval* t_start = nullptr )
{
	struct timeval t_end;
	struct timezone tz;
	gettimeofday(&t_end, &tz);
	if (t_start)
	{
		printf("%s:%ds %dus\n", notify.c_str(), t_end.tv_sec - t_start->tv_sec, t_end.tv_usec - t_start->tv_usec);
	}
	else
		printf("%s:%s %dus\n", notify.c_str(), t_end.tv_usec , t_start->tv_usec);
	
}

void inline sleep_ms(unsigned const int millisecond) {
	timeval timeout;
	timeout.tv_sec = millisecond / 1000;
	timeout.tv_usec = millisecond % 1000 * 1000;
	::select(0, nullptr, nullptr, nullptr, &timeout);
}

void inline sleep(unsigned const int second)
{
	sleep_ms(second * 1000);
}