#include <cstdio>
#include "MyStrFindMaxSub.h"
#include <iostream>
using namespace std;
int main()
{
    string str1 = "> Ping\r\n";
    string str2 = "\r\nGOLD";
    auto max_str =  MyStrFindMaxSub::FindMaxSubstr(str1, str2);
    cout << "字符串:" << str1 << " || " << str2 << " 最大子串是:" << max_str << endl;
    char c;
    cin >> c;
    return 0;
}