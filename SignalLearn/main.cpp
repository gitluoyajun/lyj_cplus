#include <cstdio>
#include "../Common/spinlock.hpp"
#include  "../Common/MyTime.h"
#include "signal.h"
#include <vector>
#include <string.h>
#include <algorithm>

using namespace std;

struct ListNode {
    int val;
    struct ListNode* next;
    ListNode(int x) :
        val(x), next(NULL) {
    }
};


  struct TreeNode {
      int val;
      TreeNode *left;
      TreeNode *right;
      TreeNode(int x) : val(x), left(NULL), right(NULL) {}
  };
 

int FindGreatestSumOfSubArray(vector<int> array);

vector<int> printListFromTailToHead(ListNode* head);
ListNode* Merge(ListNode* pHead1, ListNode* pHead2);
void replaceSpace(char* str, int length);
TreeNode* reConstructBinaryTree(vector<int> pre, vector<int> vin);
int main()
{
    //signal(SIGHUP, SIG_IGN);
    printf("hello from SignalLearn!\n");
   
    //vector<int> array({ -2, -8, -1, -5, -9 });
    //int ret = FindGreatestSumOfSubArray(array);
  /*  ListNode node1(1);
    ListNode node2(2);
    ListNode node3(3);
    ListNode node4(4);
    node1.next = &node2;
    node3.next = &node4;
    auto node = Merge(&node1,&node3);*/
   /* char str[128] = " we are ";
    replaceSpace(str,strlen(str));*/
    vector<int> pre({ 1,2,4,7,3,5,6,8 });
    vector<int> vin({ 4,7,2,1,5,3,8,6 });
    auto root = reConstructBinaryTree(pre, vin);
    return 0;
}

int FindGreatestSumOfSubArray(vector<int> array) {
    if (array.size() == 1)
        return array[0];
    int frist_num = array[0];
    int sub_sum = FindGreatestSumOfSubArray(vector<int>(array.begin() + 1, array.end()));
    int sum = frist_num + sub_sum;
    return sum > sub_sum ? sum : sub_sum > frist_num ? sub_sum : frist_num;
}



vector<int> printListFromTailToHead(ListNode* head) {
    vector<int> arry;
    if (head == nullptr || head->next == nullptr)
    {
        arry.push_back(head->val);
        return arry;
    }
    arry = printListFromTailToHead(head->next);
    arry.push_back(head->val);
    return arry;
}

ListNode* Merge(ListNode* pHead1, ListNode* pHead2)
{
    if (pHead1 == nullptr || pHead2 == nullptr)
        return pHead1 ? pHead1 : pHead2;
    ListNode* maxNode = nullptr;
    ListNode* minNode = nullptr;
    if (pHead1->val > pHead2->val)
    {
        maxNode = pHead1;
        minNode = pHead2;
    }
    else
    {
        maxNode = pHead2;
        minNode = pHead1;
    }
    ListNode* result = Merge(maxNode, minNode->next);
    minNode->next = result;
    return minNode;
}

void replaceSpace(char* str, int length) {
    if (*str == '\0')
        return;
    replaceSpace(str+1, length - 1);
    if (*str != ' ')
    {
        return;
    }
    else {
        std::string st("%20");
        st.append(str+1);
        strncpy(str, st.c_str(),st.size());
        return;
    }

}

TreeNode* reConstructBinaryTree(vector<int> pre, vector<int> vin) {
    if (pre.empty() || vin.empty())
        return nullptr;
    TreeNode* root = new TreeNode(pre[0]);

    //区分左右子数
    auto iter = find(vin.begin(), vin.end(), pre[0]);
    if (iter == vin.end())
        return nullptr;

    
    vector<int> left_vin(vin.begin(), iter);    //中序遍历,根左边的是中序遍历的左子树
    vector<int> right_vin(iter + 1, vin.end());
    vector<int>::iterator pre_right_iter = pre.begin() + left_vin.size() + 1;    //前序右节点开始位置

    vector<int> left_pre(pre.begin() + 1, pre_right_iter );    //前序遍历,根后面的长度为左子树个数的是前序遍历的左子树
    vector<int> right_pre(pre_right_iter, pre.end());

    auto left_root = reConstructBinaryTree(left_pre, left_vin);
    auto right_root = reConstructBinaryTree(right_pre, right_vin);
    root->left = left_root;
    root->right = right_root;
    return root;

}