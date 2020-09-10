#include <cstdio>
#include "while_if_systime.h"
#include <time.h>
#include <iostream>
#include <future>
#include <stack>
#include "errorTest.h"
#include "../Common/spinlock.hpp"
#include "../Common/lock_free_stack.hpp"
#include <unordered_map>
static union { char c[4]; unsigned long mylong; } endian_test = { { 'l', '?', '?', 'b' } };

#define ENDIANNESS ((char)endian_test.mylong)
using namespace std;

int main()
{
    std::future<int> future;
    std::mutex mutex;
    {
       
        lock_guard<std::mutex> lc(mutex);
        future  =  std::async(std::launch::async, [&mutex]() {
            lock_guard<std::mutex> lc(mutex);
            return 8;
            });
    }

    //cout << f1.get() << endl; //output: 8

    return 0;
}