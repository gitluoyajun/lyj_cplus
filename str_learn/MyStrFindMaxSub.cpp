#include "MyStrFindMaxSub.h"
#include <vector>
using namespace std;
std::string MyStrFindMaxSub::FindMaxSubstr(std::string str1, std::string str2)
{
	if (str1.size() < str2.size())
	{
		str1.swap(str2);
	}
	auto len1 = str1.size();
	auto len2 = str2.size();
	int  max_len = 0, start = 0;
	vector<vector<int>> dp(len1,vector<int>(len2,0));
	for (int l = 0; l < len1; l++)
	{
		for (int r = 0; r < len2; r++)
		{
			if (str1[l] == str2[r])
			{
				if (l > 0 && r > 0)
				{
					dp[l][r] = dp[l - 1][r - 1] + 1;
				}
				else
				{
					dp[l][r] = 1;
				}
					
				
				if (dp[l][r] >  max_len)
				{
					max_len = dp[l][r];
					start = l - max_len + 1;
				}
			}
		}
	}
	return str1.substr(start, max_len);
}
