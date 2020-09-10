#pragma once
#include <sys/time.h>
#include <cstdio>
void WhileIFTime()
{
	bool sign = true;
	struct timeval t_start, t_end;
	struct timezone tz;
	gettimeofday(&t_start, &tz);
	while (!sign)
	{

	}
	gettimeofday(&t_end, &tz);
	printf("while 花费时间秒:%d微秒:%d\n", t_end.tv_sec - t_start.tv_sec,t_end.tv_usec - t_start.tv_usec);

	gettimeofday(&t_start, &tz);
	if (sign)
	{

	}
	gettimeofday(&t_end, &tz);
	printf("if 花费时间秒:%d微秒:%d\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_usec - t_start.tv_usec);

}